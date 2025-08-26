# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"

void IrcServer::sendPrivmsgToChannel(Client* sender, const std::string& chanName
    , const std::string& message) {
    Channel* channel = getChannel(chanName);
    if (!channel) {
        std::string message = msg_no_such_channel + std::string(" ") + chanName;
        numericReply(sender, ERR_NOSUCHCHANNEL, "PRIVMSG", message);
        return;
    }
    channel->broadcast(sender, message, false);
}

void IrcServer::sendPrivmsgToUser(Client* sender, const std::string& nick
    , const std::string& message) {
    Client* receiver = getClientByNick(nick);
    if (!receiver) {
        numericReply(sender, ERR_NOSUCHNICK, "PRIVMSG", msg_invalid_nick);
        return;
    }
    receiver->setQueueBuffer(message); // TO DO If user is away and also send RPL_AWAY
}

void IrcServer::privateMsg(Client* client, std::vector<std::string>& allparams) {
    std::string cmd = "PRIVMSG";
    if (allparams.empty())
        return numericReply(client, ERR_NORECIPIENT, cmd, msg_no_recipirnt);
    if (allparams.size() < 2)
        return numericReply(client, ERR_NOTEXTTOSEND, cmd, msg_notexttosend);

    std::vector<std::string> targets;
    std::stringstream ss(allparams[0]);
    std::string target;
    while (std::getline(ss, target, ',')) {
        if (!target.empty())
            targets.push_back(target);
    }

    for (size_t i = 0; i < targets.size(); ++i) { //building the going message 
        std::string message = buildPrivmsg(client, targets[i], allparams);
        if (targets[i][0] == '#' || targets[i][0] == '&') {
            sendPrivmsgToChannel(client, targets[i], message);
        } else {
            sendPrivmsgToUser(client, targets[i], message);
        }
    }
}

std::string IrcServer::buildPrivmsg(Client* sender, const std::string& target
    , const std::vector<std::string>& allparams) {
    std::ostringstream ss;
    ss << ":" << sender->genHostMask() << " PRIVMSG " << target << " :";
    for (size_t i = 1; i < allparams.size(); ++i) {
        ss << allparams[i];
        if (i + 1 != allparams.size()) ss << " ";
    }
    ss << "\r\n";
    return ss.str();
}
