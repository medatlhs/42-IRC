#pragma once
#include <string>

enum ClientState {ANNONYMOUS, REGISTERED, DISCONNECTED};

class Client {
    private:
        int         _clientSocket;
        std::string _nickName;
        std::string _userName;
        std::string _realName;
    public:
        ClientState _state;
        std::string _clientBuffer;

        Client(int clientSocket);
        
        //setters and lfucking getters
        void setNickName(const std::string &nickName);
        void setUserName(const std::string &userName);
        void setRealName(const std::string &realName);
        std::string &getNickName(void);
        std::string &getUserName(void);
        std::string &getRealName(void);
};
