#include "../Includes/Client.hpp"

Client::Client(int clientSocket) : _clientSocket(clientSocket), _nickName("guest"), _userName("user") { }

std::string &Client::getNickName(void) { return _nickName; }
std::string &Client::getUserName(void) { return _userName; }
std::string &Client::getRealName(void) { return _fullName; }
std::string &Client::getBuffer(void) { return _clientBuffer; }

// void Client::setNickName(const std::string&NickName) { this->_nickName = NickName; }
// void Client::setUserName(const std::string&userName) { this->_userName = userName; }
// void Client::setRealName(const std::string&realName) { this->_realName = realName; }
void Client::setBuffer(std::string newData) { clearBuffer(); this->_clientBuffer = newData; }

void Client::clearBuffer() { this->_clientBuffer.clear(); }

//:<server_hostname> <numeric_code> <recipient_nickname> <parameters> :<human_readable_message>
void Client::sendMsg(int numiCode, std::string nickN, std::string params, std::string fullMsj) {
    std::string message = ":thc-server.com ";
    message.append(std::to_string(numiCode)).append(" ").append(nickN).append(" ");
    message.append(params).append(" :").append(fullMsj);
    ssize_t bytesSent = send(this->_clientSocket, message.c_str(), message.length(), 0);
    if (bytesSent < 0)
        perror("send failed");
}

bool Client::validateAscii(const std::string &input, const std::string &cmd) {
    (void)cmd;
    std::string notAllowed = "#@:,!&?.";
    for (size_t i = 0; i < input.length(); i++) {
        if (notAllowed.find(input[i]) != std::string::npos)
            return false ;
    }
    return true;
}

void Client::setNickName(std::vector<std::string>&allParams, std::map<int, Client*>&clients) {
    std::string cmd = "NICK";
    if (allParams.size() != 1)
        return sendMsg(ERR_NEEDMOREPARAMS, _nickName, cmd, msg_param);
    std::string nickName = allParams.at(0);
    if (nickName.length() > 10) {
        return sendMsg(ERR_ERRONEUSNICKNAME, _nickName, cmd, msg_too_long);
    } else if (isdigit(nickName[0])) {
        return sendMsg(ERR_ERRONEUSNICKNAME, _nickName, cmd, msg_digit_start);
    } else if (!validateAscii(nickName, cmd))
        return sendMsg(ERR_ERRONEUSNICKNAME, _nickName, cmd, msg_inva_char);
    std::map<int, Client*>::iterator it = clients.begin();
    while (it != clients.end()) {
        if (it->second->getNickName() == nickName)
            return sendMsg(ERR_ALREADYREGISTRED, _nickName, cmd, msg_taken);
        it++;
    }
    this->_nickName = nickName;
    if (this->_stage == USER_SET) { this->_state = REGISTERED; }
    else if (this->_stage == NOTHING_SET) { this->_stage =  NICK_SET; }

}

void Client::registerUser(std::vector<std::string>&allParams, std::map<int, Client*>&clients) {
    (void)clients;
    std::string cmd = "USER";
    if (this->_state == REGISTERED)
        return sendMsg(ERR_ALREADYREGISTRED, _nickName, cmd, msg_already_reg);
    if (allParams.size() < 4)
        return sendMsg(ERR_NEEDMOREPARAMS, _nickName, cmd, msg_param);
    std::string userName = allParams.at(0).substr(0, 10);
    if (!validateAscii(userName, cmd))
        return sendMsg(ERR_NEEDMOREPARAMS, _nickName, cmd, msg_inva_char);

    bool seenColon = false;
    for (size_t i = 0; i < allParams.size(); i++) {
        std::string singleParam = allParams[i];
        if (singleParam.find(':') != std::string::npos) { // igh gis tla ira aykchm
            if ( i != 3 || (i == 3 && singleParam.at(0) != ':'))
                return sendMsg(ERR_NEEDMOREPARAMS, _nickName, cmd, "colon issue I\n");
            seenColon = true;
        }
    }
    if (!seenColon) return sendMsg(ERR_NEEDMOREPARAMS, _nickName, cmd, "colon issue II\n");

    std::string fullName;
    for (size_t i = 3; i < allParams.size(); i++) {
        fullName.append(allParams.at(i));
        if (i+1 != allParams.size()) { fullName.append(" "); }
    }
    if (std::count(fullName.begin(), fullName.end(), ':') > 1)
        return sendMsg(ERR_NEEDMOREPARAMS, _nickName, cmd, "colon issue III\n");

    this->_userName = allParams.at(0);
    this->_fullName = fullName.erase(0, 1);
    if (this->_stage == NICK_SET) {
        this->_state = REGISTERED;
        // sendMsg();
    } else if (this->_stage == NOTHING_SET)
        this->_stage = USER_SET;
    std::cout << "User just got registered\n";
}




void Client::displayAllInfo(void) {
    std::cout << "---------------------------------------\n";
    std::cout << "client state    : " << _state << std::endl;
    std::cout << "client stage    : " << _stage << std::endl;
    std::cout << "client nick name: [" << _nickName << "]" << std::endl;
    std::cout << "client user name: [" << _userName << "]" << std::endl;
    std::cout << "client full name: [" << _fullName << "]" << std::endl;
}