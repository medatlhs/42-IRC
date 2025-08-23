#pragma once
#include <fcntl.h>
#include <sys/time.h>
#include <map>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h> //sockaddress_in
#include <vector>
#include <unistd.h>
#include <sstream>
#include "./replyCodes.hpp"
#include "./Client.hpp"
// #include "Channel.hpp"
class Client;
class Channel;

class IrcServer {
    private:
        int         _port;
        std::string _password;
        int         _servSockfd;
        size_t      _highestfd;
        fd_set      _masterSet;
        std::string _servname;
        std::map<int, Client*> _clientsBySock;
        std::map<std::string, Client *>  _clientsByNick;
        std::map<std::string, Channel*>  _allChannels;
    public:
        IrcServer(int port, std::string passw);

        void setupServer(void);
        void startAccepting(void);

        void newConnection();
        void newMessage(int clientSock);
        void disconnected(int clientSock);

        void sendQueuedData(int clientSock);

        bool checkNickTaken(const std::string &nick);
        Client *getClientByNick(const std::string &nick);
        Client *getClientByfd(int clientSock);
        Channel *getChannel(const std::string &channelname);

        void    parseMessage(Client *client);
        void    handleNick(Client *client, std::vector<std::string> &allparams);
        void    handleUser(Client *client, std::vector<std::string> &allparams);
        void    privateMsg(Client *client, std::vector<std::string> &allparams);
        

        void numericReply(Client *client, int code, std::string params, std::string msj);
        void sendWelcomeMsg(Client *client);
        // fd_set fetchReadyClients();

        //utils
        bool validateAscii(std::string&input, std::string&cmd);
        std::vector<std::string> seperator(std::string &str, char c);
        void displayAllInfo(Client *client);

        void channelManager(Client *client, std::vector<std::string> &allparams);
        bool channelExists(const std::string &channelname);
        bool checkIfOnChannel(Client *client, const std::string &channelname);
        // ~IrcServer();
};


