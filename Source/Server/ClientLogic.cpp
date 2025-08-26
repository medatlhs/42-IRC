# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"

void IrcServer::sendQueuedData(int clientSock) {
    Client *client = this->getClientByfd(clientSock);
    if (!client || !client->_dataWaiting) return;
    while (true) {
        size_t pos = client->getQueueBuffer().find("\r\n");
        if (pos == std::string::npos) break ;
        std::string completeReply = client->getQueueBuffer().substr(0, pos); // +1 for \n
        completeReply.append("\r\n");
        const char *data = completeReply.c_str();
        size_t totalSent = 0;
        while (totalSent < completeReply.size()) {
            int bytesSent = send(clientSock, data + totalSent, completeReply.size() - totalSent, 0);
            if (bytesSent <= 0)
                return std::cerr << std::strerror(errno) << std::endl, void();
            totalSent += bytesSent;
        }
        client->getQueueBuffer().erase(0, pos + 2);
    }
    if (client->getQueueBuffer().empty()) client->_dataWaiting = false;
}

void IrcServer::clientDisconnected(int clientSock) {
    std::cout << "client disconnected!\n";
    Client *client = getClientByfd(clientSock);
    if (!client) return ;
    _clientsBySock.erase(clientSock);
    if (!client->getNickName().empty()) _clientsByNick.erase(client->getNickName());
    std::vector<Channel *> &joinedChans = client->getJoindChans(); // remove from channels
    for (size_t i = 0; i < joinedChans.size(); i++) {
        if (joinedChans[i]->isUserInChannel(client))
            joinedChans[i]->removeMember(client);
        if (joinedChans[i]->isUserInvited(client))
            joinedChans[i]->removeInvitation(client);
        if (joinedChans[i]->isUserBanned(client))
            joinedChans[i]->unbanUser(client);
        if (joinedChans[i]->getMembersCount() == 0) {
            _allChannels.erase(joinedChans[i]->getChannelName());
            delete joinedChans[i];
            continue ;
        }
        //:john!johnuser@host.example.com QUIT :Client disconnected
        std::string channelsAlert =  ":" + client->genHostMask() + " QUIT :Client disconnected" + "\r\n";
        joinedChans[i]->broadcast(client, channelsAlert, false);
    }
    FD_CLR(clientSock, &_masterSet);
    close(clientSock);
    delete client;
}
