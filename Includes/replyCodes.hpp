#pragma once

enum ErrorCode { 
        RPL_WELCOME = 1, 
        RPL_YOURHOST = 2,
        RPL_CREATED = 3,
        ERR_NOSUCHNICK = 401,
        ERR_NOSUCHCHANNEL = 403,
        ERR_ERRONEUSNICKNAME = 432, 
        ERR_ALREADYREGISTRED = 433, 
        ERR_NEEDMOREPARAMS = 461,
        ERR_USERONCHANNEL = 443,
        ERR_NORECIPIENT = 411,
        ERR_NOTEXTTOSEND = 412,
        ERR_CHANNELISFULL = 471,
        ERR_INVITEONLYCHAN = 473,
        ERR_BANNEDFROMCHAN = 474,
        ERR_UNKNOWNCOMMAND = 421,
        ERR_NOTONCHANNEL = 442,
        ERR_BADCHANNELKEY = 475
};

#define msg_param "Not enough parameters!"
#define msg_digit_start "First char cannot be a digit!"
#define msg_too_long "Nickname is too long!"
#define msg_taken "Nickname is already taken!"
#define msg_inva_char "Invalid characters used!"
#define msg_cmd_unknown "Unknown command!"
#define msg_cmd_too_long "Command is too long!"
#define msg_not_authorized "You are not authorized!"
#define msg_already_reg "You are already registered!"

#define msg_invalid_nick "No such nick name exists!"
#define msg_no_recipirnt "No recipient given!"
#define msg_notexttosend "No text to send!"

//channels
#define msg_no_such_channel "No such channel exists!"
#define msg_banned "You are banned from this channel!"
#define msg_user_limit "Channel user limit reached!"
#define msg_no_privileges "You do not have the necessary privileges!"
#define msg_useronchannel "You are already on that channel!"
#define msg_invite_only "Cannot join channel invite only!"
#define msg_not_member "You are not channel member!"
#define msg_wrong_key "Cannot join channel (+k) - incorrect key!"

#define msg_server_full "Server is full!"
#define msg_ping_timeout "Ping timeout!"
#define msg_invalid_mode "Invalid mode!"
#define msg_bad_password "Incorrect password!"
#define msg_nick_change_denied "Nickname change denied!"
#define msg_invalid_topic "Invalid topic format!"
#define msg_channel_locked "Channel is locked!"
#define msg_no_join_permission "You don't have permission to join this channel!"
