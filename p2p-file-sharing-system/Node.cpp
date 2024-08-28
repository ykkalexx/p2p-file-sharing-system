#include "Node.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>

Node::Node(const std::string& address, int port) : address(address), port(port) {}

void Node::startServer() {
    SOCKET serverSocket = network.createSocket();
    if (!network.bindSocket(serverSocket, port)) {
        std::cerr << "Failed to bind socket on port " << port << std::endl;
        return;
    }

    if (!network.listenOnSocket(serverSocket, 5)) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return;
    }

    std::cout << "Server started on " << address << ":" << port << std::endl;

    while (true) {
        SOCKET clientSocket = network.acceptOnSocket(serverSocket);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        std::thread(&Node::handleClient, this, clientSocket).detach();
    }
}

void Node::handleClient(SOCKET clientSocket) {
    std::string request = network.ReceiveData(clientSocket);
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    if (command == "JOIN") {
        std::string nodeAddress;
        iss >> nodeAddress;
        // Handle join request
        std::cout << "Node " << nodeAddress << " joined the network" << std::endl;
        network.SendData(clientSocket, "WELCOME " + nodeAddress);

        // Send current DHT entries to the new node
        for (const auto& entry : dht.getAllEntries()) {
            network.SendData(clientSocket, "UPLOAD " + entry.first + " " + entry.second);
        }
    }
    else if (command == "LEAVE") {
        std::string nodeAddress;
        iss >> nodeAddress;
        // Handle leave request
        std::cout << "Node " << nodeAddress << " left the network" << std::endl;
    }
    else if (command == "UPLOAD") {
        std::string filename, nodeAddress;
        iss >> filename >> nodeAddress;
        dht.addFile(filename, nodeAddress);
        std::cout << "File " << filename << " uploaded by node " << nodeAddress << std::endl;
    }
    else if (command == "REQUEST") {
        std::string filename;
        iss >> filename;
        std::ifstream inFile(filename, std::ios::binary);
        std::ostringstream oss;
        oss << inFile.rdbuf();
        std::string fileData = oss.str();
        network.SendData(clientSocket, fileData);
        std::cout << "File " << filename << " sent to client" << std::endl;
    }
    else if (command == "SEARCH") {
        std::string keyword;
        iss >> keyword;
        std::cout << "Search request for keyword: " << keyword << std::endl;
        handleSearchRequest(clientSocket, keyword);
    }
    else if (command == "SEARCH_RESULT") {
        std::string filename, nodeAddress;
        iss >> filename >> nodeAddress;
        std::cout << "Search result received: " << filename << " from node " << nodeAddress << std::endl;
        handleSearchResponse(filename, nodeAddress);
	}
	else if (command == "SEARCH_RESULT_NOT_FOUND") {
		std::cout << "Search result not found" << std::endl;
	}

    closesocket(clientSocket);
}

void Node::joinNetwork(const std::string& networkAddress) {
    SOCKET clientSocket = network.createSocket();
    if (!network.ConnectingToServer(clientSocket, networkAddress, 8080)) {
        std::cerr << "Failed to connect to server." << std::endl;
        return;
    }

    network.SendData(clientSocket, "JOIN " + address + ":" + std::to_string(port));
    std::string response = network.ReceiveData(clientSocket);
    std::cout << "Received: " << response << std::endl;

    // Receive DHT entries from the server
    while (true) {
        std::string entry = network.ReceiveData(clientSocket);
        if (entry.empty()) break;
        std::istringstream iss(entry);
        std::string command, filename, nodeAddress;
        iss >> command >> filename >> nodeAddress;
        if (command == "UPLOAD") {
            dht.addFile(filename, nodeAddress);
        }
    }

    closesocket(clientSocket);
}

void Node::leaveNetwork() {
    std::vector<std::pair<std::string, std::string>> knownNodes = dht.getAllEntries();
    for (const auto& nodeEntry : knownNodes) {
        const std::string& nodeAddress = nodeEntry.second;

        SOCKET socket = network.createSocket();
        if (network.ConnectingToServer(socket, nodeAddress, 8080)) {
            network.SendData(socket, "LEAVE " + address);
            closesocket(socket);
        }
    }

    std::cout << "Node left the network." << std::endl;
}

