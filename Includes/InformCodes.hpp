#pragma once

#define MSJ_PARAM "Not enough parameters!\n"
#define MSJ_DIGITATSTART "First character is a digit!\n"
#define MSJ_TOOLONG "Too long nickname!\n"
#define MSJ_TAKENNICK "Nick name is already taken!\n"
#define MSJ_INVALIDCHAR "Used invalid charachters!\n"

enum ErrorCode {
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NEEDMOREPARAMS = 461,
    ERR_NICKNAMEINUSE = 433
};

