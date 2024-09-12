#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

#include "Client.hpp"

class Channel {
    public:
        void    kick(Client* client);
        void    invite(Client* client);
        void    topic(Client* client, const std::string& topic);

        void    set_option_i();
        void    set_option_t();
        void    set_option_k(std::string key);
        void    set_option_o(Client* client);
        void    set_option_l(unsigned int users_limit);

        void    unset_option_i();
        void    unset_option_t();
        void    unset_option_k();
        void    unset_option_o(Client* client);
        void    unset_option_l();

    private:
        std::vector<Client*> clients;
        std::vector<Client*> operators;

        std::string channel_name;
        std::string channel_topic;

        bool is_invite_only;
        bool is_topic_restrict;
        std::string channel_key;
        unsigned int channel_users_limit;


};

#endif
