#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fcntl.h>
#include <sys/time.h>
#include <map>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h> // sockaddress_in
#include <vector>
#include <unistd.h>
#include <arpa/inet.h> // for inet
#include <netinet/in.h> 
#include <sstream>
#include "./replyCodes.hpp"
#include "./Channel.hpp"

enum ClientState { ANNONYMOUS, REGISTERED, DISCONNECTED };
enum LoginStage { NOTHING_SET, NICK_SET, USER_SET };
class Channel;
class Client {
    private:
        int         _clientSocket;
        std::string _nickName;
        std::string _userName;
        std::string _fullName;
        std::string _mode;
        std::string _host;
        std::string _unused;
        std::string _servname;

        ClientState _state;
        LoginStage  _stage;

        std::string _recvBuffer;
        std::string _queueBuffer;
        std::vector<Channel *> _joinedChans;
    public:
        bool        _dataWaiting;
        Client(int clientSocket, const std::string&servname);
        void setNickName(const std::string &nick);
        void setUserName(const std::string &user);
        void setFullName(const std::string &name);
        void setLoginStage(LoginStage stage);
        void setClientState(ClientState state);
        void setRecvBuffer(const std::string newData);
        void setQueueBuffer(const std::string newData);
        void setHost(sockaddr_in clientAddr);

        //utils
        std::string genHostMask(void);

        int         getSockfd();
        std::string &getNickName(void);
        std::string &getUserName(void);
        std::string &getRealName(void);
        std::string &getFullName(void);
        LoginStage  &getLoginStage(void);
        ClientState &getClientState(void);
        std::string &getRecvBuffer(void);
        std::string &getQueueBuffer(void);

        void addChannelMembership(Channel *channel);
        void removeChannelMembership(Channel *channel);
        bool isChannelMember(Channel *channel);
        std::vector<Channel *> &getJoindChans(void);
};

