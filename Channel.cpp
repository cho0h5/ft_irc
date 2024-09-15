#include "Channel.hpp"

Channel::Channel() {
    // channel_name = name;
    // is_invite_only = false;
    // is_topic_restrict = false;
    // channel_users_limit = 0;
    current_users_count = 0;
}

std::map<std::string, Client*>& Channel::get_clients() {
    return clients;
}

std::map<std::string, Client*>& Channel::get_operators() {
    return operators;
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
    clients.erase(client->get_nickname());
}