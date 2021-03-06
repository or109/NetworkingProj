#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include "TCPSocket.h"
using namespace std;

TCPSocket::TCPSocket(int connected_sock, struct sockaddr_in serverAddr,
		struct sockaddr_in peerAddr) {
	cout << "Created new socket" << endl;
	this->sock = connected_sock;
	this->serv_name = serverAddr;
	this->peer_addr = peerAddr;
}

// For server
TCPSocket::TCPSocket(int port) {
	// Open TCP socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
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
		cclose();
	} else {
		printf("Server is up.\n");
	}
}

// For client
TCPSocket::TCPSocket(string peerIp, int port) {
	//TODO: open TCP socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Error opening channel");
		cclose();
	}

	//TODO: set the peer address to connect to
	bzero(&peer_addr, sizeof(peer_addr));
	peer_addr.sin_family = (short) AF_INET;
	peer_addr.sin_addr.s_addr = inet_addr(peerIp.data());
	peer_addr.sin_port = htons(port);
	//TODO: connect the socket to the peer server

	if (connect(sock, (struct sockaddr *) &peer_addr, sizeof(peer_addr)) < 0) {
		perror("Error establishing communications");
		cclose();
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
	cout << "Connected socket fd: " << connect_sock << endl;

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
