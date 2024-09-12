#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#include "Server.hpp"

Server::Server(char* port, std::string password) : server_password(password) {
	if (std::strlen(port) > 6 || std::atoi(port) < 1024) {
		std::cout << "error\n";
		return;
	}
	server_port = std::atoi(port);

	create_kqueue();
	if (open_server()) {
		std::cout << "error\n";
		return;
	}
	add_event(server_socket_fd, EVFILT_READ);
}

int Server::run() {
	std::cout << "run\n";
	while (true) {
		int nev = kevent(kqueue_fd, changelist.data(), changelist.size(),
				eventlist, EVENTLIST_SIZE, NULL);
		changelist.clear();

		if (nev < 0) {
			perror("kqueue");
			exit(EXIT_FAILURE);
		}
		else {
			for (int i = 0; i < nev; i++) {
				struct kevent event = eventlist[i];

				if (event.flags & EV_ERROR) {
					perror("kqueue");
					exit(EXIT_FAILURE);
				} else if (event.ident == server_socket_fd) {
					const int client_socket_fd = accept(server_socket_fd, (struct sockaddr*)NULL, NULL);
					const Client client(client_socket_fd);

					clients_fd[client_socket_fd] = client;
					add_event(client_socket_fd, EVFILT_READ);
					// add_event(client_socket_fd, EVFILT_WRITE);
				} else if (event.filter == EVFILT_READ) {
					char buf[1024];
					int n = read(event.ident, buf, 1024);
					if (n <= 0) {
						close(event.ident);
						continue;
					}
					clients_fd[event.ident].read_handler(buf, n);
				}
				// TODO: EVFILT_WRITE
			}
		}
	}
}

void Server::add_event(uintptr_t ident, int16_t filter) {
	struct kevent event;

	EV_SET(&event, ident, filter, EV_ADD | EV_ENABLE, 0, 0, 0);
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

	bind(server_socket_fd, (struct sockaddr*)&addr, sizeof(addr));

	listen(server_socket_fd, 1);

	return 0;
}

void Server::command_nick(const int fd, const std::string &nickname) {
    Client *client = &clients_fd[fd];

    if (!client->get_nickname().empty()) {
        clients_nickname.erase(client->get_nickname());
    }
    clients_nickname[nickname] = client;

    client->set_nickname(nickname);
}

int Server::command_user(const int fd, const std::string &username, const std::string &realname) {
    Client *client = &clients_fd[fd];

    if (client->get_username().empty()) return -1;  // 이미 등록되어있으면
    // :molybdenum.libera.chat 462 younghoc :You are already connected and cannot handshake again

    client->set_nickname(username);
    client->set_realname(realname);

    return 0;
}
