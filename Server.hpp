#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <sys/event.h>

#include "Channel.hpp"
#include "Client.hpp"

#define EVENTLIST_SIZE 8

class Server {
public:
	Server(char* port, std::string password);

	int run();	// return value: error code

private:
	//
	std::map<std::string, Channel> channels;
	std::map<std::string, Client> clients_nickname; // nickname -> client
	std::map<int, Client*> clients_fd; // fd -> client

	std::string server_password;
	int server_port;
	//

	std::vector<struct kevent> changelist;
	struct kevent eventlist[EVENTLIST_SIZE];

	int kqueue_fd;
	unsigned int server_socket_fd;

	int create_kqueue();
	int open_server();
	void add_event(uintptr_t ident, int16_t filter);
};

#endif
