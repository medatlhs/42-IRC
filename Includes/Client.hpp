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

enum ClientState { ANNONYMOUS, REGISTERED, DISCONNECTED };
enum LoginStage { NOTHING_SET, NICK_SET, USER_SET };
class Client {
    private:
        int         _clientSocket;
        std::string _nickName;
        std::string _userName;
        std::string _fullName;
        std::string _clientBuffer;
        std::string _mode;
        std::string _unused;
    public:
        ClientState _state;
        LoginStage  _stage;
        Client(int clientSocket);

        void setNickName(std::vector<std::string> &allParams, std::map<int, Client*> &clients);
        void registerUser(std::vector<std::string> &allParams, std::map<int, Client*> &clients);
        std::string &genHostMask(void);


        // Utils:
        bool validateAscii(const std::string &input, const std::string &cmd);
        void sendMsg(int numiCode, std::string nickN, std::string params, std::string fullMsj);
        void displayAllInfo(void);



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
