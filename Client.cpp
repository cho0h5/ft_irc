#include "Server.hpp"
#include "Client.hpp"
#include <unistd.h>
#include <iostream>

Client::Client() : is_registered(false), is_authorized(false) {
}

Client::Client(const int fd, const std::string &ip) : fd(fd), hostname(ip), is_registered(false), is_authorized(false) {
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

std::string Client::get_identifier() const {
    return get_nickname() + "!~" + get_username() + "@" + get_hostname();
}

bool Client::get_is_registered() const {
    return is_registered;
}

bool Client::get_is_authorized() const {
    return is_authorized;
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

void Client::set_is_registered() {
    is_registered = true;
}

void Client::set_is_authorized() {
    is_authorized = true;
}

int Client::read_handler(Server *server) {
    char buf[1024];
    const int n = read(fd, buf, 1024);
    if (n <= 0) {
		return -1;
	}
    read_buffer.append(buf, n);

    size_t pos;
    while ((pos = read_buffer.find("\r\n")) != std::string::npos) {
        std::string line = read_buffer.substr(0, pos);
        server->command_parsing(fd, line);
        read_buffer.erase(0, pos + 2);
    }

    return 0;
}

int Client::write_handler() {
    const ssize_t n = write(fd, write_buffer.c_str(), write_buffer.size());
    write_buffer.erase(0, n);

    if (n <= 0) {
		return -1;
	} else {
	    return 0;
	}
}

bool Client::is_write_buffer_empty() const {
    return write_buffer.empty();
}

void Client::send_message(const std::string &sender, const std::string &message) {
    if (message.empty()) return;

    // std::cout << ":" << sender << " " << message << '\n';
    write_buffer += ":";
    write_buffer += sender;
    write_buffer += " ";
    write_buffer += message;
    write_buffer += "\r\n";
}

bool Client::is_connect_possible() const {
    return (!nickname.empty() && !username.empty() && !realname.empty() && !hostname.empty());
}
