#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
    public:
        Client();
        Client(const int fd);

        int get_fd() const;

    private:

        int fd;
        std::string nickname;
        std::string username;
        std::string hostname;
};
#endif
