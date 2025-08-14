#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "./InformCodes.hpp"
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

enum ClientState {ANNONYMOUS, NICK_SET, USER_SET, REGISTERED ,DISCONNECTED};

class Client {
    private:
        int         _clientSocket;
        std::string _nickName;
        std::string _userName;
        std::string _realName;
        std::string _clientBuffer;
    public:
        ClientState _state;

        Client(int clientSocket);

        void setNickName(std::vector<std::string> &allParams, std::map<int, Client*> &clients);
        void sendError(int numiCode, std::string nickN, std::string params, std::string fullMsj);
        //setters and lfucking getters
        void setUserName(const std::string &userName);
        void setRealName(const std::string &realName);
        void setBuffer(std::string newData);
        void clearBuffer(void);
        std::string &getNickName(void);
        std::string &getUserName(void);
        std::string &getRealName(void);
        std::string &getBuffer(void);
};
