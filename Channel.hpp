#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

#include "Client.hpp"

class Channel {
    public:

    private:
        std::vector<Client*> clients;
        std::vector<Client*> operators;

        std::string channel_name;
        std::string channel_topic;
        std::string channel_key; // password

        // int channel_users_limit;
        // bool is_topic_restrict;
        // bool is_invite_only;
        
};

#endif