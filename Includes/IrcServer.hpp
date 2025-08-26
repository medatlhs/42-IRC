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

        void handleNewConnection();
        void handleComingData(int clientSock);
        void clientDisconnected(int clientSock);

        void sendQueuedData(int clientSock);

        bool    checkNickTaken(const std::string &nick);
        Client  *getClientByNick(const std::string &nick);
        Client  *getClientByfd(int clientSock);
        Channel *getChannel(const std::string &channelname);

        void    parseMessage(Client *client);
        void    handleNick(Client *client, std::vector<std::string> &allparams);
        void    handleUser(Client *client, std::vector<std::string> &allparams);
        
        //privatemessage
        void privateMsg(Client* client, std::vector<std::string>& allparams);
        void sendPrivmsgToUser(Client* sender, const std::string& nick, const std::string& message);
        void sendPrivmsgToChannel(Client* sender, const std::string& chanName, const std::string& message);
        std::string buildPrivmsg(Client* sender, const std::string& target, const std::vector<std::string>& allparams);

        void numericReply(Client *client, int code, std::string params, std::string msj);
        void sendWelcomeMsg(Client *client);
        // fd_set fetchReadyClients();

        //utils
        bool validateAscii(std::string&input, std::string&cmd);
        std::vector<std::string> seperator(std::string &str, char c);
        void displayAllInfo(Client *client);

        bool isValidChannelName(const std::string& name);
        bool channelExists(const std::string &channelname);
        void handleJoinExisting(Channel* channel, Client* client, const std::string& channelName);
        void handleCreateChannel(Client* client, const std::string& channelName);
        void channelManager(Client *client, std::vector<std::string> &allparams);
        // ~IrcServer();
};


