#include "./Includes/IrcServer.hpp"

int main(void) {

    IrcServer myIrcServer;

    myIrcServer.setupServer();
    myIrcServer.startAccepting();
    return 0;
}

