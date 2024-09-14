#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include "Server.hpp"
#include "Client.hpp"

Server::Server(char* port, std::string password) : server_password(password) {
	if (std::strlen(port) > 6 || std::atoi(port) < 1024) {
		std::cout << "error\n";
		exit(EXIT_FAILURE);
	}
	server_port = std::atoi(port);

	create_kqueue();
	if (open_server()) {
		std::cout << "error\n";
		exit(EXIT_FAILURE);
	}
	add_event(server_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE);
}

int Server::run() {
	std::cout << "run\n";
	while (true) {
	    set_kqueue_write_event();
		int nev = kevent(kqueue_fd, changelist.data(), changelist.size(),
				eventlist, EVENTLIST_SIZE, NULL);
		changelist.clear();

		if (nev < 0) {
			perror("kqueue");
			exit(EXIT_FAILURE);
		}
		else {
			for (int i = 0; i < nev; i++) {
				const struct kevent event = eventlist[i];

				if (event.flags & EV_ERROR) {
					perror("kqueue");
					exit(EXIT_FAILURE);
				} else if (event.ident == server_socket_fd) {
    				struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
					const int client_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
					char client_ip[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
					std::string client_ip_str(client_ip);
					const Client client(client_fd, client_ip_str);
					clients_fd[client_fd] = client;
					add_event(client_fd, EVFILT_READ, EV_ADD | EV_ENABLE);
					add_event(client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE);
				} else if (event.filter == EVFILT_READ) {
					const int ret = clients_fd[event.ident].read_handler(this);

					if (ret) {
					    // TODO: remove this client
					}
				} else if (event.filter == EVFILT_WRITE) {
				    clients_fd[event.ident].write_handler();
				}
			}
		}
	}
}

void Server::add_event(uintptr_t ident, int16_t filter, uint16_t flags) {
	struct kevent event;

	EV_SET(&event, ident, filter, flags, 0, 0, 0);
	changelist.push_back(event);
}

int Server::create_kqueue() {
	kqueue_fd = kqueue();
	if (kqueue_fd == -1) {
		return -1;
	}

	return 0;
}

int Server::open_server() {
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(server_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	   return -1;
	}

	listen(server_socket_fd, 1);

	return 0;
}

void Server::set_kqueue_write_event() {
    for (std::map<int, Client>::const_iterator it = clients_fd.begin(); it != clients_fd.end(); it++) {
        const Client client = it->second;

        if (client.is_write_buffer_empty()) {
            add_event(client.get_fd(), EVFILT_WRITE, EV_ADD | EV_DISABLE);
        } else {
            add_event(client.get_fd(), EVFILT_WRITE, EV_ADD | EV_ENABLE);
        }
    }
}

void Server::command_parsing(const int fd, const std::string &command) {
    const std::string Commands[] = {"NICK", "USER", "PRIVMSG", "JOIN", "MODE", "TOPIC", "KICK", "INVITE" };
    std::stringstream ss(command);
    std::string exec_cmd, token;
    std::vector<std::string> tokens;

    // find execute commands
    const int size = sizeof(Commands) / sizeof(Commands[0]);
    for (int i = 0; i < size; i++) {
        const std::string &cmd = Commands[i];
        if (command.find(cmd) == 0) {
            exec_cmd = cmd;
            break;
        }
    }

    // tokenize
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (exec_cmd == "NICK") {
        command_nick(fd, tokens);
    } else if (exec_cmd == "USER") {
        // command_user(fd, tokens[1], tokens[4]);
    }
	// else if (exec_cmd == "PRIVMSG") {
    //     if (tokens[1][0] == '#') {
    //         command_privmsg_channel(tokens[1], tokens[2]);
    //     } else {
    //         command_privmsg_user(tokens[1], tokens[2]);
    //     }
    // }


}

void Server::send_error(const int fd, const int error_code) {
    // TODO: 431, 432, 433, 461, 462
    (void)fd;
    (void)error_code;
}
