#include "Netowrk.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

Network::Network() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		exit(1);
	}
}

Network::~Network() {
	WSACleanup();
}

SOCKET Network::createSocket() {
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

bool Network::bindSocket(SOCKET socket, int port) {
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	return bind(socket, (struct sockaddr*)&address, sizeof(address)) != SOCKET_ERROR;
}

bool Network::listenOnSocket(SOCKET socket, int backlog) {
	return listen(socket, backlog) != SOCKET_ERROR;
}

SOCKET Network::acceptOnSocket(SOCKET socket) {
	sockaddr_in address;
	int addrlen = sizeof(address);

	return accept(socket, (struct sockaddr*)&address, &addrlen);
}

bool Network::ConnectingToServer(SOCKET socket, const std::string& address, int port) {
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	inet_pton(AF_INET, address.c_str(), &server_address.sin_addr);

	return connect(socket, (struct sockaddr*)&server_address, sizeof(server_address)) != SOCKET_ERROR;
}

bool Network::SendData(SOCKET socket, const std::string& data) {
	return send(socket, data.c_str(), data.size(), 0) != SOCKET_ERROR;
}

std::string Network::ReceiveData(SOCKET socket) {
	char buffer[1024] = { 0 };
	int valread = recv(socket, buffer, 1024, 0);

	return std::string(buffer, valread);
}