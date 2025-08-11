#include <sys/socket.h>
#include <netinet/in.h> // for sockaddress_in
#include <iostream>
#include <unistd.h>
#define PORT 9090

int main(int ac, char **argv) {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0)
        return std::cerr << "Error: Building a socket!", 1;
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSock, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        return std::cerr << "Error: Binding a socket!", 1;
    
    if (listen(serverSock, 5) < 0)
        return std::cerr << "Error: Listeing!", 1;
    std::cout << "Server is lsitening on port: " << PORT << std::endl;

    int clientSocke = accept(serverSock, nullptr, nullptr);
    if (clientSocke < 0)
        return std::cerr << "Error: Accepting a connection!", 1;
    std::cout << "Detected a new connection\n";

    char buffer[1024] = {0};
    ssize_t recvBytes = recv(clientSocke, &buffer, sizeof(buffer), 0);
    if (recvBytes < 0)
        std::cerr << "Error: Receiving data!" << std::endl;

    
    if (send(clientSocke, buffer, sizeof(buffer), 0) == -1)
        std::cerr << "Error: Sending data!" << std::endl;
    
    std::string bigData(300 * 1024 * 1024, 'A');
    std::cout << "Sendind data ... \n";
    ssize_t bytesSent = send(clientSocke, bigData.data(), bigData.size(), 0);
    if (bytesSent == -1)
        std::cout << "Error: sending bigdata\n";
    std::cout << "Bytes Sent - " << bytesSent << std::endl;
    
    close(clientSocke);
    close(serverSock);
    return 0;
}
