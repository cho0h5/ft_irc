#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>

#include "Client.hpp"

class Channel {
    public:
        Channel();
        Channel(std::string name);

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
        void    unset_option_k(std::string key);
        void    unset_option_o(Client* client);
        void    unset_option_l();

        std::map<std::string, Client*> get_clients();
        std::map<std::string, Client*> get_operators();

        Client* get_client(std::string client_nickname);
        Client* get_operator(std::string operator_nickname);

        std::string get_name();
        std::string get_topic();
        std::string get_key();

        bool         get_option_invite_only();
        bool         get_option_topic();
        unsigned int get_users_limit();
        unsigned int get_current_users_count();

        void set_channel_key(std::string key);

        void set_current_users_count(unsigned int count);
        void add_client(Client* client);
        void remove_client(Client* client);
        void add_operator(Client* client);
        void remove_operator(Client* client);

    private:
        std::map<std::string, Client*> clients;
        std::map<std::string, Client*> operators;

        std::string channel_name;
        std::string channel_topic;
        std::string channel_key;

        bool is_invite_only;
        bool is_topic_restrict;
        unsigned int channel_users_limit;
        unsigned int current_users_count;


};

#endif
