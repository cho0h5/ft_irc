#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

class Error {
    public:
    // <client> <nickname> :No such nick/channel
    // :silver.libera.chat 401 younghoc asdfawefawe :No such nick/channel
    static std::string err_nosuchnick(const std::string &nickname, const std::string &wrong_recipient);    // 401

    // <client> <channel> :No such channel
    // :server_name 403 <nickname> <channel_name> :No such channel
    static std::string err_nosuchchannel(const std::string &nickname, const std::string &channelname);    // 403

    // <client> <channel> :Cannot send to channel
    // :tantalum.libera.chat 404 younghoc #42 :Cannot send to nick/channel
    static std::string err_cannotsendtochan(const std::string &nickname, const std::string &channelname);    // 404



    // <client> :No recipient given (<command>)
    // :silver.libera.chat 411 younghoc :No recipient given (PRIVMSG)
    static std::string err_norecipient(const std::string &nickname, const std::string &command);   // 411
    // <client> :No text to send
    // :silver.libera.chat 412 younghoc :No text to send
    static std::string err_notexttosend(const std::string &nickname);  // 412

    // <client> <command> :Unknown command
    // :lead.libera.chat 421 younghoc ASDF :Unknown command
    static std::string err_unknowncommand(const std::string &nickname, const std::string &wrong_command);    // 421

    // <client> :No nickname given
    // :tantalum.libera.chat 431 * :No nickname given
    static std::string err_nonicknamegiven();   // 431
    // <client> <nick> :Erroneus nickname
    // :tantalum.libera.chat 432 * awef@#$ :Erroneous Nickname
    static std::string err_erroneusnickname(const std::string &wrong_nickname);  // 432
    // <client> <nick> :Nickname is already in use
    // :tantalum.libera.chat 433 * asdf :Nickname is already in use.
    static std::string err_nicknameinuse(const std::string &wrong_nickname);     // 433

    // <client> <nick> <channel> :They aren't on that channel
    // :server 441 <nick> <user> <channel> :They aren't on that channel
    static std::string err_usernotinchannel(const std::string &nickname, const std::string &user, const std::string &channel); // 441

    // <client> <channel> :You're not on that channel
    // :server_name 442 <nickname> <channel_name> :You're not on that channel
    static std::string err_notonchannel(const std::string &nickname, const std::string &channel_name); // 442

    // <client> <nick> <channel> :is already on channel
    // :server_name 443 <inviting_user> <invited_user> <channel_name> :is already on channel
    static std::string err_useronchannel(const std::string &inviting_user, const std::string &invited_user, const std::string &channel_name); // 443

    // <client> :You have not registered
    // :tantalum.libera.chat 451 * :You have not registered
    static std::string err_notregistered(); // 451

    // <client> <command> :Not enough parameters
    // :iridium.libera.chat 461 * USER :Not enough parameters
    static std::string err_needmoreparams(const std::string &command);    // 461
    // <client> :You may not reregister
    // :calcium.libera.chat 462 younghoc :You are already connected and cannot handshake again
    static std::string err_alreadyregistered(const std::string &nickname); // 462

    // <client> <channel> :Cannot join channel (+l)
    // :server_name 471 <nickname> <channel_name> :Cannot join channel (+l) - channel is full
    static std::string err_channelisfull(const std::string &nickname, const std::string &channel_name); // 471

    // <client> <modechar> :is unknown mode char to me
    // :server 472 <user> l :is unknown mode char to me
    static std::string err_unknownmode(const std::string &nickname, const std::string &modechar); // 472


    // <client> <channel> :Cannot join channel (+i)
    // :server_name 473 <nickname> <channel_name> :Cannot join channel (+i) - invite only
    static std::string err_inviteonlychan(const std::string &nickname, const std::string &channel_name); // 473

    // <client> <channel> :Cannot join channel (+k)
    // :server_name 475 <nickname> <channel_name> :Cannot join channel (+k) - bad key
    static std::string err_badchannelkey(const std::string &nickname, const std::string &channel_name); // 475

    // <channel> :Bad Channel Mask
    // :server_name 476 <nickname> <channel_name> :Bad Channel Mask
    static std::string err_badchanmask(const std::string &nickname, const std::string &channel_name); // 476

    // <client> <channel> :You're not channel operator
    // :server_name 482 <nickname> <channel_name> :You're not a channel operator
    static std::string err_chanoprivsneeded(const std::string &nickname, const std::string &channel_name); // 482

    private:
};

#endif
