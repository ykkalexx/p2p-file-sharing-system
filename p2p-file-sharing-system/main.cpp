#include "Node.h"
#include <iostream>
#include <thread>
#include <sstream>
#include <filesystem>
#include <vector>
#include <algorithm>

void CLIUi() {
    std::cout << "Commands:\n";
    std::cout << "  join <network_address>\n";
    std::cout << "  leave\n";
    std::cout << "  upload <filename>\n";
    std::cout << "  download <filename>\n";
    std::cout << "  list\n";
    std::cout << "  search <keyword>\n";
    std::cout << "  help\n";
    std::cout << "  clear\n";
    std::cout << "  exit\n";
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
            if (networkAddress.empty()) {
                std::cerr << "Error: Network address is required.\n";
            }
            else {
                node.joinNetwork(networkAddress);
            }
        }
        else if (cmd == "leave") {
            node.leaveNetwork();
        }
        else if (cmd == "upload") {
            std::string filename;
            iss >> filename;
            if (filename.empty()) {
                std::cerr << "Error: Filename is required.\n";
            }
            else if (!std::filesystem::exists(filename)) {
                std::cerr << "Error: File does not exist.\n";
            }
            else {
                node.uploadFile(filename);
            }
        }
        else if (cmd == "download") {
            std::string filename;
            iss >> filename;
            if (filename.empty()) {
                std::cerr << "Error: Filename is required.\n";
            }
            else {
                node.downloadFile(filename);
            }
        }
        else if (cmd == "list") {
            node.listFiles();
        }
        else if (cmd == "search") {
            std::string keyword;
            iss >> keyword;
            if (keyword.empty()) {
                std::cerr << "Error: Keyword is required.\n";
            }
            else {
                node.searchFile(keyword);
            }
        }
        else if (cmd == "help") {
            CLIUi();
        }
        else if (cmd == "clear") {
            clearScreen();
        }
        else if (cmd == "exit") {
            node.leaveNetwork();
            break;
        }
        else {
            std::cerr << "Error: Unknown command.\n";
            CLIUi();
        }
    }

    serverThread.join();
    return 0;
}