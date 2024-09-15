#include "Error.hpp"

std::string Error::err_nosuchnick(const std::string &nickname, const std::string &wrong_recipient) {
    return "401 " + nickname + " " + wrong_recipient + " :No such nick/channel";
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

std::string Error::err_needmoreparams(const std::string &command) {
    return "461 * " + command + " :Not enough parameters";
}

std::string Error::err_alreadyregistered(const std::string &nickname) {
    return "462 " + nickname + " :You may not reregister";
}
