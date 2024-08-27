#include "Node.h"
#include <iostream>
#include <thread>
#include <sstream>

void CLIUi() {
    std::cout << "Commands:\n";
    std::cout << "  join <network_address>\n";
    std::cout << "  leave\n";
    std::cout << "  upload <filename>\n";
    std::cout << "  download <filename>\n";
    std::cout << "  list\n";
    std::cout << "  exit\n";
}

int main() {
    Node node("127.0.0.1", 8080);
    std::thread serverThread(&Node::startServer, &node);

    std::string command;
    CLIUi();
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "join") {
            std::string networkAddress;
            iss >> networkAddress;
            node.joinNetwork(networkAddress);
        }
        else if (cmd == "leave") {
            node.leaveNetwork();
        }
        else if (cmd == "upload") {
            std::string filename;
            iss >> filename;
            node.uploadFile(filename);
        }
        else if (cmd == "download") {
            std::string filename;
            iss >> filename;
            node.downloadFile(filename);
        }
        else if (cmd == "list") {
            node.listFiles();
        }
        else if (cmd == "exit") {
            node.leaveNetwork();
            break;
        }
        else {
            CLIUi();
        }
    }

    serverThread.join();
    return 0;
}