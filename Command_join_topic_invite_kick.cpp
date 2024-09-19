#include "Error.hpp"
#include "Server.hpp"
#include "Client.hpp"

bool check_channel_name(const std::string &channel_name) {
    if (channel_name.size() < 2 || channel_name.size() > 50)
        return false;

    if (channel_name[0] != '#')
        return false;

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
        clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams(cmds[0]));
        return;
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
            if (channel.get_client(&clients_fd[fd]) == NULL)
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

        // incorrect channel name : ERR_BADCHANMASK, 476
        if (!check_channel_name(input_channels[i])) {
            clients_fd[fd].send_message(get_servername(), Error::err_badchanmask(clients_fd[fd].get_nickname(), input_channels[i]));
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
            clients_fd[fd].send_message(get_servername(), "MODE " + channel_name + " +t");
            clients_fd[fd].send_message(get_servername(), "353 " + clients_fd[fd].get_nickname() + " @ " + channel_name + " :@" + clients_fd[fd].get_nickname());
            clients_fd[fd].send_message(get_servername(), "366 " + clients_fd[fd].get_nickname() + " " + channel_name + " :End of /NAMES list");
            channels[channel_name].add_channel_mode("t");
            continue;
        }

        Channel& channel = iter->second;
        // incorrect key : ERR_BADCHANNELKEY, 475
        if (keys[i] != channel.get_key()) {
            clients_fd[fd].send_message(get_servername(), Error::err_badchannelkey(clients_fd[fd].get_nickname(), channel.get_name()));
            continue;
        }

        // channel is full : ERR_CHANNELISFULL, 471
        if (channel.get_current_users_count() == channel.get_users_limit()) {
            clients_fd[fd].send_message(get_servername(), Error::err_channelisfull(clients_fd[fd].get_nickname(), channel.get_name()));
            continue;
        }

        // invite only : ERR_INVITEONLYCHAN, 473
        if (channel.get_option_invite_only() && channel.get_invited_client(&clients_fd[fd]) == NULL) {
            clients_fd[fd].send_message(get_servername(), Error::err_inviteonlychan(clients_fd[fd].get_nickname(), channel.get_name()));
            continue;
        }

        std::set<Client*> joined_users = channel.get_clients();
        for (std::set<Client*>::iterator iter = joined_users.begin(); iter != joined_users.end(); iter++)
            (*iter)->send_message(clients_fd[fd].get_identifier(), "JOIN " + channel.get_name());


        channel.add_client(&clients_fd[fd]);
        channel.set_current_users_count(channel.get_current_users_count() + 1);

        clients_fd[fd].send_message(clients_fd[fd].get_identifier(), "JOIN " + channel.get_name());

        if (!iter->second.get_topic().empty()) {
            Channel& channel = iter->second;
            clients_fd[fd].send_message(get_servername(), "332 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " :" + channel.get_topic());
            clients_fd[fd].send_message(get_servername(), "333 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " " + channel.get_channel_topic_set_member() + " " + channel.get_channel_topic_set_time());
        }

        joined_users = channel.get_clients();
        std::set<Client*> joined_operators = channel.get_operators();
        std::string joined_users_str = "";
        for (std::set<Client*>::iterator iter = joined_users.begin(); iter != joined_users.end(); iter++) {
            if (joined_operators.find(*iter) != joined_operators.end())
                joined_users_str += "@";
            joined_users_str += (*iter)->get_nickname() + " ";
        }
        clients_fd[fd].send_message(get_servername(), "353 " + clients_fd[fd].get_nickname() + " @ " + channel.get_name() + " :" + joined_users_str);
        clients_fd[fd].send_message(get_servername(), "366 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " :End of /NAMES list");
    }
}

#include <iostream>
void Server::command_topic(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

    // not enough parameters : ERR_NEEDMOREPARAMS, 461
    if (cmds.size() < 2) {
        clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams(cmds[0]));
        return;
    }

    // not TOPIC command
    if (cmds[0] != "TOPIC" || 3 < cmds.size()) {
        return;
    }

    std::map<std::string, Channel>::iterator iter = channels.find(cmds[1]);
    // no such channel : ERR_NOSUCHCHANNEL, 403
    if (iter == channels.end()) {
        clients_fd[fd].send_message(get_servername(), Error::err_nosuchchannel(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

    // not in channel : ERR_NOTONCHANNEL, 442
    if (iter->second.get_client(&clients_fd[fd]) == NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_notonchannel(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

    // get topic
    if (cmds.size() == 2) {
        if (iter->second.get_topic() == "") {
            // no topic : RPL_NOTOPIC, 331
            clients_fd[fd].send_message(get_servername(), "331 " + clients_fd[fd].get_nickname() + " " + iter->second.get_name() + " :No topic is set");
            return;
        }

        Channel& channel = iter->second;
        clients_fd[fd].send_message(get_servername(), "332 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " :" + channel.get_topic());
        return;
    }

    // set topic
    if (iter->second.get_option_topic_restrict() && iter->second.get_operator(&clients_fd[fd]) == NULL) {
        // not channel operator : ERR_CHANOPRIVSNEEDED, 482
        clients_fd[fd].send_message(get_servername(), Error::err_chanoprivsneeded(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }
    iter->second.set_channel_topic(cmds[2]);
    std::cout << "topic: " << iter->second.get_topic() << std::endl;
    iter->second.set_channel_topic_set_member(clients_fd[fd].get_identifier());
    iter->second.set_channel_topic_set_time();

    // send changed topic to all clients in the channel
    Channel& channel = iter->second;
    std::set<Client*> joined_users = iter->second.get_clients();
    for (std::set<Client*>::iterator iter = joined_users.begin(); iter != joined_users.end(); iter++)
        (*iter)->send_message(clients_fd[fd].get_identifier(), "TOPIC " + channel.get_name() + " :" + channel.get_topic());
}

void Server::command_invite(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }
    // not enough parameters : ERR_NEEDMOREPARAMS, 461
    if (cmds.size() < 3) {
        clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams(cmds[0]));
        return;
    }

    if (cmds[0] != "INVITE" || 3 < cmds.size()) {
        // not INVITE command
        return;
    }

    std::map<std::string, Channel>::iterator iter = channels.find(cmds[2]);
    Channel& channel = iter->second;
    // no such channel : ERR_NOSUCHCHANNEL, 403
    if (iter == channels.end()) {
        clients_fd[fd].send_message(get_servername(), Error::err_nosuchchannel(clients_fd[fd].get_nickname(), cmds[2]));
        return;
    }

    // not in channel : ERR_NOTONCHANNEL, 442
    if (channel.get_client(&clients_fd[fd]) == NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_notonchannel(clients_fd[fd].get_nickname(), cmds[2]));
        return;
    }

    // not channel operator : ERR_CHANOPRIVSNEEDED, 482
    if (channel.get_operator(&clients_fd[fd]) == NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_chanoprivsneeded(clients_fd[fd].get_nickname(), cmds[2]));
        return;
    }

    std::map<std::string, Client*>::iterator invitee = clients_nickname.find(cmds[1]);
    // no such nick : ERR_NOSUCHNICK, 401
    if (invitee == clients_nickname.end()) {
        clients_fd[fd].send_message(get_servername(), Error::err_nosuchnick(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

    // already in channel : ERR_USERONCHANNEL, 443
    if (channel.get_client(invitee->second) != NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_useronchannel(clients_fd[fd].get_nickname(), invitee->first, cmds[2]));
        return;
    }

    // invite success
    clients_fd[fd].send_message(get_servername(), "341 " + clients_fd[fd].get_nickname() + " " + invitee->second->get_nickname() + " " + channel.get_name());
    invitee->second->send_message(clients_fd[fd].get_identifier(), "INVITE " + invitee->second->get_nickname() + " " + channel.get_name());
    channel.add_invited_client(invitee->second);

}

void Server::command_kick(const int fd, std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

    // not enough parameters : ERR_NEEDMOREPARAMS, 461
    if (cmds.size() < 3) {
        clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams(cmds[0]));
        return;
    }

    if (cmds[0] != "KICK" || 4 < cmds.size()) {
        // not KICK command
        return;
    }

    std::map<std::string, Channel>::iterator iter = channels.find(cmds[1]);
    // no such channel : ERR_NOSUCHCHANNEL, 403
    if (iter == channels.end()) {
        clients_fd[fd].send_message(get_servername(), Error::err_nosuchchannel(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

    Channel& channel = iter->second;
    // not in channel : ERR_NOTONCHANNEL, 442
    if (channel.get_client(&clients_fd[fd]) == NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_notonchannel(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

    // not channel operator : ERR_CHANOPRIVSNEEDED, 482
    if (channel.get_operator(&clients_fd[fd]) == NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_chanoprivsneeded(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

    std::vector<std::string> kicked_nicknames;
    if (cmds[2].find(',') == std::string::npos) {
        kicked_nicknames.push_back(cmds[2]);
    }
    else {
        while (cmds[2].find(',') != std::string::npos) {
            const size_t pos = cmds[2].find(',');
            kicked_nicknames.push_back(cmds[2].substr(0, pos));
            cmds[2].erase(0, pos + 1);
        }
    }
    std::string reason = "";
    if (cmds.size() == 4)
        reason = " :" + cmds[3];

    for (size_t i = 0; i < kicked_nicknames.size(); i++) {
        std::map<std::string, Client*>::iterator kicked_client = clients_nickname.find(kicked_nicknames[i]);
        // no such nick : ERR_NOSUCHNICK, 401
        if (kicked_client == clients_nickname.end()) {
            clients_fd[fd].send_message(get_servername(), Error::err_nosuchnick(clients_fd[fd].get_nickname(), kicked_nicknames[i]));
            continue;
        }

        // not in channel : ERR_USERNOTINCHANNEL, 441
        if (channel.get_client(kicked_client->second) == NULL) {
            // 채널에 없는 클라이언트에게만 보냄
            clients_nickname[kicked_client->first]->send_message(get_servername(), Error::err_usernotinchannel(clients_fd[fd].get_nickname(), kicked_client->first, channel.get_name()));
            continue;
        }

        // kick success
        std::set<Client*> joined_users = channel.get_clients();
        for (std::set<Client*>::iterator iter = joined_users.begin(); iter != joined_users.end(); iter++)
            (*iter)->send_message(get_servername(), "KICK " + channel.get_name() + " " + kicked_client->first + reason);

        if (channel.get_operator(kicked_client->second) != NULL)
            channel.remove_operator(kicked_client->second);
        channel.remove_client(kicked_client->second);
        channel.set_current_users_count(channel.get_current_users_count() - 1);
    }
    prune_channel();
}
