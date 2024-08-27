#include <iostream>
#include "Netowrk.h"
#include <thread>

void server() {
	Network network;

	SOCKET serverSocket = network.createSocket();
	if (!network.bindSocket(serverSocket, 8080)) {
		std::cerr << "Failed to bind socket." << std::endl;
		return;
	}

	if (!network.listenOnSocket(serverSocket)) {
		std::cerr << "Failed to listen on socket." << std::endl;
		return;
	}

	std::cout << "Server is listening on port 8080." << std::endl;

	SOCKET clientSocket = network.acceptOnSocket(serverSocket);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Failed to accept client." << std::endl;
		return;
	}

	std::string message = network.ReceiveData(clientSocket);
	std::cout << "Received: " << message << std::endl;

	network.SendData(clientSocket, "Hello from server!");
	closesocket(clientSocket);
	closesocket(serverSocket);
}

void client() {
	Network network;
	SOCKET clientSocket = network.createSocket();
	if (!network.ConnectingToServer(clientSocket, "127.0.0.1", 8080)) {
		std::cerr << "Failed to connect to server." << std::endl;
		return;
	}

	network.SendData(clientSocket, "Hello from client!");
	std::string response = network.ReceiveData(clientSocket);
	std::cout << "Received: " << response << std::endl;

	closesocket(clientSocket);
}


int main() {
	std::thread serverThread(server);
	// giving time to the server to start
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::thread clientThread(client);

	serverThread.join();
	clientThread.join();

	return 0;
}
