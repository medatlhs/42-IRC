#include "../Includes/Client.hpp"
#include "../Includes/IrcServer.hpp"

Client::Client(int clientSocket, const std::string&servname) 
    : _clientSocket(clientSocket), _nickName("guest"), _userName("user"), _servname(servname) { }

int Client::getSockfd() { return _clientSocket; }
ClientState &Client::getState(void) { return _state; }
LoginStage  &Client::getStage(void) { return _stage; }
std::string &Client::getNickName(void) { return _nickName; }
std::string &Client::getUserName(void) { return _userName; }
std::string &Client::getFullName(void) { return _fullName; }
std::string &Client::getRecvBuffer(void) { return _recvBuffer; }
std::string &Client::getQueueBuffer(void) { return _queueBuffer; }
std::string Client::genHostMask() { return  _nickName + "!" + _userName + "@" + _host; }

void Client::setQueueBuffer(const std::string newData) {  
    _queueBuffer.clear(); 
    _queueBuffer = newData; 
}
void Client::setRecvBuffer(const std::string newData) {
    _queueBuffer.clear(); 
    _recvBuffer = newData; 
}
void Client::setStage(LoginStage stage) { this->_stage = stage; }
void Client::setState(ClientState state) { this->_state = state; }
void Client::setNickName(const std::string &nick) { this->_nickName = nick; }
void Client::setUserName(const std::string &user) { this->_userName = user; }
void Client::setFullName(const std::string &fullname) { this->_fullName = fullname; }

void Client::setHost(sockaddr_in clientAddr) {
    std::string ip = inet_ntoa(clientAddr.sin_addr);
    if (ip.empty())
        ip = "0.0.0.0"; // temp fix
    this->_host = ip;
}

void Client::sendPrivateMsg(std::string &message) {
    setQueueBuffer(message);
    this->_dataWaiting = true;
}