#include "../Includes/replyCodes.hpp"
#include "../Includes/IrcServer.hpp"
#include "../Includes/Channel.hpp"
#include "../Includes/Client.hpp"
#include <sys/select.h> // fd_set

IrcServer::IrcServer(int port, std::string passw) 
    : _port(port), _password(passw), _servname("irc-server") { }

Client *IrcServer::getClientByfd(int clientSock) {
    try {
        Client *client = _clientsBySock.at(clientSock);
        return client;
    } catch(const std::exception &e) {
        return nullptr;
    }
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
    if (fcntl(_servSockfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl failed!");
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(_servSockfd, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        throw std::runtime_error("bind failed!");
    if (listen(_servSockfd, 5) < 0)
        throw std::runtime_error("listen failed!");
    std::cout << "Server is lsitening on port " << this->_port << "..." << std::endl;
}

void IrcServer::newConnection() {
    sockaddr_in clientAddress;
    socklen_t   addressLength = sizeof(clientAddress);
    size_t clientSockfd = accept(_servSockfd, (sockaddr*)&clientAddress, &addressLength);
    if (clientSockfd < 0)
        throw std::runtime_error("accept failed!"); //TO DO may continue based on errno
    std::cout << "Detected a new connection!\n";
    if (fcntl(clientSockfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("accept failed!"); // TO DO may continue
    if (clientSockfd > _highestfd) _highestfd = clientSockfd;
    FD_SET(clientSockfd, &_masterSet);
    Client *newClient = new Client(clientSockfd, _servname);
    newClient->_dataWaiting = false;
    newClient->setStage(NOTHING_SET);
    newClient->setState(ANNONYMOUS);
    newClient->setHost(clientAddress);
    _clientsBySock.insert(std::make_pair(clientSockfd, newClient));
}

void IrcServer::parseMessage(Client *client) {
    // check empty buffer
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
}

void IrcServer::newMessage(int clientSock) {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    int recvBytes = recv(clientSock, buffer, sizeof(buffer), 0);
    if (!recvBytes) {
        this->disconnected(clientSock);
    } else if (recvBytes < 0)
        throw std::runtime_error("recv failed"); // TO DO can be continued
    else {
        Client *client = this->getClientByfd(clientSock);
        client->setRecvBuffer(buffer);
        this->parseMessage(client);
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
    if (checkNickTaken(allparams[0])) {
        return numericReply(client, ERR_ALREADYREGISTRED, cmd, msg_taken);
    } else if (std::isdigit(allparams[0][0]))
        return numericReply(client, ERR_ERRONEUSNICKNAME, cmd, msg_digit_start);
    else if (!validateAscii(allparams[0], cmd))
        return numericReply(client, ERR_ERRONEUSNICKNAME, cmd, msg_inva_char);
    client->setNickName(allparams[0]);
    if (client->getStage() == USER_SET) {
        client->setState(REGISTERED);
        _clientsByNick.insert(std::make_pair(allparams[0], client));
        this->sendWelcomeMsg(client);
    } else if (client->getStage() == NOTHING_SET)
        client->setStage(NICK_SET);
}

void IrcServer::handleUser(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "USER";
    if (client->getState() == REGISTERED)
        return numericReply(client, ERR_ALREADYREGISTRED, cmd, msg_already_reg);
    else if (allparams.size() < 4)
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
    if (client->getStage() == NICK_SET) {
        client->setState(REGISTERED);
        _clientsByNick.insert(std::make_pair(client->getNickName(), client));
        this->sendWelcomeMsg(client);
    } else if (client->getStage() == NOTHING_SET) 
        client->setStage(USER_SET);
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

//JOIN #chan1,#chan2,#chan3 ,,key3
void IrcServer::channelManager(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "JOIN", channelname;
    if (allparams.size() > 2)
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);
    std::vector<std::string> channels = this->seperator(allparams[0], ',');
    std::vector<std::string> keys;
    if (allparams.size() == 2)
        keys = this->seperator(allparams[1], ',');
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i][0] != '#' && channels[i][0] != '&') {
            numericReply(client, ERR_NOSUCHCHANNEL, cmd, msg_no_such_channel); // can add name
            continue ;
        } else if (channelExists(channels[i]) == true) { // channel exists
            Channel *channel = _allChannels[channels[i]];
             if (channel->isUserInChannel(client)) {
                numericReply(client, ERR_USERONCHANNEL, cmd, msg_useronchannel);
                continue ;
            } else if (channel->getLimit() > 0 && (channel->getMembersCount()+1) > channel->getLimit()) {
                numericReply(client, ERR_CHANNELISFULL, cmd, msg_user_limit);
                continue ;
            } else if (channel->isUserBanned(client)) {
                numericReply(client, ERR_BANNEDFROMCHAN, cmd, msg_banned);
                continue ;
            } else if (channel->isPrivateChannel()) {
                if (channel->isUserInvited(client)) {
                    channel->addRegularMember(client);
                    std::string fullmessage = ":"+client->genHostMask()+" JOIN :"+channels[i]+"\r\n"; 
                    channel->broadcast(client, fullmessage, true);
                } else numericReply(client, ERR_INVITEONLYCHAN, cmd, msg_invite_only);
                continue ;
            }
            channel->addRegularMember(client);
            std::string fullmessage = ":"+client->genHostMask()+" JOIN :"+channels[i]+"\r\n"; 
            channel->broadcast(client, fullmessage, true); // TO DO TOPIC replys
        } else { // create a channel
            Channel *newChannel = new Channel(channels[i]);
            // client->makeUserOperator();
            newChannel->addRegularMember(client);
            _allChannels.insert(std::make_pair(channels[i], newChannel));
            std::string fullmessage = ":"+client->genHostMask()+" JOIN :"+channels[i]+"\r\n"; 
            newChannel->broadcast(client, fullmessage, true); // TO DO TOPIC replys
        }
    }
}

// PRIVMSG nick :hey there
void IrcServer::privateMsg(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "PRIVMSG", target, fullMessage;
    if (allparams.empty())
        return numericReply(client, ERR_NORECIPIENT, cmd, msg_no_recipirnt);
    if (allparams.size() < 2)
        return numericReply(client, ERR_NOTEXTTOSEND, cmd, msg_notexttosend);
    std::stringstream ss(allparams[0]);
    std::vector<std::string> targets;
    while (std::getline(ss, target, ','))
        targets.push_back(target);
    for (size_t i = 1; i < allparams.size(); i++) {
        fullMessage.append(allparams[i]);
        if (i+1!=allparams.size()) fullMessage.append(" ");
    }
    fullMessage.append("\r\n"); //add in final destination
    Client *reciver;
    for (size_t i = 0; i < targets.size(); i++) {
        if (targets[i].empty()) {
            numericReply(client, ERR_NORECIPIENT, cmd, msg_no_recipirnt);
            continue;
        } else if (targets[i].at(0) == '#' || targets[i].at(0) == '&') { // its a afucking channel
            Channel *channel = getChannel(targets[i]);
            if (!channel) {
                std::string msg = msg_no_such_channel + std::string(" ") + targets[i]; 
                numericReply(client, ERR_NOSUCHCHANNEL, cmd, msg); // can add name
                continue ;
            }
            channel->broadcast(client, fullMessage, false);
        } else {
            reciver = getClientByNick(targets[i]);
            if (!reciver) {
                this->numericReply(client, ERR_NOSUCHNICK, cmd, msg_invalid_nick);
                continue ;
            } // client not found 
            reciver->setQueueBuffer(client->genHostMask() + " " + cmd + " " + targets[i] + " " + fullMessage);
        }
    } // RPL_AWAY
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
    fd_set readReadySet;
    fd_set writeReadySet;
    _highestfd = _servSockfd;
    FD_SET(_servSockfd, &_masterSet);
    while (true) {
        FD_ZERO(&readReadySet);
        FD_ZERO(&writeReadySet);
        readReadySet = _masterSet;
        writeReadySet = _masterSet;
        // writeReadySet = this->fetchReadyClients();
        int readyforIO = select(_highestfd+1, &readReadySet, &writeReadySet, nullptr, 0);
        if (readyforIO == -1)
            throw std::runtime_error("select failed!");
        for (size_t i = 0; i <= _highestfd; i++) {
            if (FD_ISSET(i, &readReadySet)) {
                if (i == (size_t)_servSockfd) this->newConnection();
                else this->newMessage(i);
            }
        }
        for (size_t i = 0; i <= _highestfd; i++) {
            if (FD_ISSET(i, &writeReadySet)) {
                if ((int)i != _servSockfd) {
                    Client *client = getClientByfd(i);
                    if (!client) continue ;
                    if (client->_dataWaiting) this->sendQueuedData(i);
                }
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

void IrcServer::disconnected(int clientSock) {
    std::cout << "client disconnected!\n";
    FD_CLR(clientSock, &_masterSet);
    close(clientSock);
}

void IrcServer::displayAllInfo(Client *client) {
    std::cout << "---------------------------------------\n";
    std::cout << "client state    : " << client->getState() << std::endl;
    std::cout << "client stage    : " << client->getStage() << std::endl;
    std::cout << "client nick name: [" << client->getNickName() << "]" << std::endl;
    std::cout << "client user name: [" << client->getUserName() << "]" << std::endl;
    std::cout << "client full name: [" << client->getFullName() << "]" << std::endl;
    std::cout << "number of client by nick: " << _clientsByNick.size() << std::endl;
    std::cout << "number of client by fd: " << _clientsBySock.size() << std::endl;
}