void Node::handleFailure(const std::string& failedNodeAddress) {
    for (const auto& entry : dht.getAllEntries()) {
        if (entry.second == failedNodeAddress) {
            dht.removeFile(entry.first);
        }
    }

    std::cout << "Node with address " << failedNodeAddress << " has failed." << std::endl;
}

void Node::uploadFile(const std::string& filename) {
    // Check if the file exists
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "File " << filename << " does not exist." << std::endl;
        return;
    }

    // Add file to DHT
    dht.addFile(filename, address);
    std::cout << "File " << filename << " uploaded to DHT" << std::endl;

    // Notify other nodes about the new file
    std::vector<std::pair<std::string, std::string>> knownNodes = dht.getAllEntries();
    for (const auto& nodeEntry : knownNodes) {
        const std::string& nodeAddress = nodeEntry.second;

        // Create a socket and attempt to connect to the node
        SOCKET socket = network.createSocket();
        if (network.ConnectingToServer(socket, nodeAddress, 8080)) {
            // Send the upload command with the filename and address
            network.SendData(socket, "UPLOAD " + filename + " " + address);
            closesocket(socket);
        }
    }
}

void Node::downloadFile(const std::string& filename) {
    // Locate file in DHT
    std::string nodeAddress = dht.lookupFile(filename);
    if (nodeAddress.empty()) {
        std::cerr << "File " << filename << " not found in DHT" << std::endl;
        return;
    }
    std::cout << "File " << filename << " found at node " << nodeAddress << std::endl;

    // Connect to the node and request the file
    SOCKET socket = network.createSocket();
    if (!network.ConnectingToServer(socket, nodeAddress, 8080)) {
        std::cerr << "Failed to connect to node at " << nodeAddress << std::endl;
        return;
    }

    // Send a request for the file
    network.SendData(socket, "REQUEST " + filename);

    // Receive the file data
    std::string fileData = network.ReceiveData(socket);
    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(fileData.c_str(), fileData.size());
    outFile.close();

    std::cout << "File " << filename << " downloaded from node " << nodeAddress << std::endl;
    closesocket(socket);
}

void Node::listFiles() {
    std::vector<std::pair<std::string, std::string>> files = dht.getAllEntries();
    if (files.empty()) {
        std::cout << "No files found in the network." << std::endl;
    }
    else {
        std::cout << "Files in the network:" << std::endl;
        for (const auto& file : files) {
            std::cout << "  " << file.first << " (stored at " << file.second << ")" << std::endl;
        }
    }
}

void Node::searchFile(const std::string& keyword) {
    std::cout << "Initiating search for keyword: " << keyword << std::endl;
    std::vector<std::pair<std::string, std::string>> knownNodes = dht.getAllEntries();
    bool fileFound = false;

    for (const auto& nodeEntry : knownNodes) {
        const std::string& nodeAddress = nodeEntry.second;

        SOCKET socket = network.createSocket();
        if (network.ConnectingToServer(socket, nodeAddress, 8080)) {
            network.SendData(socket, "SEARCH " + keyword);
            closesocket(socket);
        }
        else {
            std::cerr << "Failed to connect to node: " << nodeAddress << std::endl;
        }
    }

    if (!fileFound) {
        std::cout << "File not found for keyword: " << keyword << std::endl;
    }
}

void Node::handleSearchRequest(SOCKET clientSocket, const std::string& keyword) {
    std::vector<std::pair<std::string, std::string>> files = dht.getAllEntries();
    bool fileFound = false;

    for (const auto& file : files) {
        if (file.first.find(keyword) != std::string::npos) {
            fileFound = true;
            network.SendData(clientSocket, "SEARCH_RESULT " + file.first + " " + address);
        }
    }

    if (!fileFound) {
        network.SendData(clientSocket, "SEARCH_RESULT_NOT_FOUND");
    }
}

void Node::handleSearchResponse(const std::string& filename, const std::string& nodeAddress) {
    std::cout << "File found: " << filename << " at node " << nodeAddress << std::endl;
}