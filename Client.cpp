#include "Client.hpp"

Client::Client() {
}

Client::Client(const int fd) : fd(fd) {
}

int Client::get_fd() const {
    return fd;
}
