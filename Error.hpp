#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

class Error {
    public:
    static std::string err_unknowncommand(const std::string &nickname, const std::string &command);    // 421

    static std::string err_nonicknamegiven();   // 431
    static std::string err_erroneusnickname();  // 432
    static std::string err_nicknameinuse();     // 433
    static std::string err_nickcollision();     // 436

    static std::string err_needmoreparams();    // 461
    static std::string err_alreadyregistered(); // 462

    private:
};

#endif
