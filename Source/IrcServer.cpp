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
    std::cout << "IrcServer got destroyed\n";
}
// USER <username> <mode> <unused> :<realname> 
void IrcServer::parseCommand(int clientSocket_) {
    std::vector<std::string> allParams;
    Client *client = this->clients.at(clientSocket_);
    std::stringstream ss(client->getBuffer());
    std::string command;
    if (!(ss >> command))
        std::cout << "Error: Client provided an empty command!\n"; // TO DO
    std::string param;
    while (ss >> param)
        allParams.push_back(param);
    if (command == "NICK")
        client->setNickName(allParams, clients);
    client->clearBuffer();
}

void IrcServer::setupServer() {
    (void)this->servPort;
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
    std::cout << "handled one: " << clientSocket_ << std::endl;
    char buffer[1024] = {0};
    ssize_t recvBytes = recv(clientSocket_, &buffer, sizeof(buffer), 0);
    if (recvBytes < 0) {
        std::cerr << "Error: Receiving data!\n"; exit(1);
    } else if (!recvBytes) { // client dicoonnected
        std::cout << "client disconected!\n";
        close(clientSocket_);
        FD_CLR(clientSocket_, &this->masterfds);
        // Remove client from clients // TO DO
    } else {
        try {
            std::cout << clientSocket_ << std::endl;
            Client *myclient = this->clients.at(clientSocket_);
            myclient->setBuffer(buffer);
        } catch(const std::exception& e) {
            std::cerr << e.what() << "here\n";
        }
        parseCommand(clientSocket_);
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
            if (FD_ISSET(i, &readfds)) { // isin the set
                if (i == this->servSocket) { // new connection on master socket
                    int clientSock = accept(this->servSocket, nullptr, nullptr);
                    if (clientSock < 0) {
                        std::cerr << "Error: Accepting a connection!", exit(1);
                    }
                    std::cout << "Detected a new connection\n";
                    if (clientSock > maxfds)
                        maxfds = clientSock;
                    FD_SET(clientSock, &this->masterfds);
                    std::cout << "the scoket client: " << clientSock << std::endl;
                    Client *newClient = new Client(clientSock);
                    newClient->_state = ANNONYMOUS;
                    this->clients.insert(std::make_pair(clientSock, newClient));
                } else
                    this->handleDataReq(i);
            }
        }
    }
}


