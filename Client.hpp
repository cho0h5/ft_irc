#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
    public:
        Client();
        Client(const int fd);

        int get_fd() const;
        void read_handler(const char *buf, const ssize_t n);

    private:
        int fd;
        std::string nickname;
        std::string username;
        std::string hostname;

        std::string read_buffer;
};
#endif
