#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
    public:
        Client();
        Client(const int fd);

        int get_fd() const;
        std::string get_nickname() const;
        std::string get_username() const;
        std::string get_realname() const;
        std::string get_hostname() const;
        void set_nickname(const std::string &nickname);
        void set_username(const std::string &username);
        void set_realname(const std::string &username);
        void set_hostname(const std::string &hostname);

        void read_handler(const char *buf, const ssize_t n);

    private:
        int fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string hostname;

        std::string read_buffer;
};
#endif
