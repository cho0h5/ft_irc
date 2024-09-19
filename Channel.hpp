#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

#include "Client.hpp"

class Channel {
    public:
        Channel();
        Channel(const std::string &name);

        void    set_option_i();
        void    set_option_t();
        // void    set_option_k(std::string key);
        // void    set_option_o(Client* client);
        // void    set_option_l(unsigned int users_limit);

        void    unset_option_i();
        void    unset_option_t();
        // void    unset_option_k(std::string key);
        // void    unset_option_o(Client* client);
        // void    unset_option_l();

        void set_channel_users_limit(const unsigned int limit);

        std::set<Client*> get_clients() const;
        std::set<Client*> get_operators() const;

        Client* get_client(Client *client) const;
        Client* get_operator(Client *op) const;
        Client* get_invited_client(Client *invited) const;

        std::string get_name() const;
        std::string get_topic() const;
        std::string get_key() const;

        bool         get_option_invite_only() const;
        bool         get_option_topic_restrict() const;
        unsigned int get_users_limit() const;
        unsigned int get_current_users_count() const;

        void set_channel_key(const std::string &key);
        void set_channel_topic(const std::string &topic);


        void set_current_users_count(const unsigned int count);
        void add_client(Client* client);
        void remove_client(Client* client);

        void add_operator(Client* client);
        void remove_operator(Client* client);
        std::string get_channel_mode() const;

        void add_invited_client(Client* client);

        /*----------------------mode----------------------*/
        void add_channel_mode(const std::string &mode);
        void remove_channel_mode(const std::string &mode);

        std::string get_channel_params() const;
        std::string get_channel_generated_time() const;

        void set_channel_topic_set_time();
        std::string get_channel_topic_set_time() const;
        /*----------------------message----------------------*/
        void send_message(const Client &client, const std::string &message) const;

    private:
        std::set<Client*> clients;
        std::set<Client*> operators;
        std::set<Client*> invited_clients;

        std::string channel_name;
        std::string channel_topic;
        std::string channel_key;

        bool is_invite_only;
        bool is_topic_restrict;
        unsigned int channel_users_limit;
        unsigned int current_users_count;

        std::string channel_mode;
        std::string channel_generated_time;
        std::string channel_topic_set_time;
};

#endif
