#include "Channel.hpp"

Channel::Channel() {
    // is_invite_only = false;
    // is_topic_restrict = false;
    // channel_users_limit = 0;
    current_users_count = 0;
}
Channel::Channel(std::string name) {
    channel_name = name;
    // is_invite_only = false;
    // is_topic_restrict = false;
    // channel_users_limit = 0;
    current_users_count = 0;
}

Client* Channel::get_clients(std::string client_nickname) {
    if (clients.find(client_nickname) == clients.end()) {
        return NULL;
    }
    return clients[client_nickname];
}

Client* Channel::get_operators(std::string operator_nickname) {
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

bool    Channel::get_option_topic() {
    return is_topic_restrict;
}

unsigned int    Channel::get_users_limit() {
    return channel_users_limit;
}

unsigned int Channel::get_current_users_count() {
    return current_users_count;
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