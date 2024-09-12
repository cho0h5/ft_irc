#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <sys/event.h>

#define EVENTLIST_SIZE 8
#define PORT 8080

class Server {
public:
	Server();

	int run();	// return value: error code
	void add_event(uintptr_t ident, int16_t filter);

private:
	std::vector<struct kevent> changelist;
	struct kevent eventlist[EVENTLIST_SIZE];

	int kqueue_fd;
	int server_socket_fd;

	int create_kqueue();
	int open_server();
};

#endif
