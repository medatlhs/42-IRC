#include "../Includes/replyCodes.hpp"
#include "../Includes/IrcServer.hpp"
#include "../Includes/Channel.hpp"
#include "../Includes/Client.hpp"
#include <sys/select.h> // fd_set

IrcServer::IrcServer(int port, std::string passw) 
    : _port(port), _password(passw), _servname("irc-server") { }

Client *IrcServer::getClientByfd(int clientSock) {
    std::map<int, Client *>::iterator it;
    for (it = _clientsBySock.begin(); it != _clientsBySock.end(); it++)
        if (it->first == clientSock)
            return it->second;
    return nullptr;
}

Client *IrcServer::getClientByNick(const std::string&nick) {
    std::cout << "nick: [" << nick << "]" << std::endl;
    try {
        Client *client = _clientsByNick.at(nick);
        return client;
    } catch(const std::exception &e) {
        std::cout << "clientbynick\n";
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

Channel *IrcServer::getChannel(const std::string &channelname) {
    try {
        Channel *channel = _allChannels.at(channelname);
        return channel;
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}

bool IrcServer::checkNickTaken(const std::string &nick) {
    std::map<std::string, Client *>::iterator it;
    for (it = _clientsByNick.begin(); it != _clientsByNick.end(); it++)
        if (!nick.compare(it->first)) return true ;
    return false;
}

void IrcServer::setupServer() {
    _servSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_servSockfd < 0)
        throw std::runtime_error("socket failed!");
    int yes = 1;
    if (setsockopt(_servSockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        std::cerr << "setsockopt failed! Errno: " << std::strerror(errno) << std::endl;
    if (fcntl(_servSockfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl failed!");
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(_servSockfd, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        throw std::runtime_error("bind failed!");
    if (listen(_servSockfd, 10) < 0)
        throw std::runtime_error("listen failed!");
    std::cout << "Irc Server lsitening on port [" << this->_port << "] ..." << std::endl;
}

void IrcServer::handleNewConnection() {
    sockaddr_in clientAddress;
    socklen_t   addressLength = sizeof(clientAddress);
    int clientSockfd = accept(_servSockfd, (sockaddr*)&clientAddress, &addressLength);
    if (clientSockfd == -1)
        return std::cerr << "accept failed! Errno: " << std::strerror(errno), void();
    std::cout << "Detected a new connection!\n";
    if (fcntl(clientSockfd, F_SETFL, O_NONBLOCK) == -1)
        return std::cerr << "fncntl failed!", void();
    if ((size_t)clientSockfd > _highestfd) 
        _highestfd = clientSockfd;
    FD_SET(clientSockfd, &_masterSet);
    Client *newClient = new Client(clientSockfd, _servname);
    newClient->_dataWaiting = false;
    newClient->setLoginStage(NOTHING_SET);
    newClient->setClientState(ANNONYMOUS);
    newClient->setHost(clientAddress);
    _clientsBySock[clientSockfd] = newClient;
}

void IrcServer::parseMessage(Client *client) {
    if (client->getRecvBuffer().empty())
        return ;
    std::stringstream ss(client->getRecvBuffer());
    std::string messageCmd, singleparam;
    std::vector<std::string> allparameters;
    ss >> messageCmd;
    while (ss >> singleparam)
        allparameters.push_back(singleparam);
    if (!messageCmd.compare("NICK")) this->handleNick(client, allparameters);
    else if (!messageCmd.compare("USER")) this->handleUser(client, allparameters);
    else if (!messageCmd.compare("PRIVMSG")) this->privateMsg(client, allparameters);
    else if (!messageCmd.compare("JOIN")) this->channelManager(client, allparameters);
    else if (messageCmd == "INFO") this->displayAllInfo(client);
    else numericReply(client, ERR_UNKNOWNCOMMAND, "", "Unknown command!");
}

void IrcServer::clientDisconnected(int clientSock) {
    std::cout << "client disconnected!\n";
    Client *client = getClientByfd(clientSock);

    if ((client->getClientState()==ANNONYMOUS && client->getLoginStage()==NICK_SET)
        || client->getClientState() == REGISTERED) { //  nick set
        _clientsByNick.erase(client->getNickName());
    }

    
    _clientsBySock.erase(clientSock);
    delete client;
    // chan->broadcast(":" + client->getNick() + " QUIT :Client disconnected\r\n",
    FD_CLR(clientSock, &_masterSet);
    close(clientSock);
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
        if (!client) return ;
        client->setRecvBuffer(buffer);
        parseMessage(client);
    }
}

void IrcServer::numericReply(Client *client, int code, std::string params, std::string msj) {
    std::stringstream ss;
    std::string numericCode = std::to_string(code);
    if (code < 10) 
        numericCode = "00" + std::to_string(code);
    ss  << ":" << _servname << " " << numericCode << " " << client->getNickName();
    if (!params.empty()) ss << " ";
    ss << params << " :" << msj << "\n";
    if (!client->_dataWaiting) client->setQueueBuffer(ss.str());
    else client->setQueueBuffer(client->getQueueBuffer().append(ss.str()));
    client->_dataWaiting = true;
}

bool IrcServer::validateAscii(std::string &input, std::string &cmd) {
    (void)cmd;
    std::string notAllowed = "#@:,!&?.";
    for (size_t i = 0; i < input.length(); i++)
        if (notAllowed.find(input[i]) != std::string::npos)
            return false ;
    return true;
}

void IrcServer::handleNick(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "NICK";

    if (allparams.size() != 1)
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
        this->sendWelcomeMsg(client);
    } else if (client->getLoginStage() == NOTHING_SET)
        client->setLoginStage(NICK_SET);
}

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
    if (!seenColon) return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);
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
        this->sendWelcomeMsg(client);
    } else if (client->getLoginStage() == NOTHING_SET) 
        client->setLoginStage(USER_SET);
}

std::vector<std::string> IrcServer::seperator(std::string &str, char c) {
    std::stringstream ss(str);
    std::vector<std::string> container;
    std::string element;
    while (std::getline(ss, element, c))
        container.push_back(element);
    return container;
}
//channels
bool IrcServer::channelExists(const std::string &channelname) {
    return _allChannels.find(channelname) != _allChannels.end();
}

bool IrcServer::isValidChannelName(const std::string& name) {
    return !name.empty() && (name[0] == '#' || name[0] == '&');
}

void IrcServer::handleJoinExisting(Channel* channel, Client* client, const std::string& channelName) {
    if (channel->isUserInChannel(client))
        return numericReply(client, ERR_USERONCHANNEL, "JOIN", msg_useronchannel);
    if (channel->getLimit() > 0 && (channel->getMembersCount() + 1) > channel->getLimit())
        return numericReply(client, ERR_CHANNELISFULL, "JOIN", msg_user_limit);
    if (channel->isUserBanned(client))
        return numericReply(client, ERR_BANNEDFROMCHAN, "JOIN", msg_banned);
    if (channel->isPrivateChannel() && !channel->isUserInvited(client))
        return numericReply(client, ERR_INVITEONLYCHAN, "JOIN", msg_invite_only);
    channel->addRegularMember(client);
    broadcastJoin(channel, client, channelName);
}

void IrcServer::broadcastJoin(Channel* channel, Client* client, const std::string& channelName) {
    std::string fullMessage = ":" + client->genHostMask() + " JOIN :" + channelName + "\r\n";
    channel->broadcast(client, fullMessage, true); // TODO RPL_TOPIC RPL_NAMREPLY
}

void IrcServer::handleCreateChannel(Client* client, const std::string& channelName) {
    Channel* newChannel = new Channel(channelName);
    newChannel->addRegularMember(client);
    client->addtoJoindChannels(channelName);
    _allChannels[channelName] = newChannel;
    broadcastJoin(newChannel, client, channelName);
}

void IrcServer::channelManager(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "JOIN";
    if (allparams.size() > 2)
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);

    std::vector<std::string> channels = seperator(allparams[0], ',');
    std::vector<std::string> keys;
    if (allparams.size() == 2)
        keys = seperator(allparams[1], ',');

    for (size_t i = 0; i < channels.size(); i++) {
        std::string chanName = channels[i];
        if (!isValidChannelName(chanName)) {
            std::string msg = msg_no_such_channel + std::string(" ") + chanName;
            numericReply(client, ERR_NOSUCHCHANNEL, cmd, msg_no_such_channel);
            continue;
        }

        if (channelExists(chanName)) {
            handleJoinExisting(_allChannels[chanName], client, chanName);
        } else {
            handleCreateChannel(client, chanName);
        }
    }
}

// PRIVMSG
std::string IrcServer::buildPrivmsg(Client* sender, const std::string& target, const std::vector<std::string>& allparams) {
    std::ostringstream ss;
    ss << ":" << sender->genHostMask() << " PRIVMSG " << target << " :";
    for (size_t i = 1; i < allparams.size(); ++i) {
        ss << allparams[i];
        if (i + 1 != allparams.size()) ss << " ";
    }
    return ss.str();
}

void IrcServer::sendPrivmsgToChannel(Client* sender, const std::string& chanName, const std::string& message) {
    Channel* channel = getChannel(chanName);
    if (!channel) {
        numericReply(sender, ERR_NOSUCHCHANNEL, "PRIVMSG", msg_no_such_channel + std::string(" ") + chanName);
        return;
    }
    channel->broadcast(sender, message, false);
}

void IrcServer::sendPrivmsgToUser(Client* sender, const std::string& nick, const std::string& message) {
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

void IrcServer::sendQueuedData(int clientSock) {
    Client *client = this->getClientByfd(clientSock);
    const char *data = client->getQueueBuffer().c_str();
    int bytesSent = send(clientSock, data, client->getQueueBuffer().length(), 0);
    if (bytesSent <= 0) 
        std::cout << "Error: transmitting data\n";
    else
        client->_dataWaiting = false;
}

void IrcServer::startAccepting() {
    FD_SET(_servSockfd, &_masterSet);
    _highestfd = _servSockfd;
    while (true) {
        fd_set  readReady = _masterSet;
        fd_set  writeReady = _masterSet;
        int     nreadyfds = select(_highestfd + 1, &readReady, &writeReady, nullptr, 0);
         if (nreadyfds == -1) {
            std::cerr << "select failed! Errno: " << std::strerror(errno);
            continue ;
         }

        for (size_t fd = 0; fd <= _highestfd; ++fd) {
            if (FD_ISSET(fd, &readReady)) {
                if (fd == (size_t)_servSockfd) handleNewConnection();
                else handleComingData(fd);
            }
        }

        for (size_t fd = 0; fd <= _highestfd; ++fd) {
            if (FD_ISSET(fd, &writeReady)) {
                sendQueuedData(fd);
            }
        }
    }
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
