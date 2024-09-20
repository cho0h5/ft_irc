#include "Channel.hpp"
#include "Client.hpp"
#include <ctime>
#include <sstream>

Channel::Channel() : channel_mode("+") {
    is_invite_only = false;
    is_topic_restrict = true;
    channel_users_limit = 500;
    current_users_count = 1;

    std::time_t now = std::time(0);

    std::stringstream ss;
    ss << now;
    channel_generated_time = ss.str();
}

Channel::Channel(const std::string &name) : channel_mode("+") {
    channel_name = name;
    is_invite_only = false;
    is_topic_restrict = true;
    channel_users_limit = 500;
    current_users_count = 1;

    std::time_t now = std::time(0);
    std::stringstream ss;
    ss << now;
    channel_generated_time = ss.str();
}

void Channel::set_channel_topic_set_time() {
    std::time_t now = std::time(0);

    std::stringstream ss;
    ss << now;
    channel_topic_set_time = ss.str();
}

std::string Channel::get_channel_topic_set_time() const {
    return channel_topic_set_time;
}

void Channel::set_channel_topic_set_member(const std::string &member_identifier) {
    channel_topic_set_member = member_identifier;
}

std::string Channel::get_channel_topic_set_member() const {
    return channel_topic_set_member;
}

std::set<Client*> Channel::get_clients() const {
    return clients;
}

std::set<Client*> Channel::get_operators() const {
    return operators;
}


Client* Channel::get_client(Client *client) const {
    std::set<Client*>::const_iterator it = clients.find(client);
    if (it == clients.end()) {
        return NULL;
    }
    return *it;
}

Client* Channel::get_operator(Client *op) const {
    std::set<Client*>::const_iterator it = operators.find(op);
    if (it == operators.end()) {
        return NULL;
    }
    return *it;
}

Client* Channel::get_invited_client(Client *invited) const {
    std::set<Client*>::const_iterator it = invited_clients.find(invited);
    if (it == invited_clients.end()) {
        return NULL;
    }
    return *it;
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
    if (client == NULL)
        return;
    clients.insert(client);
}

void Channel::remove_client(Client* client) {
    if (client == NULL)
        return;
    clients.erase(client);
}

void Channel::add_operator(Client* client) {
    if (client == NULL)
        return;
    operators.insert(client);
}

void Channel::remove_operator(Client* client) {
    if (client == NULL)
        return;
    operators.erase(client);
}

void Channel::add_invited_client(Client* client) {
    if (client == NULL)
        return;
    invited_clients.insert(client);
}

void Channel::send_message(const Client &client, const std::string &message) const {
    const std::string sender_identifier = client.get_identifier();

    for (std::set<Client*>::const_iterator it = clients.begin(); it != clients.end(); it++) {
        if ((*it)->get_fd() == client.get_fd()) {   // 송신자에게는 보내지 않음
            continue;
        }

        (*it)->send_message(sender_identifier, message);
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
        else if (channel_mode[idx] == 'l') {
            std::stringstream ss;
            ss << channel_users_limit;
            if (params.size())
                params += " " + ss.str();
            else
                params += ss.str();
        }
    }
    return params;
}

std::string Channel::get_channel_generated_time() const {
    return channel_generated_time;
}
