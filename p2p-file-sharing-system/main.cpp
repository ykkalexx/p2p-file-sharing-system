#include "Node.h"
#include <iostream>
#include <thread>

void server() {
    Node node("127.0.0.1", 8080);
    std::thread serverThread(&Node::startServer, &node);
    node.joinNetwork("127.0.0.1");
    node.uploadFile("example.txt");
    std::this_thread::sleep_for(std::chrono::seconds(5)); 
    node.leaveNetwork();
    serverThread.join();
}

void client() {
    Node node("127.0.0.1", 8081);
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
    node.joinNetwork("127.0.0.1");
    node.downloadFile("example.txt");
    node.leaveNetwork();
}

int main() {
    std::thread server_thread(server);
    std::thread client_thread(client);

    server_thread.join();
    client_thread.join();

    return 0;
}