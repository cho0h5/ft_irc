#include "Client.hpp"

Client::Client() {
}

Client::Client(const int fd) : fd(fd) {
}

int Client::get_fd() const {
    return fd;
}

void Client::read_handler(const char *buf, const ssize_t n) {
    read_buffer.append(buf, n);

    size_t pos;
    while ((pos = read_buffer.find("\r\n")) != std::string::npos) {
        std::string line = read_buffer.substr(0, pos);
        // std::cout << "Extracted line: " << line << std::endl;
        read_buffer.erase(0, pos + 2);
    }
}
