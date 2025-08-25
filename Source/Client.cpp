#include "../Includes/Client.hpp"
#include "../Includes/IrcServer.hpp"

Client::Client(int clientSocket, const std::string&servname) 
    : _clientSocket(clientSocket), _nickName("guest"), _userName("user"), _servname(servname) { }

int Client::getSockfd() { return _clientSocket; }
ClientState &Client::getClientState(void) { return _state; }
LoginStage  &Client::getLoginStage(void) { return _stage; }
std::string &Client::getNickName(void) { return _nickName; }
std::string &Client::getUserName(void) { return _userName; }
std::string &Client::getFullName(void) { return _fullName; }
std::string &Client::getRecvBuffer(void) { return _recvBuffer; }
std::string &Client::getQueueBuffer(void) { return _queueBuffer; }
std::string Client::genHostMask() { return  _nickName + "!" + _userName + "@" + _host; }

void Client::setQueueBuffer(const std::string newData) {
    if (!_dataWaiting) { _queueBuffer = newData; }
    else _queueBuffer.append(newData); 
    _dataWaiting = true;
}
void Client::setRecvBuffer(const std::string newData) {
    _queueBuffer.clear(); 
    _recvBuffer = newData; 
}
void Client::setLoginStage(LoginStage stage) { this->_stage = stage; }
void Client::setClientState(ClientState state) { this->_state = state; }
void Client::setNickName(const std::string &nick) { this->_nickName = nick; }
void Client::setUserName(const std::string &user) { this->_userName = user; }
void Client::setFullName(const std::string &fullname) { this->_fullName = fullname; }

void Client::setHost(sockaddr_in clientAddr) {
    std::string ip = inet_ntoa(clientAddr.sin_addr);
    if (ip.empty()) ip = "0.0.0.0"; // temp fix
    this->_host = ip;
}

void Client::addToJoindChannels(const std::string &channelname) {
    _joinedChans.push_back(channelname);
}

void Client::remvfromJoindChannels(const std::string &channelname) {
    _joinedChans.erase(channelname);
}

void Client::isMemberOfChannel(const std::string &channelname) {
    std::vector<std::>:string:iterator it;
    it  = std::find(_joinedChans.being(), _joinedChans.end(), channelname);
    if (it != _joinedChans.end())
        return true;
    return false;
}