# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"


bool IrcServer::checkNickTaken(const std::string &nick) {
    std::map<std::string, Client *>::iterator it;
    for (it = _clientsByNick.begin(); it != _clientsByNick.end(); it++)
        if (!nick.compare(it->first)) return true ;
    return false;
}

void IrcServer::handleNick(Client *client, std::vector<std::string> &allparams) {

    std::string cmd = "NICK";
    if (allparams.size() < 1)
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);
    if (checkNickTaken(allparams[0]))
        return numericReply(client, ERR_ALREADYREGISTRED, cmd, msg_taken);
    if (std::isdigit(allparams[0][0]))
        return numericReply(client, ERR_ERRONEUSNICKNAME, cmd, msg_digit_start);
    if (!validateAscii(allparams[0], cmd))
        return numericReply(client, ERR_ERRONEUSNICKNAME, cmd, msg_inva_char);

    client->setNickName(allparams[0]);
    if (client->getLoginStage() == USER_SET) {
        client->setClientState(REGISTERED);
        _clientsByNick[allparams[0]] = client;
        sendWelcomeMsg(client);
    } else if (client->getLoginStage() == NOTHING_SET)
        client->setLoginStage(NICK_SET);
}
