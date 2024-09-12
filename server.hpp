#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <sys/event.h>

#include "Channel.hpp"
#include "Client.hpp"

#define EVENTLIST_SIZE 8
#define PORT 8080

class Server {
public:
	Server(char* port, std::string password);

	int run();	// return value: error code
	void add_event(uintptr_t ident, int16_t filter);

private:
	//
	std::vector<Channel*> channels;
	std::vector<Client*> clients;
	
	std::string server_password;
	int server_port;
	//

	std::vector<struct kevent> changelist;
	struct kevent eventlist[EVENTLIST_SIZE];

	int kqueue_fd;
	unsigned int server_socket_fd;

	int create_kqueue();
	int open_server();
};

#endif
