#ifndef NODE_H
#define NODE_H

#include <string>
#include "DHT.h"
#include "Netowrk.h"
#include <openssl/sha.h>

class Node
{
private:
    DHT dht;
    Network network;
    std::string address;
    int port;

    std::string computeFileHash(const std::string& filename);

public:
    Node(const std::string& address, int port);
    void startServer();
    void handleClient(SOCKET clientSocket);
    void joinNetwork(const std::string& networkAddress);
    void leaveNetwork();
    void handleFailure(const std::string& failedNodeAddress);
    void uploadFile(const std::string& filename);
    void downloadFile(const std::string& filename);
    void listFiles();
    void searchFile(const std::string& keyword);
    void handleSearchRequest(SOCKET clientSocket, const std::string& keyword);
    void handleSearchResponse(const std::string& filename, const std::string& nodeAddress);
};

#endif // NODE_H