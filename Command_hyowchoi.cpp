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
void Server::command_nick(const int fd, std::vector<std::string> cmds) {
    
    // not NICK command
    if (cmds[0] != "NICK") {
        return;
    }
    
    // no new_nickname : ERR_NONICKNAMEGIVEN, 431
    if (cmds.size() != 2) {
        // send_error(fd, 431);
        return;
    }

    // already registered nickname : ERR_NICKNAMEINUSE, 433
    if (clients_nickname.find(cmds[1]) != clients_nickname.end()) {
        // send_error(fd, 433);
        return;
    }
    
    // check valid nickname
    for (char c : cmds[1]) {
        if (!(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || ('0' <= c && c <= '9'))) {
            // send_error(fd, 432);
            return;
        }
    }
    if (('0' <= cmds[1][0] && cmds[1][0] <= '9')) {
        // send_error(fd, 432);
        return;
    }
    // change nickname
    std::map<int, Client*>::iterator it = clients_fd.find(fd);
    it->second->set_nickname(cmds[1]);
}

int Server::command_user(const int fd, std::vector<std::string> cmds) {

}

void Server::command_privmsg_user(const int fd, std::vector<std::string> cmds) {

}

void Server::command_privmsg_channel(const int fd, std::vector<std::string> cmds) {

}

