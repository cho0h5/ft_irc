#include "Channel.hpp"
#include <chrono>

Channel::Channel() : channel_mode("+") {
    is_invite_only = false;
    is_topic_restrict = true;
    channel_users_limit = 500;
    current_users_count = 1;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(now);

    channel_generated_time = std::to_string(unix_timestamp);
}
Channel::Channel(std::string name) : channel_mode("+") {
    channel_name = name;
    is_invite_only = false;
    is_topic_restrict = true;
    channel_users_limit = 500;
    current_users_count = 1;

     std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(now);

    channel_generated_time = std::to_string(unix_timestamp);
}

std::map<std::string, Client*> Channel::get_clients() const {
    return clients;
}

std::map<std::string, Client*> Channel::get_operators() const {
    return operators;
}


Client* Channel::get_client(const std::string &client_nickname) const {
    std::map<std::string, Client*>::const_iterator it = clients.find(client_nickname);
    if (it == clients.end()) {
        return NULL;
    }
    return it->second;
}

Client* Channel::get_operator(const std::string &operator_nickname) const {
    std::map<std::string, Client*>::const_iterator it = operators.find(operator_nickname);
    if (it == operators.end()) {
        return NULL;
    }
    return it->second;
}

Client* Channel::get_invited_client(const std::string &operator_nickname) const {
    std::map<std::string, Client*>::const_iterator it = invited_clients.find(operator_nickname);
    if (it == invited_clients.end()) {
        return NULL;
    }
    return it->second;
}


std::string Channel::get_name() const {
    return channel_name;
}

std::string Channel::get_topic() const {
    return channel_topic;
}

std::string Channel::get_key() const {
    return channel_key;
}


bool    Channel::get_option_invite_only() const {
    return is_invite_only;
}

bool    Channel::get_option_topic_restrict() const {
    return is_topic_restrict;
}

unsigned int    Channel::get_users_limit() const {
    return channel_users_limit;
}

unsigned int Channel::get_current_users_count() const {
    return current_users_count;
}

void Channel::set_channel_key(const std::string &key) {
    channel_key = key;
}

void Channel::set_channel_topic(const std::string &topic) {
    channel_topic = topic;
}

void Channel::set_current_users_count(unsigned int count) {
    current_users_count = count;
}

void Channel::add_client(Client* client) {
    clients[client->get_nickname()] = client;
}

void Channel::remove_client(const Client* client) {
    if (client == NULL)
        return;
    clients.erase(client->get_nickname());
}

void Channel::add_operator(Client* client) {
    if (client == NULL)
        return;
    operators[client->get_nickname()] = client;
}

void Channel::remove_operator(const Client* client) {
    if (client == NULL)
        return;
    operators.erase(client->get_nickname());
}

void Channel::add_invited_client(Client* client) {
    if (client == NULL)
        return;
    invited_clients[client->get_nickname()] = client;
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


void Channel::add_channel_mode(const std::string &mode) {
    for (size_t idx = 0; idx < mode.size(); idx++) {
        if (channel_mode.find(mode[idx]) == std::string::npos)
            channel_mode += mode[idx];
    }
}

void Channel::remove_channel_mode(const std::string &mode) {
    for (size_t idx = 0; idx < mode.size(); idx++) {
        if (channel_mode.find(mode[idx]) != std::string::npos)
            channel_mode.erase(channel_mode.find(mode[idx]), 1);
    }
}

std::string Channel::get_channel_mode() const {
    return channel_mode;
}

void Channel::set_option_i() {
    is_invite_only = true;
}

void Channel::unset_option_i() {
    is_invite_only = false;
}

void Channel::set_option_t() {
    is_topic_restrict = true;
}

void Channel::unset_option_t() {
    is_topic_restrict = false;
}

void Channel::set_channel_users_limit(unsigned int limit) {
    channel_users_limit = limit;
}

std::string Channel::get_channel_params() const {
    std::string params = "";
    for (size_t idx = 0; idx < channel_mode.size(); idx++) {
        if (channel_mode[idx] == 'k')
            params += channel_key + " ";
        else if (channel_mode[idx] == 'l')
            params += "l" + std::to_string(channel_users_limit);
    }
    return params;
}

std::string Channel::get_channel_generated_time() const {
    return channel_generated_time;
}
