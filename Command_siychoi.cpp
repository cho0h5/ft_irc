#include "Error.hpp"
#include "Server.hpp"
#include "Client.hpp"

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

int get_user_limit(const std::string &str) {
	int limit = 0;
	for (size_t i = 0; i < str.size(); i++) {
		if ('0' <= str[i] && str[i] <= '9') {
			limit = limit * 10 + (str[i] - '0');
			if (limit > 500)
				limit = 500;
		}
		else
			return -1;
	}
	return limit;
}

bool is_right_mode(const std::string &str) {
	if (str[0] != '+' && str[0] != '-')
		return false;
	for (size_t i = 1; i < str.size(); i++) {
		if (str[i] != 'i' && str[i] != 't' && str[i] != 'k' && str[i] != 'o' && str[i] != 'l') // itkol
			return false;
	}
	return true;
}

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
		return;
	}

	Channel& channel = iter->second;
	// show channel mode info
	if (cmds.size() == 2) {
		clients_fd[fd].send_message(get_servername(), "324 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " " + channel.get_channel_mode() + " " + channel.get_channel_params());
		clients_fd[fd].send_message(get_servername(), "329 " + clients_fd[fd].get_nickname() + " " + channel.get_name() + " " + channel.get_channel_generated_time());
		return ;
	}

	// 5. not channel operator : ERR_CHANOPRIVSNEEDED, 482
	if (iter->second.get_operator(&clients_fd[fd]) == NULL) {
        clients_fd[fd].send_message(get_servername(), Error::err_chanoprivsneeded(clients_fd[fd].get_nickname(), cmds[1]));
        return;
    }

	// 4. add mode, itkol
	unsigned int args_idx = 3, args_size = cmds.size();
	std::vector<std::string> success_cmds(1, "");

	// unknown mode char : ERR_UNKNOWNMODE, 472
	if (!is_right_mode(cmds[2])) {
		clients_fd[fd].send_message(get_servername(), Error::err_unknownmode(clients_fd[fd].get_nickname(), cmds[2]));
		return;
	}

	if (cmds[2][0] == '+') {
		success_cmds[0] = "+";
		for (size_t i = 1; i < cmds[2].size(); i++) {

			// add invite only
			if (cmds[2][i] == 'i') {
				if (channel.get_option_invite_only())
					continue;
				channel.add_channel_mode("i");
				channel.set_option_i();
				success_cmds[0] += "i";
			}
			// add topic restrict
			else if (cmds[2][i] == 't') {
				if (channel.get_option_topic_restrict())
					continue;
				channel.add_channel_mode("t");
				channel.set_option_t();
				success_cmds[0] += "t";
			}
			// add/change key
			else if (cmds[2][i] == 'k') {
				// not enough parameters : ERR_NEEDMOREPARAMS, 461
				if (args_idx >= args_size) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				channel.add_channel_mode("k");
				channel.set_channel_key(cmds[args_idx]);
				success_cmds[0] += "k";
				success_cmds.push_back(cmds[args_idx++]);
			}
			// add limit
			else if (cmds[2][i] == 'l') {
				// not enough parameters : ERR_NEEDMOREPARAMS, 461
				if (args_idx >= args_size) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				channel.add_channel_mode("l");
				// invalid limit : ERR_NEEDMOREPARAMS, 461
				int limit = get_user_limit(cmds[args_idx++]);
				if (limit == -1) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				channel.set_channel_users_limit(limit);
				success_cmds[0] += "l";
				success_cmds.push_back(std::to_string(limit));
			}

			// add operator, TODO : 여러 명 한 번에 지원할 수 있는 서버도 있다던데 우린 한 명씩만 하는 거로 할까..?
			else if (cmds[2][i] == 'o') {
				// not enough parameters : ERR_NEEDMOREPARAMS, 461
				if (args_idx >= args_size) {
					clients_fd[fd].send_message(get_servername(), Error::err_needmoreparams("MODE"));
					continue;
				}
				// no such nick : ERR_NOSUCHNICK, 401
				std::map<std::string, Client*>::const_iterator it = clients_nickname.find(cmds[args_idx]);
				if (it == clients_nickname.end()) {
					clients_fd[fd].send_message(get_servername(), Error::err_nosuchnick(clients_fd[fd].get_nickname(), cmds[args_idx++]));
					continue;
				}
        		// not in channel : ERR_USERNOTINCHANNEL, 441
				if (channel.get_client(it->second) == NULL) {
					clients_fd[fd].send_message(get_servername(), Error::err_usernotinchannel(clients_fd[fd].get_nickname(), cmds[args_idx++], channel.get_name()));
					continue;
				}
				// already operator
				if (channel.get_operator(it->second) != NULL) {
				    args_idx += 1;
					continue;
				}
				channel.add_operator(clients_nickname[cmds[args_idx++]]);
			}
		}
	}
	// 4. remove mode, itkol
	else if (cmds[2][0] == '-') {
		success_cmds[0] = "-";
		for (size_t i = 1; i < cmds[2].size(); i++) {
			// remove invite only
			if (cmds[2][i] == 'i') {
				if (!channel.get_option_invite_only())
					continue;
				channel.unset_option_i();
				channel.remove_channel_mode("i");
				success_cmds[0] += "i";
			}
			// remove topic restrict
			else if (cmds[2][i] == 't') {
				if (!channel.get_option_topic_restrict())
					continue;
				channel.unset_option_t();
				channel.remove_channel_mode("t");
				success_cmds[0] += "t";
			}
			// remove key
			else if (cmds[2][i] == 'k') {
				channel.remove_channel_mode("k");
				channel.set_channel_key("");
				success_cmds[0] += "k";
			}
			// remove limit
			else if (cmds[2][i] == 'l') {
				channel.remove_channel_mode("l");
				channel.set_channel_users_limit(500);
				success_cmds[0] += "l";
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
                std::map<std::string, Client*>::const_iterator it = clients_nickname.find(cmds[args_idx]);
				if (channel.get_client(it->second) == NULL) {
					clients_fd[fd].send_message(get_servername(), Error::err_usernotinchannel(clients_fd[fd].get_nickname(), cmds[args_idx++], channel.get_name()));
					continue;
				}
				// not channel operator : ERR_CHANOPRIVSNEEDED, 482
				if (channel.get_operator(it->second) == NULL) {
					// clients_fd[fd].send_message(get_servername(), Error::err_chanoprivsneeded(clients_fd[fd].get_nickname(), cmds[args_idx++]));
					continue;
				}
				channel.remove_operator(clients_nickname[cmds[args_idx++]]);
			}
		}
	}

	// make message with success commands

	// no mode changed
	if (success_cmds[0] == "+" || success_cmds[0] == "-")
		return;

	std::string successed_mode_cmds_message = "";
	for (size_t i = 0; i < success_cmds.size(); i++)
	 	successed_mode_cmds_message += (success_cmds[i] + " ");

	// send changed mode to all clients in the channel
    std::set<Client*> joined_users = channel.get_clients();
    for (std::set<Client*>::iterator iter = joined_users.begin(); iter != joined_users.end(); iter++)
        (*iter)->send_message(clients_fd[fd].get_identifier(), "MODE " + channel.get_name() + " " + successed_mode_cmds_message);
}
