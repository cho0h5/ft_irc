#include "Client.hpp"

Client::Client() {
}

Client::Client(const int fd) : fd(fd) {
}

int Client::get_fd() const {
    return fd;
}

std::string Client::get_nickname() const {
    return nickname;
}

std::string Client::get_username() const {
    return username;
}

std::string Client::get_realname() const {
    return realname;
}

std::string Client::get_hostname() const {
    return hostname;
}

void Client::set_nickname(const std::string &nickname) {
    this->nickname = nickname;
}

void Client::set_username(const std::string &username) {
    this->username = username;
}

void Client::set_realname(const std::string &realname) {
    this->realname = realname;
}

void Client::set_hostname(const std::string &hostname) {
    this->hostname = hostname;
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
