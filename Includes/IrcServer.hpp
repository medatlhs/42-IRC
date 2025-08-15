#pragma once
#include <fcntl.h>
#include <sys/time.h>
#include <map>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h> // sockaddress_in
#include <vector>
#include <unistd.h>
#include <sstream>
#include "Client.hpp"
#include "Channel.hpp"

#define PORT 4040

class IrcServer {
    private:
        int         servPort;
        int         servSocket;
        fd_set      masterfds;
        std::string servPass;
        std::string _servName;

        std::map<int, Client *> clients;
        std::map<std::string, Channel *> channels;

    public:
        IrcServer();
        IrcServer(int port, std::string passw);

        void setupServer(void);
        void startAccepting(void);
        void handleDataReq(int clientSocket_);
        void parseCommand(int clientSocket_);

        ~IrcServer();
};

