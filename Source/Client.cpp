#include "../Includes/Client.hpp"

Client::Client(int clientSocket) : _clientSocket(clientSocket) { }

void Client::setBuffer(const std::string &newData) {
    this->_clientBuffer.append(newData);
}

std::string &Client::getBuffer(void) {
    return _clientBuffer;
}