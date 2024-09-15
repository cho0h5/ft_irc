#include "Error.hpp"
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
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

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
            if (channel.get_client(clients_fd[fd].get_nickname()) == NULL)
                continue;

            channel.remove_client(&clients_fd[fd]);
            channel.remove_operator(&clients_fd[fd]);
            channel.set_current_users_count(channel.get_current_users_count() - 1);
        }
        return;
    }


    // get channel names
    std::vector<std::string> input_channels;
    if (cmds[1].find(',') == std::string::npos) {
        input_channels.push_back(cmds[1]);
    }
    else {
        while (cmds[1].find(',') != std::string::npos) {
            const size_t pos = cmds[1].find(',');
            input_channels.push_back(cmds[1].substr(0, pos));
            cmds[1].erase(0, pos + 1);
        }
    }

    std::vector<std::string> keys(input_channels.size(), "");
    // get channel keys
    if (cmds.size() == 3) {
        if (cmds[1].find(',') == std::string::npos) {
            keys[0] = cmds[2];
        }
        else {
            size_t idx = 0;
            while (cmds[2].find(',') != std::string::npos) {
                const size_t pos = cmds[2].find(',');
                keys[idx++] = cmds[2].substr(0, pos);
                cmds[2].erase(0, pos + 1);
            }
        }
    }


    // join channels
    for (size_t i = 0; i < input_channels.size(); i++) {

        if (!check_channel_name(input_channels[i])) {
            // incorrect channel name : ERR_BADCHANMASK, 476
            // send_error(fd, 476);
            return;
        }

        // find channel names in server
        std::map<std::string, Channel>::iterator iter = channels.find(input_channels[i]);

        // if there is no channel, create new channel
        if (iter == channels.end()) {
            std::string channel_name = input_channels[i];
            channels[channel_name] = Channel(channel_name);

            if (keys[i] != "") {
                Channel& channel = channels[channel_name];
                channel.set_channel_key(keys[i]);
            }


            channels[channel_name].add_client(&clients_fd[fd]);
            channels[channel_name].add_operator(&clients_fd[fd]);
            clients_fd[fd].send_message(clients_fd[fd].get_identifier(), "JOIN " + channel_name);
            clients_fd[fd].send_message(get_servername(), "MODE " + channel_name + " +Cnst");
            clients_fd[fd].send_message(get_servername(), "353 " + clients_fd[fd].get_nickname() + " @ " + channel_name + " :@" + clients_fd[fd].get_nickname());
            clients_fd[fd].send_message(get_servername(), "366 " + clients_fd[fd].get_nickname() + " " + channel_name + " :End of /NAMES list");
            continue;
        }

    
        Channel& channel = iter->second;
        if (keys[i] != channel.get_key()) {
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

        std::map<std::string, Client*> joined_users = channel.get_clients();
        for (std::map<std::string, Client*>::iterator iter = joined_users.begin(); iter != joined_users.end(); iter++)
            iter->second->send_message(clients_fd[fd].get_identifier(), "JOIN " + channel.get_name());

        channel.add_client(&clients_fd[fd]);
        channel.set_current_users_count(channel.get_current_users_count() + 1);

        clients_fd[fd].send_message(clients_fd[fd].get_identifier(), "JOIN " + channel.get_name());
        joined_users = channel.get_clients();
        std::map<std::string, Client*> joined_operators = channel.get_operators();
        std::string joined_users_str = "";
        for (std::map<std::string, Client*>::iterator iter = joined_users.begin(); iter != joined_users.end(); iter++) {
            if (joined_operators.find(iter->first) != joined_operators.end())
                joined_users_str += "@";
            joined_users_str += iter->first + " ";
        }
        clients_fd[fd].send_message(get_servername(), "353 " + clients_fd[fd].get_nickname() + " @ " + channel.get_name() + " :" + joined_users_str);
        clients_fd[fd].send_message(get_servername(), "366 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " :End of /NAMES list");

    }
}


void Server::command_topic(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }
      (void)fd;
    (void)cmds;


}

void Server::command_invite(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

    (void)fd;
    (void)cmds;
}

void Server::command_kick(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

    (void)fd;
    (void)cmds;
}
