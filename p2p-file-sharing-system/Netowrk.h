#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class Network
{
public:
	Network();
	~Network();
	SOCKET createSocket();
	bool bindSocket(SOCKET socket, int port);
	bool listenOnSocket(SOCKET socket, int backlog = 5);
	SOCKET acceptOnSocket(SOCKET socket);
	bool ConnectingToServer(SOCKET socket, const std::string& address, int port);
	bool SendData(SOCKET socket, const std::string& data);
	std::string ReceiveData(SOCKET socket);

private:
	WSADATA wsaData;
};

#endif // NETWORK_H