#include "../Includes/IrcServer.hpp"
#include "../Includes/Client.hpp"
#include "../Includes/Channel.hpp"

IrcServer::IrcServer() : servPort(PORT), servPass("default") { }

IrcServer::IrcServer(int port, std::string passw) : servPort(port), servPass(passw) {
    std::cout << "server is being created!\n";
    this->setupServer();
    this->startAccepting();
}

IrcServer::~IrcServer() {
    std::cout << "IrcServer got destroted\n";
}

void IrcServer::setupServer() {
    if ((this->servSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: Building a socket!"; exit(1);
    }
    if (fcntl(this->servSocket, F_SETFL, O_NONBLOCK) < 0) {
            std::cerr << "Error: Setting socket to non-blocking!" << std::endl;
            exit(1);
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(servSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error: Binding a socket!"; exit(1);
    }
    if (listen(this->servSocket, 5) < 0) {
        std::cerr << "Error: Listeing!"; exit(1);
    }
    std::cout << "Server is lsitening on port " << PORT << "..." << std::endl;
}

void IrcServer::handleDataReq(int clientSocket_) {
    char buffer[1024] = {0};
    ssize_t recvBytes = recv(clientSocket_, &buffer, sizeof(buffer), 0);
    if (recvBytes<0) {
        std::cerr << "Error: Receiving data!\n"; exit(1);
    } else if (recvBytes == 0) { // client dicoonnected
        std::cout << "client disconected!\n";
        close(clientSocket_);
        FD_CLR(clientSocket_, &this->masterfds);
        // Remove client from clients // TO DO
    } else {
        try {
            this->clients.at(clientSocket_)->_clientBuffer.append(buffer);
            std::cout << this->clients.at(clientSocket_)->_clientBuffer << std::endl;
        } catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
}

void IrcServer::startAccepting() {
    fd_set readfds;
    FD_ZERO(&this->masterfds);
    FD_SET(this->servSocket, &this->masterfds);
    int maxfds = this->servSocket;
    while (true) {
        FD_ZERO(&readfds);
        readfds = this->masterfds;
        int readyForIO = select(maxfds+1, &readfds, nullptr, nullptr, 0);
        if (readyForIO < 0) {
            std::cout << "Error: select() failed!"; exit(1);
        }
        for (int i = 0; i < (maxfds + 1); i++) {
            if (FD_ISSET(i, &readfds)) {
                if (i == this->servSocket) { // new connection on master socket
                    int clientSock = accept(this->servSocket, nullptr, nullptr);
                    if (clientSock < 0) {
                        std::cerr << "Error: Accepting a connection!", exit(1);
                    }
                    std::cout << "Detected a new connection\n";
                    if (clientSock > maxfds)
                        maxfds = clientSock;
                    FD_SET(clientSock, &this->masterfds);
                    Client *newClient = new Client(i);
                    newClient->_state = ANNONYMOUS;
                    this->clients.insert(std::make_pair(clientSock, newClient));
                } else
                    this->handleDataReq(i);
            }
        }
    }
}


