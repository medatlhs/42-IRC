#pragma once

enum ErrorCode { 
        ERR_ERRONEUSNICKNAME = 432, ERR_ALREADYREGISTRED = 433, 
        ERR_NEEDMOREPARAMS = 461, 
};

#define msg_param "Not enough parameters!\n"
#define msg_digit_start "First char cannot be a digit!\n"
#define msg_too_long "Nickname is too long!\n"
#define msg_taken "Nickname is already taken!\n"
#define msg_inva_char "Invalid characters used!\n"
#define msg_invalid_nick "Invalid nickname!\n"
#define msg_nick_required "Nickname is required!\n"
#define msg_cmd_unknown "Unknown command!\n"
#define msg_cmd_too_long "Command is too long!\n"
#define msg_not_authorized "You are not authorized!\n"
#define msg_already_reg "You are already registered!\n"
#define msg_no_such_channel "No such channel exists!\n"
#define msg_invite_only "Channel is invite-only!\n"
#define msg_banned "You are banned from this channel!\n"
#define msg_user_limit "Channel user limit reached!\n"
#define msg_no_privileges "You do not have the necessary privileges!\n"
#define msg_server_full "Server is full!\n"
#define msg_ping_timeout "Ping timeout!\n"
#define msg_invalid_mode "Invalid mode!\n"
#define msg_bad_password "Incorrect password!\n"
#define msg_nick_change_denied "Nickname change denied!\n"
#define msg_invalid_topic "Invalid topic format!\n"
#define msg_channel_locked "Channel is locked!\n"
#define msg_no_join_permission "You don't have permission to join this channel!\n"


