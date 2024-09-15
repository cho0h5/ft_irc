#include "Error.hpp"

// <client> <command> :Unknown command
// :lead.libera.chat 421 younghoc ASDF :Unknown command
std::string Error::err_unknowncommand(const std::string &nickname, const std::string &command) {
    return "421 " + nickname + " " + command + " :Unknown command";
}
