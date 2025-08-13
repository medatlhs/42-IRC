#pragma once
#include <string>

enum ClientState {ANNONYMOUS, LOGED, REGISTERED, DISCONNECTED};

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
        
        //setters and lfucking getters
        void setNickName(const std::string &nickName);
        void setUserName(const std::string &userName);
        void setRealName(const std::string &realName);
        void setBuffer(const std::string &newData);
        std::string &getNickName(void);
        std::string &getUserName(void);
        std::string &getRealName(void);
        std::string &getBuffer(void);
};
