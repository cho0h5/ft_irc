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
        void set_nickname(const std::string &nickname);
        void set_username(const std::string &username);
        void set_realname(const std::string &realname);
        void set_hostname(const std::string &hostname);

        int read_handler(Server *server);
        void write_handler();

        bool is_write_buffer_empty() const;
        void send_message(const std::string message);

        bool is_connect_possible() const;

    private:
        int fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string hostname;

        std::string read_buffer;
        std::string write_buffer;
};
#endif
