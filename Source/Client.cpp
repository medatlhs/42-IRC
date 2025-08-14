#include "../Includes/Client.hpp"

Client::Client(int clientSocket) : _clientSocket(clientSocket), _nickName("guest") { }

std::string &Client::getNickName(void) { return _nickName; }
std::string &Client::getUserName(void) { return _userName; }
std::string &Client::getRealName(void) { return _realName; }
std::string &Client::getBuffer(void) { return _clientBuffer; }

// void Client::setNickName(const std::string&NickName) { this->_nickName = NickName; }
// void Client::setUserName(const std::string&userName) { this->_userName = userName; }
// void Client::setRealName(const std::string&realName) { this->_realName = realName; }
void Client::setBuffer(std::string newData) { clearBuffer(); this->_clientBuffer = newData; }

void Client::clearBuffer() { this->_clientBuffer.clear(); }

//:<server_hostname> <numeric_code> <recipient_nickname> <parameters> :<human_readable_message>
void Client::sendError(int numiCode, std::string nickN, std::string params, std::string fullMsj) {
    std::string message = ":thc-server.com ";
    message.append(std::to_string(numiCode)).append(" ").append(nickN).append(" ");
    message.append(params).append(" :").append(fullMsj);
    std::cout << "mesj >" << message << std::endl;
    ssize_t bytesSent = send(this->_clientSocket, message.c_str(), message.length(), 0);
    if (bytesSent < 0)
        perror("send failed");
}

void Client::setNickName(std::vector<std::string> &allParams, std::map<int, Client*> &clients) {
    std::string cmd = "NICK";
    if (allParams.size() != 1)
        return sendError(ERR_NEEDMOREPARAMS, _nickName, cmd, MSJ_PARAM);
    std::string nickName = allParams.at(0);
    if (nickName.length() > 10) {
            return sendError(ERR_ERRONEUSNICKNAME, _nickName, cmd, MSJ_TOOLONG);
    } else if (isdigit(nickName[0]))
        return sendError(ERR_ERRONEUSNICKNAME, _nickName, cmd, MSJ_DIGITATSTART);
    std::string notAllowed = "#@:,!&?.";
    for (size_t i = 0; i < nickName.length(); i++) {
        if (notAllowed.find(nickName[i]) != std::string::npos)
            return sendError(ERR_ERRONEUSNICKNAME, _nickName, cmd, MSJ_INVALIDCHAR);
    }
    std::map<int, Client*>::iterator it = clients.begin();
    while (it != clients.end()) {
        if (it->second->getNickName() == nickName)
            return sendError(ERR_NICKNAMEINUSE, _nickName, cmd, MSJ_TAKENNICK);
        it++;
    }
    if (this->_state == ClientState::ANNONYMOUS)
        this->_state = ClientState::NICK_SET;
    this->_nickName = nickName;
}
