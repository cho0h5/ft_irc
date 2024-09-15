#include "Error.hpp"

std::string Error::err_nosuchnick(const std::string &nickname, const std::string &wrong_recipient) {
    return "401 " + nickname + " " + wrong_recipient + " :No such nick/channel";
}

std::string Error::err_nosuchchannel(const std::string &nickname, const std::string &channelname) {
    return "403 " + nickname + " " + channelname + " :No such channel";
}

std::string Error::err_cannotsendtochan(const std::string &nickname, const std::string &channelname) {
    return "404 " + nickname + " " + channelname + " :Cannot send to nick/channel";
}

std::string Error::err_norecipient(const std::string &nickname, const std::string &command) {
    return "411 " + nickname + " :No recipient given (" + command + ")";
}

std::string Error::err_notexttosend(const std::string &nickname) {
    return "412 " + nickname + " :No text to send";
}

std::string Error::err_unknowncommand(const std::string &nickname, const std::string &wrong_command) {
    return "421 " + nickname + " " + wrong_command + " :Unknown command";
}

std::string Error::err_nonicknamegiven() {
    return "431 * :No nickname given";
}

std::string Error::err_erroneusnickname(const std::string &wrong_nickname) {
    return "432 * " + wrong_nickname + " :Erroneus nickname";
}

std::string Error::err_nicknameinuse(const std::string &wrong_nickname) {
    return "433 * " + wrong_nickname + " :";
}

std::string Error::err_usernotinchannel(const std::string &nickname, const std::string &user, const std::string &channel) {
    return "441 " + nickname + " " + user + " " + channel + " :They aren't on that channel";
}

std::string Error::err_notonchannel(const std::string &nickname, const std::string &channel_name) {
    return "442 " + nickname + " " + channel_name + " :You're not on that channel";
}

std::string Error::err_useronchannel(const std::string &inviting_user, const std::string &invited_user, const std::string &channel_name) {
    return "443 " + inviting_user + " " + invited_user + " " + channel_name + " :is already on channel";
}


std::string Error::err_notregistered() {
    return "451 * :You have not registered";
}

std::string Error::err_needmoreparams(const std::string &command) {
    return "461 * " + command + " :Not enough parameters";
}

std::string Error::err_alreadyregistered(const std::string &nickname) {
    return "462 " + nickname + " :You may not reregister";
}

std::string Error::err_channelisfull(const std::string &nickname, const std::string &channel_name) {
    return "471 " + nickname + " " + channel_name + " :Cannot join channel (+l) - channel is full";
}

std::string Error::err_inviteonlychan(const std::string &nickname, const std::string &channel_name) {
    return "473 " + nickname + " " + channel_name + " :Cannot join channel (+i) - invite only";
}

std::string Error::err_badchannelkey(const std::string &nickname, const std::string &channel_name) {
    return "475 " + nickname + " " + channel_name + " :Cannot join channel (+k) - bad key";
}

std::string Error::err_badchanmask(const std::string &nickname, const std::string &channel_name) {
    return "476 " + nickname + " " + channel_name + " :Bad Channel Mask";
}

std::string Error::err_chanoprivsneeded(const std::string &nickname, const std::string &channel_name) {
    return "482 " + nickname + " " + channel_name + " :You're not a channel operator";
}