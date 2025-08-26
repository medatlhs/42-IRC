# include "../Includes/replyCodes.hpp"
# include "../Includes/IrcServer.hpp"
# include "../Includes/Channel.hpp"
# include "../Includes/Client.hpp"

std::vector<std::string> IrcServer::seperator(std::string &str, char c) {
    std::stringstream ss(str);
    std::vector<std::string> container;
    std::string element;
    while (std::getline(ss, element, c))
        container.push_back(element);
    return container;
}

bool IrcServer::validateAscii(std::string &input, std::string &cmd) {
    (void)cmd;
    std::string notAllowed = "#@:,!&?.";
    for (size_t i = 0; i < input.length(); i++)
        if (notAllowed.find(input[i]) != std::string::npos)
            return false ;
    return true ;
}

Client *IrcServer::getClientByfd(int clientSock) {
    std::map<int, Client *>::iterator it;
    for (it = _clientsBySock.begin(); it != _clientsBySock.end(); it++)
        if (it->first == clientSock)
            return it->second;
    return nullptr;
}

Client *IrcServer::getClientByNick(const std::string&nick) {
    std::cout << "nick: [" << nick << "]" << std::endl;
    try {
        Client *client = _clientsByNick.at(nick);
        return client;
    } catch(const std::exception &e) {
        std::cout << "clientbynick\n";
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}

Channel *IrcServer::getChannel(const std::string &channelname) {
    try {
        Channel *channel = _allChannels.at(channelname);
        return channel;
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}
