#pragma once

#define PARAMERROR "Not enough parameters!\n"
#define DIGITATSTART "First character is a digit!\n"
#define TOOLONG "Too long nickname!\n"

enum ErrorCode {
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NEEDMOREPARAMS = 461,
    ERR_NICKNAMEINUSE = 433
};

