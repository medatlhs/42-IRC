# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"

void IrcServer::sendQueuedData(int clientSock) {
    Client *client = this->getClientByfd(clientSock);
    if (!client || !client->_dataWaiting) return;
    while (true) {
        size_t pos = client->getQueueBuffer().find("\r\n");
        if (pos == std::string::npos) 
            break ;
        std::string completeReply = client->getQueueBuffer().substr(0, pos);
        completeReply.append("\r\n");
        const char *data = completeReply.c_str();
        size_t totalSent = 0;
        while (totalSent < completeReply.size()) {
            // int bytesSent = send(clientSock, data + totalSent, completeReply.size() - totalSent, 0);
            int bytesSent = send(clientSock, data + totalSent, 1, 0);
            if (bytesSent <= 0)
                return std::cerr << std::strerror(errno) << std::endl, void();
            totalSent += bytesSent;
            usleep(10000);
        }
        client->getQueueBuffer().erase(0, pos + 2);
    }
    if (client->getQueueBuffer().empty()) client->_dataWaiting = false;
}

void IrcServer::numericReply(Client *client, int code, std::string params, std::string msj) {
    std::stringstream ss;
    std::string numericCode = std::to_string(code);
    if (code < 10) numericCode = "00" + std::to_string(code);
    else if (code < 100) numericCode = "0" + std::to_string(code);
    ss  << ":" << _servname << " " << numericCode << " " << client->getNickName();
    if (!params.empty()) ss << " ";
    ss << params << " :" << msj << "\r\n";
    client->setQueueBuffer(ss.str());
}
void IrcServer::sendWelcomeMsg(Client *client) {
    std::string fullMsg001 = "Welcome to the internet relay chat " + client->genHostMask();
    numericReply(client, 1, "", fullMsg001);
    std::string fullMsg002 = "Your host is localhost, running version 1.0 beta";
    numericReply(client, 2, "", fullMsg002);
    std::string fullMsg003 = "This server was created 15 Aug 2025 at 22:00:00";
    numericReply(client, 3, "", fullMsg003);
}

void IrcServer::displayAllInfo(Client *client) {
    std::cout << "---------------------------------------\n";
    std::cout << "client state    : " << client->getClientState() << std::endl;
    std::cout << "client stage    : " << client->getLoginStage() << std::endl;
    std::cout << "client nick name: [" << client->getNickName() << "]" << std::endl;
    std::cout << "client user name: [" << client->getUserName() << "]" << std::endl;
    std::cout << "client full name: [" << client->getFullName() << "]" << std::endl;
    std::cout << "number of client by nick: " << _clientsByNick.size() << std::endl;
    std::cout << "number of client by fd: " << _clientsBySock.size() << std::endl;
}

