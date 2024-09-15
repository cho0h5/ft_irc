#include "Error.hpp"
#include "Server.hpp"
#include "Client.hpp"

// NICK <new_nickname>
// TODO : client 객체 생성 시 nickname 임의로 지정
void Server::command_nick(const int fd, const std::vector<std::string> &cmds) {
    // not NICK command
    if (cmds[0] != "NICK") {
        return;
    }

    // no new_nickname : ERR_NONICKNAMEGIVEN, 431
    if (cmds.size() != 2) {
        clients_fd[fd].send_message(get_servername(), Error::err_nonicknamegiven());
        return;
    }

    // already registered nickname : ERR_NICKNAMEINUSE, 433
    if (clients_nickname.find(cmds[1]) != clients_nickname.end()) {
        clients_fd[fd].send_message(get_servername(), Error::err_nicknameinuse(cmds[1]));
        return;
    }

    // check valid nickname
    for (unsigned long i = 0; i < cmds[1].size(); i++) {
        const char c = cmds[1][i];
        if (!(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || ('0' <= c && c <= '9') || c == '_')) {
            clients_fd[fd].send_message(get_servername(), Error::err_erroneusnickname(cmds[1]));
            return;
        }
    }
    if (('0' <= cmds[1][0] && cmds[1][0] <= '9')) {
        clients_fd[fd].send_message(get_servername(), Error::err_erroneusnickname(cmds[1]));
        return;
    }
    // change nickname
    std::map<int, Client>::iterator it = clients_fd.find(fd);
    Client &client = it->second;
    if (client.get_nickname().empty())
        clients_nickname.erase(client.get_nickname());
    client.set_nickname(cmds[1]);
    clients_nickname[cmds[1]] = &clients_fd[fd];

    if (it->second.is_connect_possible()) {
        register_client(fd);
    }
}

void Server::command_user(const int fd, const std::vector<std::string> &cmds) {
    // not USER command
    if (cmds[0] != "USER") {
        return;
    }

    // no username : ERR_NEEDMOREPARAMS, 461
    if (cmds.size() != 5 || cmds[1].empty() || cmds[4].empty()) {
        clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams(cmds[0]));
        return;
    }

    // already registered username : ERR_ALREADYREGISTRED, 462
    std::map<int, Client>::iterator it = clients_fd.find(fd);
    if (it->second.get_username() != "") {
        const std::string &nickname = clients_fd[fd].get_nickname();
        clients_fd[fd].send_message(get_servername(), Error::err_alreadyregistered(nickname));
        return;
    }

    // username contains space, but not prefixed with colon : 무슨 에러징...
    if (cmds[5].find(" ") != std::string::npos && cmds[5][0] != ':') {
        clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams(cmds[0]));
        return;
    }
    // change username
    it->second.set_username(cmds[1]);
    it->second.set_realname(cmds[4]);   // TODO: 이름 유효성 검사 해야함

    if (it->second.is_connect_possible()) {
        register_client(fd);
    }
}

void Server::command_privmsg_user(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

    const std::string command = cmds[0];
    const std::string &nickname = clients_fd[fd].get_nickname();

    if (cmds.size() == 1) {
        clients_fd[fd].send_message(get_servername(), Error::err_norecipient(nickname, command));
        return;
    }

    if (cmds.size() == 2) {
        clients_fd[fd].send_message(get_servername(), Error::err_notexttosend(nickname));
        return;
    }

    const std::string recipient = cmds[1];
    const std::string message = cmds[2];

    std::map<std::string, Client*>::iterator it = clients_nickname.find(recipient);
    if (it == clients_nickname.end()) {
        clients_fd[fd].send_message(get_servername(), Error::err_nosuchnick(nickname, recipient));
        return;
    }

    Client &client = clients_fd[fd];
    it->second->send_message(client.get_identifier(), "PRIVMSG " + nickname + " :" + message);
}

void Server::command_privmsg_channel(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

    (void)fd;
    (void)cmds;
}
