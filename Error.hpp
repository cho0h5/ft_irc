#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

class Error {
    public:
    // <client> <command> :Unknown command
    // :lead.libera.chat 421 younghoc ASDF :Unknown command
    static std::string err_unknowncommand(const std::string &nickname, const std::string &command);    // 421

    // <client> <nickname> :No such nick/channel
    // :silver.libera.chat 401 younghoc asdfawefawe :No such nick/channel
    static std::string err_nosuchnick();    // 401
    // <client> :No recipient given (<command>)
    // :silver.libera.chat 411 younghoc :No recipient given (PRIVMSG)
    static std::string err_norecipient();   // 411
    // <client> :No text to send
    // :silver.libera.chat 412 younghoc :No text to send
    static std::string err_notexttosend();  // 412

    // <client> :No nickname given
    // :tantalum.libera.chat 431 * :No nickname given
    static std::string err_nonicknamegiven();   // 431
    // <client> <nick> :Erroneus nickname
    // :tantalum.libera.chat 432 * awef@#$ :Erroneous Nickname
    static std::string err_erroneusnickname();  // 432
    // <client> <nick> :Nickname is already in use
    // :tantalum.libera.chat 433 * asdf :Nickname is already in use.
    static std::string err_nicknameinuse();     // 433

    // <client> <command> :Not enough parameters
    // :iridium.libera.chat 461 * USER :Not enough parameters
    static std::string err_needmoreparams();    // 461
    // <client> :You may not reregister
    // :calcium.libera.chat 462 younghoc :You are already connected and cannot handshake again
    static std::string err_alreadyregistered(); // 462

    private:
};

#endif
