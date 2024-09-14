#include "Server.hpp"
#include "Client.hpp"
// #include <sstream>

// void Server::command_parsing(const int fd, std::string command) {
//     const std::string Commands[] = {
//         "NICK", "USER", "PRIVMSG", "JOIN", "MODE", "TOPIC", "KICK", "INVITE" };
//     std::stringstream ss(command);
//     std::string exec_cmd, token;
//     std::vector<std::string> tokens;

//     // find execute commands
//     for (const std::string &cmd : Commands) {
//         if (command.find(cmd) == 0) {
//             exec_cmd = cmd;
//             break;
//         }
//     }

//     // tokenize
//     while (ss >> token) {
//         tokens.push_back(token);
//     }

//     if (exec_cmd == "NICK") {
//         command_nick(fd, tokens[1]);
//     } else if (exec_cmd == "USER") {
//         command_user(fd, tokens[1], tokens[4]);
//     // } else if (exec_cmd == "PRIVMSG") {
//     //     if (tokens[1][0] == '#') {
//     //         command_privmsg_channel(tokens[1], tokens[2]);
//     //     } else {
//     //         command_privmsg_user(tokens[1], tokens[2]);
//     //     }
//     }


// }

// NICK <new_nickname>
// TODO : client 객체 생성 시 nickname 임의로 지정
void Server::command_nick(const int fd, const std::vector<std::string> &cmds) {

    // not NICK command
    if (cmds[0] != "NICK") {
        return;
    }

    // no new_nickname : ERR_NONICKNAMEGIVEN, 431
    if (cmds.size() != 2) {
        send_error(fd, 431);
        return;
    }

    // already registered nickname : ERR_NICKNAMEINUSE, 433
    if (clients_nickname.find(cmds[1]) != clients_nickname.end()) {
        send_error(fd, 433);
        return;
    }

    // check valid nickname
    for (unsigned long i = 0; i < cmds[1].size(); i++) {
        const char c = cmds[1][i];
        if (!(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || ('0' <= c && c <= '9'))) {
            send_error(fd, 432);
            return;
        }
    }
    if (('0' <= cmds[1][0] && cmds[1][0] <= '9')) {
        send_error(fd, 432);
        return;
    }
    // change nickname
    std::map<int, Client>::iterator it = clients_fd.find(fd);
    it->second.set_nickname(cmds[1]);

    if (it->second.is_connect_possible()) {
        welcome(fd);
    }
}

void Server::command_user(const int fd, const std::vector<std::string> &cmds) {
    // not USER command
    if (cmds[0] != "USER") {
        return;
    }

    // no username : ERR_NEEDMOREPARAMS, 461
    if (cmds.size() != 5 || cmds[1].empty() || cmds[4].empty()) {
        send_error(fd, 461);
        return;
    }

    // already registered username : ERR_ALREADYREGISTRED, 462
    std::map<int, Client>::iterator it = clients_fd.find(fd);
    if (it->second.get_username() != "") {
        send_error(fd, 462);
        return;
    }

    // username contains space, but not prefixed with colon : 무슨 에러징...
    if (cmds[5].find(" ") != std::string::npos && cmds[5][0] != ':') {
        send_error(fd, 461);
        return;
    }
    // change username
    it->second.set_username(cmds[1]);
    it->second.set_realname(cmds[4]);   // TODO: 이름 유효성 검사 해야함

    if (it->second.is_connect_possible()) {
        welcome(fd);
    }
}

void Server::command_privmsg_user(const int fd, const std::vector<std::string> &cmds) {
    if (cmds.size() == 1) {
        send_error(fd, 411);
        return;
    }

    if (cmds.size() == 2) {
        send_error(fd, 412);
        return;
    }

    const std::string recipient = cmds[1];
    const std::string message = cmds[2];

    std::map<std::string, Client*>::const_iterator it = clients_nickname.find(recipient);
    if (it == clients_nickname.end()) {
        send_error(fd, 401);
        return;
    }
}

void Server::command_privmsg_channel(const int fd, const std::vector<std::string> &cmds) {
    (void)fd;
    (void)cmds;
}
