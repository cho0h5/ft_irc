#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Server;

class Client {
    public:
        Client();
        Client(const int fd, const std::string &ip);

        int get_fd() const;
        std::string get_nickname() const;
        std::string get_username() const;
        std::string get_realname() const;
        std::string get_hostname() const;
        std::string get_identifier() const;
        bool get_is_registered() const;
        bool get_is_authorized() const;
        void set_nickname(const std::string &nickname);
        void set_username(const std::string &username);
        void set_realname(const std::string &realname);
        void set_hostname(const std::string &hostname);
        void set_is_registered();
        void set_is_authorized();

        int read_handler(Server *server);
        int write_handler();

        bool is_write_buffer_empty() const;
        void send_message(const std::string &sender, const std::string &message);

        bool is_connect_possible() const;

    private:
        int fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string hostname;

        bool is_registered;
        bool is_authorized;

        std::string read_buffer;
        std::string write_buffer;
};
#endif
