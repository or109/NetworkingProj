#include "TCPSocket.h"

#include <iostream>
#include <string.h>
#include <strings.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/types.h>

#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>


using namespace std;

const std::string currentDateTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

TCPSocket::TCPSocket(int connected_sock, struct sockaddr_in serverAddr,
		struct sockaddr_in peerAddr) {
	cout << "New socket created" << endl;
	this->sock = connected_sock;
	this->serv_name = serverAddr;
	this->peer_addr = peerAddr;
}

TCPSocket::TCPSocket(int port) {
	// Open TCP socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

	// re-use already bound address/port (if possible)
	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
		printf("Cannot set SO_REUSEADDR option "
			   "on listen socket (%s)\n", strerror(errno));
	// set TCP_NODELAY for sure
	optval = 1;
	if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int)) < 0)
		printf("Cannot set TCP_NODELAY option "
			   "on listen socket (%s)\n", strerror(errno));

	if (sock < 0) {
		perror("Error opening channel");
		cclose();
	}

	// Set the server address for binding
	bzero(&serv_name, sizeof(serv_name));
	serv_name.sin_family = AF_INET;
	serv_name.sin_port = htons(port);

	// Bind the socket to the address
	if (bind(sock, (struct sockaddr *) &serv_name, sizeof(serv_name)) < 0) {
		perror("Error naming channel");
		cerr << "Server is down" << endl;
		cclose();
	} else {
		this->isOpen = true;
		cout << currentDateTime() + " - Server is up\n";
	}
}

TCPSocket::TCPSocket(string peerIp, int port) {    //for client
	//TODO: open TCP socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Error opening channel");
		cclose();
	}

	// Set the peer address to connect to
	bzero(&peer_addr, sizeof(peer_addr));
	peer_addr.sin_family = (short) AF_INET;
	peer_addr.sin_addr.s_addr = inet_addr(peerIp.data());
	peer_addr.sin_port = htons(port);
	this->isOpen = true;
	// Connect the socket to the peer server

	if (connect(sock, (struct sockaddr *) &peer_addr, sizeof(peer_addr)) < 0) {
		perror("Error establishing communications");
		cclose();
		this->isOpen = false;
	}
}

TCPSocket* TCPSocket::listenAndAccept() {
	//TODO: listen on the socket
	listen(sock, 1);

	//TODO: call accept on the socket
	socklen_t len = sizeof(peer_addr);
	bzero(&peer_addr, sizeof(peer_addr));

	int connect_sock = accept(sock, (struct sockaddr *) &peer_addr, &len);

	if (connect_sock < 0) {
		return NULL;
	}
	cout << "conneted socket fd: " << connect_sock << endl;
	//TODO: return new TCPSocket object holding the new secondary server socket returned from the accept call
	return new TCPSocket(connect_sock, serv_name, peer_addr);
}

int TCPSocket::recv(char* buffer, int length) {
	//TODO: read from the socket

	bzero(buffer, 0);
	int byteRead = read(sock, buffer, length);

	return byteRead;
}

int TCPSocket::send(const char* msg, int len) {
	//TODO: write on the socket
	return write(sock, msg, len);
}

void TCPSocket::cclose() {
	shutdown(sock, SHUT_RDWR);
	close(sock);
	this->isOpen = false;
}

string TCPSocket::fromAddr() {
	char *addr = inet_ntoa(peer_addr.sin_addr);
	return addr;
}

string TCPSocket::fromPort() {
	int port = ntohs(peer_addr.sin_port);
	char sport[10];
	sprintf(sport, "%d", port);

	return sport;
}

int TCPSocket::getFileDescriptor() {
	return this->sock;
}

TCPSocket::~TCPSocket() {
	int serverfd;
	serverfd = socket(AF_INET, SOCK_STREAM, 0);

	// Free sockaddr_in serv_name
	memset(&this->serv_name, 0, sizeof(this->serv_name));
	this->serv_name.sin_family = AF_INET;
	this->serv_name.sin_addr.s_addr = INADDR_ANY;
	bind(serverfd, (sockaddr *) &this->serv_name, sizeof(this->serv_name));

	// Free sockaddr_in peer_addr
	memset(&this->peer_addr, 0, sizeof(this->peer_addr));
	this->peer_addr.sin_family = AF_INET;
	this->peer_addr.sin_addr.s_addr = INADDR_ANY;
	bind(serverfd, (sockaddr *) &this->peer_addr, sizeof(this->peer_addr));

	cout << "Bye Bye TCPSocket" << endl;
}
