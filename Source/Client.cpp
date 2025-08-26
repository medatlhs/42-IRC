#include "../Includes/Client.hpp"
#include "../Includes/IrcServer.hpp"
#include <arpa/inet.h>

Client::Client(int clientSocket, const std::string& servname)
    : _clientSocket(clientSocket), _servname(servname),
      _state(ANNONYMOUS), _stage(NOTHING_SET), _dataWaiting(false) { }

void Client::setNickName(const std::string &nick) {
    _nickName = nick;
}

void Client::setUserName(const std::string &user) {
    _userName = user;
}

void Client::setFullName(const std::string &name) {
    _fullName = name;
}

void Client::setLoginStage(LoginStage stage) {
    _stage = stage;
}

void Client::setClientState(ClientState state) {
    _state = state;
}

void Client::setRecvBuffer(const std::string newData) {
    _recvBuffer += newData;
}

void Client::setQueueBuffer(const std::string newData) {
    _queueBuffer += newData;
}

void Client::setHost(sockaddr_in clientAddr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, INET_ADDRSTRLEN);
    _host = std::string(ip);
}
// Fucking Getters 
std::string Client::genHostMask() {
    return _nickName + "!" + _userName + "@" + _host;
}

int Client::getSockfd() {
    return _clientSocket;
}

std::string &Client::getNickName() {
    return _nickName;
}

std::string &Client::getUserName() {
    return _userName;
}

std::string &Client::getRealName() {
    return _fullName;
}

std::string &Client::getFullName() {
    return _fullName;
}

LoginStage &Client::getLoginStage() {
    return _stage;
}

ClientState &Client::getClientState() {
    return _state;
}

std::string &Client::getRecvBuffer() {
    return _recvBuffer;
}

std::string &Client::getQueueBuffer() {
    return _queueBuffer;
}

void Client::addChannelMembership(Channel *channel) {
    if (!isChannelMember(channel))
        _joinedChans.push_back(channel);
}

void Client::removeChannelMembership(Channel *channel) {
    std::vector<Channel *>::iterator it;
    for (it = _joinedChans.begin(); it != _joinedChans.end(); ++it) {
        if (*it == channel) {
            _joinedChans.erase(it);
            break;
        }
    }
}

bool Client::isChannelMember(Channel *channel) {
    std::vector<Channel *>::iterator it;
    for (it = _joinedChans.begin(); it != _joinedChans.end(); ++it) {
        if (*it == channel)
            return true;
    }
    return false;
}

std::vector<Channel *> &Client::getJoindChans() {
    return _joinedChans;
}

