#include "./Includes/IrcServer.hpp"

int main(int ac, char **av) {
    if (ac != 2) 
        return 1;
    std::stringstream ss(av[1]);
    int port;
    ss >> port;
    IrcServer myIrcServer(port, "123");
    try {
        myIrcServer.setupServer();
        myIrcServer.startAccepting();
    } catch(std::exception &e) {
        return std::cerr << e.what() << std::endl, 1;
    }
    return 0;
}
