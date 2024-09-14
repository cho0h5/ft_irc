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
	std::map<std::string, Client*> clients_nickname; // nickname -> client
	std::map<int, Client> clients_fd; // fd -> client

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

	// hyowchoi
	void command_parsing(const int fd, std::string command);
	void command_nick(const int fd, std::vector<std::string> cmds);
	void command_user(const int fd, std::vector<std::string> cmds);
	void command_privmsg_user(const int fd, std::vector<std::string> cmds);
	void command_privmsg_channel(const int fd, std::vector<std::string> cmds);

	// younghoc
	void command_join(const int fd, std::vector<std::string> cmds);
	void command_topic(const int fd, std::vector<std::string> cmds);
	void command_invite(const int fd, std::vector<std::string> cmds);
	void command_kick(const int fd, std::vector<std::string> cmds);

	// siychoi
	void command_mode(const int fd, std::vector<std::string> cmds);
};

#endif
