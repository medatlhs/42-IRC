# include "../Includes/replyCodes.hpp"
# include "../Includes/IrcServer.hpp"
# include "../Includes/Channel.hpp"
# include "../Includes/Client.hpp"

void IrcServer::parseMessage(Client *client) {
    std::string recivedData = client->getRecvBuffer();
    if (recivedData.empty()) return ;

    size_t npos = recivedData.find('\n');
    std::string fullIrcCommand;
    if ((npos) != std::string::npos) {
        fullIrcCommand = recivedData.substr(0, npos);
        client->getRecvBuffer().erase(0, npos + 1);
    } 

    std::stringstream ss(fullIrcCommand);
    std::string messageCmd, singleparam;
    std::vector<std::string> allparameters;
    ss >> messageCmd;
    while (ss >> singleparam)
        allparameters.push_back(singleparam);

    if (!messageCmd.compare("NICK")) 
        this->handleNick(client, allparameters);
    else if (!messageCmd.compare("USER")) 
        this->handleUser(client, allparameters);
    else if (!messageCmd.compare("PRIVMSG")) 
        this->privateMsg(client, allparameters);
    else if (!messageCmd.compare("JOIN")) 
        this->channelManager(client, allparameters);
    else if (messageCmd == "INFO") 
        this->displayAllInfo(client);
    // else numericReply(client, ERR_UNKNOWNCOMMAND, "", "Unknown command!");
}

void IrcServer::handleComingData(int clientSock) {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    int recvBytes = recv(clientSock, buffer, sizeof(buffer), 0);
    if (!recvBytes) {
        clientDisconnected(clientSock);
    } else if (recvBytes < 0)
        return std::cerr << "recv failed! Errno: " << std::strerror(errno), void();
    else {
        Client *client = this->getClientByfd(clientSock);
        if (!client) 
            return ;
        client->setRecvBuffer(buffer);
        parseMessage(client);
    }
}
