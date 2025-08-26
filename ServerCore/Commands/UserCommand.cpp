# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"

void IrcServer::handleUser(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "USER";
    if (client->getClientState() == REGISTERED)
        return numericReply(client, ERR_ALREADYREGISTRED, cmd, msg_already_reg);
    
    if (allparams.size() < 4)
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);

    if (!validateAscii(allparams[0], cmd))
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_inva_char);

    bool seenColon = false;
    for (size_t i = 0; i < allparams.size(); i++) {
        if (allparams[i].find(':') != std::string::npos) {
            if (seenColon || i != 3 || (i == 3 && allparams[i][0] != ':') )
                return numericReply(client, ERR_NEEDMOREPARAMS, cmd, "colon issue");
            seenColon = true;
        }
    }

    if (!seenColon) 
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);
    std::string fullname; // params are valid
    for (size_t i = 3; i < allparams.size(); i++) {
        fullname.append(allparams[i]);
        if (i + 1 != allparams.size()) fullname.append(" ");
    }

    client->setUserName(allparams[0]);
    client->setFullName(fullname.erase(0, 1));
    if (client->getLoginStage() == NICK_SET) {
        client->setClientState(REGISTERED);
        _clientsByNick[client->getNickName()] = client;
        sendWelcomeMsg(client);
    } else if (client->getLoginStage() == NOTHING_SET) 
        client->setLoginStage(USER_SET);
}
