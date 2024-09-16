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
        void    topic(Client* client, const std::string &topic);

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

        std::map<std::string, Client*> get_clients() const;
        std::map<std::string, Client*> get_operators() const;

        Client* get_client(const std::string &client_nickname) const;
        Client* get_operator(const std::string &operator_nickname) const;
        Client* get_invited_client(const std::string &operator_nickname) const;

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
        void remove_client(const Client* client);

        void add_operator(Client* client);
        void remove_operator(const Client* client);
        std::string get_channel_mode() const;

        void add_invited_client(Client* client);

        /*----------------------mode----------------------*/
        void add_channel_mode(const std::string &mode);
        void remove_channel_mode(const std::string &mode);

        std::string get_channel_params() const;
        std::string get_channel_generated_time() const;
        /*----------------------message----------------------*/
        void send_message(const Client &client, const std::string &message) const;

    private:
        std::map<std::string, Client*> clients;
        std::map<std::string, Client*> operators;
        std::map<std::string, Client*> invited_clients;

        std::string channel_name;
        std::string channel_topic;
        std::string channel_key;

        bool is_invite_only;
        bool is_topic_restrict;
        unsigned int channel_users_limit;
        unsigned int current_users_count;

        std::string channel_mode;
        std::string channel_generated_time;
};

#endif
