# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"

IrcServer::IrcServer(int port, std::string passw) 
    : _port(port), _password(passw), _servname("irc-server") { }

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
/*
JOIN # 
:medox!med@127.0.0.1 JOIN :#

:medox!med@127.0.0.1 PRIVMSG #med ::hey man

*/
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

void IrcServer::startAccepting() {
    FD_SET(_servSockfd, &_masterSet);
    _highestfd = _servSockfd;
    while (true) {
        fd_set readReady = _masterSet;
        fd_set writeReady = _masterSet;
        int nreadyfds = select(_highestfd + 1, &readReady, &writeReady, nullptr, 0);
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
