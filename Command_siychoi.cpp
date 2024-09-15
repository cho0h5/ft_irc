#include "Error.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <set>

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

void Server::command_mode(const int fd, const std::vector<std::string> &cmds) {
    if (!clients_fd[fd].get_is_registered()) {
        clients_fd[fd].send_message(get_servername(), Error::err_notregistered());
        return;
    }

	// 1. not MODE command
    if (cmds[0] != "MODE") {
        return;
    }

	// 2. not enough parameters : ERR_NEEDMOREPARAMS, 461
	if (cmds.size() < 2) {
		clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
		return;
	}

	// 3. find channel names in server
	std::map<std::string, Channel>::iterator iter = channels.find(cmds[1]);

	// no such channel : ERR_NOSUCHCHANNEL, 403
	if (iter == channels.end()) {
		clients_fd[fd].send_message(get_servername(), Error::err_nosuchchannel(clients_fd[fd].get_nickname(), cmds[1]));
		return ;
	}

	// 5. not channel operator : ERR_CHANOPRIVSNEEDED, 482
	if (iter->second.get_operator(clients_fd[fd].get_nickname()) == NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_chanoprivsneeded(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

	Channel& channel = iter->second;

	// TODO: add channel params
	if (cmds.size() == 2) {
		clients_fd[fd].send_message(get_servername(), "324 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " " + channel.get_channel_mode() + " " + channel.get_channel_params());
		return ;
	}

	unsigned int args_idx = 3, args_size = cmds.size();
	// 4. add mode , itkol
	if (cmds[2][0] == '+') {
		for (size_t i = 1; i < cmds[2].size(); i++) {

			// add invite only
			if (cmds[2][i] == 'i') {
				if (channel.get_option_invite_only())
					continue;
				channel.add_channel_mode("i");
				channel.set_option_i();
			}
			// add topic restrict
			else if (cmds[2][i] == 't') {
				if (channel.get_option_topic_restrict())
					continue;
				channel.add_channel_mode("t");
				channel.set_option_t();
			}
			// add/change key
			else if (cmds[2][i] == 'k') {
				// not enough parameters : ERR_NEEDMOREPARAMS, 461
				if (args_idx >= args_size) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				channel.add_channel_mode("k");
				channel.set_channel_key(cmds[args_idx++]);
			}
			// add limit
			else if (cmds[2][i] == 'l') {
				// not enough parameters : ERR_NEEDMOREPARAMS, 461
				if (args_idx >= args_size) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				channel.add_channel_mode("l");
				channel.set_channel_users_limit(std::stoi(cmds[args_idx++]));
			}
			
			// add operator, TODO : 여러 명 한 번에 지원할 수 있는 서버도 있다던데 우린 한 명씩만 하는 거로 할까..?
			else if (cmds[2][i] == 'o') {
				// not enough parameters : ERR_NEEDMOREPARAMS, 461
				if (args_idx >= args_size) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				// no such nick : ERR_NOSUCHNICK, 401
				if (clients_nickname.find(cmds[args_idx]) == clients_nickname.end()) {
					clients_fd[fd].send_message(get_servername(), Error::err_nosuchnick(clients_fd[fd].get_nickname(), cmds[args_idx++]));
					continue;
				}
        		// not in channel : ERR_USERNOTINCHANNEL, 441
				if (channel.get_client(cmds[args_idx]) == NULL) {
					clients_fd[fd].send_message(get_servername(), Error::err_usernotinchannel(clients_fd[fd].get_nickname(), cmds[args_idx++], channel.get_name()));
					continue;
				}
				// already operator
				if (channel.get_operator(cmds[args_idx++]) != NULL) {
					continue;
				}
				// channel.add_channel_mode("o");
				channel.add_operator(clients_nickname[cmds[args_idx++]]);
			}
			else {
				// invalid mode or not alphebet
				// TODO : 뭔 에러?
				return ;
			}
		}
	}
	// 4. remove mode, itkol
	else if (cmds[2][0] == '-') {
		for (size_t i = 1; i < cmds[2].size(); i++) {
			// remove invite only
			if (cmds[2][i] == 'i') {
				if (!channel.get_option_invite_only())
					continue;
				channel.unset_option_i();
				channel.remove_channel_mode("i");
			}
			// remove topic restrict
			else if (cmds[2][i] == 't') {
				if (!channel.get_option_topic_restrict())
					continue;
				channel.unset_option_t();
				channel.remove_channel_mode("t");
			}
			// remove key
			else if (cmds[2][i] == 'k') {
				channel.remove_channel_mode("k");
				channel.set_channel_key("");
			}
			// remove limit
			else if (cmds[2][i] == 'l') {
				channel.remove_channel_mode("l");
				channel.set_channel_users_limit(0);
			}
			// remove operator -> 서버에 따라 operator 옵션을 안 보여줘도 됨, 우린 이렇게 간다
			else if (cmds[2][i] == 'o') {
				// not enough parameters : ERR_NEEDMOREPARAMS, 461
				if (args_idx >= args_size) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				// no such nick : ERR_NOSUCHNICK, 401
				if (clients_nickname.find(cmds[args_idx]) == clients_nickname.end()) {
					clients_fd[fd].send_message(get_servername(), Error::err_nosuchnick(clients_fd[fd].get_nickname(), cmds[args_idx++]));
					continue;
				}
        		// not in channel : ERR_USERNOTINCHANNEL, 441
				if (channel.get_client(cmds[args_idx]) == NULL) {
					clients_fd[fd].send_message(get_servername(), Error::err_usernotinchannel(clients_fd[fd].get_nickname(), cmds[args_idx++], channel.get_name()));
					continue;
				}
				// not channel operator : ERR_CHANOPRIVSNEEDED, 482
				if (channel.get_operator(cmds[args_idx]) == NULL) {
					clients_fd[fd].send_message(get_servername(), Error::err_chanoprivsneeded(clients_fd[fd].get_nickname(), cmds[args_idx++]));
					continue;
				}
			}
			else {
				// invalid mode or not alphebet
				// TODO : 뭔 에러?
				return ;
			}
		}
	}
	// +, -가 아닌 다른 문자가 올 경우, 어케 처리하지
	else {
		// invalid mode
		return ;
	}
}
