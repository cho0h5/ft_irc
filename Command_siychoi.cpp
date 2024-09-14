#include "Server.hpp"
#include "Client.hpp"
#include <algorithm>

//1. mode인지 확인
//2. parameter 있는지 확인(461)
//3. 채널명확인 (403)
//	3-1. #붙었는지 확인
//	3-2. 채널명이 있는지 확인
//4. 명령어 확인 (472) - 명령어 없을 경우 채널 정보 출력
	//ex)
	//mode #42seoul
	//:lead.libera.chat 324 nickname #42seoul +Cnst <-- 기본옵션
	//:lead.libera.chat 329 nickname #42seoul 1726278610 <--- 이건뭐지 주소?

//	4-1. +-인지 확인
//	4-2. irklo 중에 하나 나와야 돼
//5. 권한 확인 (482)
//6. parameter가 없는지 확인(461)
//	6-1. o option에 nick이 존재하는지 확인(401)

void Server::command_mode(const int fd, std::vector<std::string> cmds) {

	std::string	channel_name, user_name;	//username은 어떻게 정의하지? fd로 하는 건가
	std::map<bool, std::string>	options;

	//1
    if (cmds[0] != "MODE") {
        return;
    }

	//2
	if (cmds.size() < 2) {
		//send_error(fd, 461);
		return ;
	}

	//3
	if (cmds[1].at(0) != '#') {
		//send_error(fd, 403)
		return ;
	}
	cmds[1].erase(0, 1);
	channel_name = cmds[1];	
	if (channels.find(channel_name) == channels.end()) {
		//send_error(fd, 403)
		return ;
	}
	
	if (cmds.size() == 2) {
		// 채널정보 출력
		return ;
	}
	
	std::vector<Client*>& clients = channels.find(channel_name)->second.get_clients();
	std::vector<Client*>& operators = channels.find(channel_name)->second.get_operators();

	//4
	//여기 너무 복잡한데;;

	//5
	if (find(operators.begin(), operators.end(), user_name) == operators.end()) {
		//send_error(fd, 482)
		return ;
	}
	
}
