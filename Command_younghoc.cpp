#include "Server.hpp"
#include "Client.hpp"

bool check_channel_name(const std::string &channel_name) {
    if (channel_name.size() < 2 || channel_name.size() > 50) {
        return false;
    }

    if (channel_name[0] != '#') {
        return false;
    }

    for (size_t i = 1; i < channel_name.size(); i++) {
        if (!(('0' <= channel_name[i] && channel_name[i] <= '9')
            || ('a' <= channel_name[i] && channel_name[i] <= 'z')
            || ('A' <= channel_name[i] && channel_name[i] <= 'Z')
            || channel_name[i] == '-' || channel_name[i] == '_')) {
                return false;
        }
    }
    return true;
}


void Server::command_join(const int fd, std::vector<std::string> &cmds) {
    
    // not enough parameters : ERR_NEEDMOREPARAMS, 461
    if (cmds.size() < 2) {
        // send_error(fd, 461);
    }

    // too many parameters : 뭔 에러코드지?
    if (3 < cmds.size()) {
        return;
    }

    // not JOIN command
    if (cmds[0] != "JOIN") {
        return;
    }

    // 참여중인 모든 채널에서 나감
    if (cmds.size() == 2 && cmds[0] == "JOIN" && cmds[1] == "0") {
        for (std::map<std::string, Channel>::iterator iter = channels.begin(); iter != channels.end(); iter++) {
            Channel channel = iter->second;

            // 해당 클라이언트가 채널에 속해있는지 확인
            if (channel.get_clients(clients_fd[fd].get_nickname()) == NULL) 
                continue;
            
            channel.remove_client(&clients_fd[fd]);
            channel.remove_operator(&clients_fd[fd]);
            channel.set_current_users_count(channel.get_current_users_count() - 1);
        }
        return;
    }


    // get channel names
    std::vector<std::string> input_channels;
    while (cmds[1].find(',') != std::string::npos) {
        const size_t pos = cmds[1].find(',');
        input_channels.push_back(cmds[1].substr(0, pos));
        cmds[1].erase(0, pos + 1);
    }

    std::vector<std::string> keys(input_channels.size(), "");
    // get channel keys
    if (cmds.size() == 3) {
        while (cmds[2].find(',') != std::string::npos) {
            const size_t pos = cmds[2].find(',');
            keys.push_back(cmds[2].substr(0, pos));
            cmds[2].erase(0, pos + 1);
        }
    }

    // join channels
    for (size_t i = 0; i < input_channels.size(); i++) {
        
        if (!check_channel_name(input_channels[i])) {
            // incorrect channel name : ERR_BADCHANMASK, 476
            // send_error(fd, 476);
            return;
        }

        std::map<std::string, Channel>::iterator iter = channels.find(input_channels[i]);

        // find channel names in server
        if (iter == channels.end()) {
            // no such channel : ERR_NOSUCHCHANNEL, 403
            // send_error(fd, 403);
            return;
        }

        Channel channel = iter->second;
        if (keys[i] != "" && keys[i] != channel.get_key()) {
            // incorrect key : ERR_BADCHANNELKEY, 475
            // send_error(fd, 475);
            continue;
        }

        if (channel.get_current_users_count() == channel.get_users_limit()) {
            // channel is full : ERR_CHANNELISFULL, 471
            // send_error(fd, 471);
            continue;
        }

        if (channel.get_option_invite_only()) {
            // invite only : ERR_INVITEONLYCHAN, 473
            // send_error(fd, 473);
            continue;
        }

        channel.add_client(&clients_fd[fd]);
        channel.set_current_users_count(channel.get_current_users_count() + 1);
    }

}


void Server::command_topic(const int fd, const std::vector<std::string> &cmds) {
    (void)fd;
    (void)cmds;
}

void Server::command_invite(const int fd, const std::vector<std::string> &cmds) {
    (void)fd;
    (void)cmds;
}

void Server::command_kick(const int fd, const std::vector<std::string> &cmds) {
    (void)fd;
    (void)cmds;
}
