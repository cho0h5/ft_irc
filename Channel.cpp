#include "Channel.hpp"

Channel::Channel() {
    is_invite_only = false;
    is_topic_restrict = true;
    channel_users_limit = 0;
    current_users_count = 1;
}
Channel::Channel(std::string name) {
    channel_name = name;
    is_invite_only = false;
    is_topic_restrict = true;
    channel_users_limit = 0;
    current_users_count = 1;
}

std::map<std::string, Client*> Channel::get_clients() {
    return clients;
}

std::map<std::string, Client*> Channel::get_operators() {
    return operators;
}


Client* Channel::get_client(std::string client_nickname) const {
    std::map<std::string, Client*>::const_iterator it = clients.find(client_nickname);
    if (it == clients.end()) {
        return NULL;
    }
    return it->second;
}

Client* Channel::get_operator(std::string operator_nickname) {
    if (operators.find(operator_nickname) == operators.end()) {
        return NULL;
    }
    return operators[operator_nickname];
}


std::string Channel::get_name() {
    return channel_name;
}

std::string Channel::get_topic() {
    return channel_topic;
}

std::string Channel::get_key() {
    return channel_key;
}


bool    Channel::get_option_invite_only() {
    return is_invite_only;
}

bool    Channel::get_option_topic_restrict() {
    return is_topic_restrict;
}

unsigned int    Channel::get_users_limit() {
    return channel_users_limit;
}

unsigned int Channel::get_current_users_count() {
    return current_users_count;
}

void Channel::set_channel_key(std::string key) {
    channel_key = key;
}

void Channel::set_channel_topic(std::string topic) {
    channel_topic = topic;
}

void Channel::set_current_users_count(unsigned int count) {
    current_users_count = count;
}

void Channel::add_client(Client* client) {
    clients[client->get_nickname()] = client;
}

void Channel::remove_client(Client* client) {
    if (client == NULL)
        return;
    clients.erase(client->get_nickname());
}

void Channel::add_operator(Client* client) {
    if (client == NULL)
        return;
    operators[client->get_nickname()] = client;
}

void Channel::remove_operator(Client* client) {
    if (client == NULL)
        return;
    operators.erase(client->get_nickname());
}

void Channel::send_message(const Client &client, const std::string &message) const {
    const std::string sender_identifier = client.get_identifier();

    for (std::map<std::string, Client*>::const_iterator it = clients.begin(); it != clients.end(); it++) {
        if (it->second->get_fd() == client.get_fd()) {   // 송신자에게는 보내지 않음
            continue;
        }

        it->second->send_message(sender_identifier, message);
    }
}
