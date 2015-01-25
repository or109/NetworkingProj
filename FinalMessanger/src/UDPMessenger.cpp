#include "UDPMessenger.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#define PORT_MIN_RANGE 8888

using namespace std;

// Get local IP from Linux OS
string GetLocalIP1() {
	int fd;
	struct ifreq ifr;

	char iface[] = "eth0";

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;

	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	return inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr);
}

UDPMessenger::UDPMessenger(OnRecieveClbk* clbk) {
	// Init local arguments (if needed)
	srand(time(NULL));

	// Random port from PORT_MIN_RANGE to PORT_MIN_RANGE+1111
	port = rand() % 112 + PORT_MIN_RANGE;
	this->rcvClbk = clbk;
	this->usr = new User("", "");

	// Start the socket
	udpSocket = new UDPSocket(port);
	this->start();
}

// Read from the socket and print the received messages
void UDPMessenger::run() {
	char buffer[100];

	while (true) {
		memset((void*) buffer, 0, 100);
		udpSocket->recv(buffer, 100);

		// Send message to console
		rcvClbk->handleMessage(buffer);
	}
}

// Send the given message to the given destination
void UDPMessenger::sendTo(string msg, string ip, int port) {
	udpSocket->sendTo(msg, ip, port);
}

// Send the message to the address of the last received message
void UDPMessenger::reply(string msg) {
	udpSocket->reply(msg);
}

void UDPMessenger::close() {
	this->logout();

	// Close the thread
	this->~MThread();

	// Close the socket
	udpSocket->cclose();

	// Wait for thread to exit
	this->waitForThread();

	// Delete and free any allocated resources
	delete udpSocket;
	delete rcvClbk;
}

// Register user to server
string UDPMessenger::registerin(string user, string password) {
	HTTPClient* CPDS = new HTTPClient(IP_ADDR"/register");
	CPDS->setParam("user", user);
	CPDS->setParam("password", password);

	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);

		return body;
	}

	return "ERROR - cannot connect to server";
}

// Login user to server
string UDPMessenger::login(string user, string password) {
	usr->username = user;
	usr->password = password;

	HTTPClient* CPDS = new HTTPClient(IP_ADDR"/login");
	CPDS->setParam("user", user);
	CPDS->setParam("password", password);
	string localIp = GetLocalIP1();
	CPDS->setParam("ip", localIp);

	// Convert int to string
	char sport[10] = "";
	sprintf(sport, "%d", port);
	CPDS->setParam("port", sport);

	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);
		char data[300] = "", code[10] = "";

		sscanf(body.data(), "%s %s", code, data);

		if (strcmp(code, "OK") == 0) {
			usr->login(localIp, port);
		}

		return body;
	}

	return "ERROR - cannot connect to server";
}

// Get logged users from server
string UDPMessenger::getUserList() {

	HTTPClient* CPDS = new HTTPClient(IP_ADDR"/onlineusers");

	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);

		return body;
	}

	return "ERROR - cannot connect to server";
}

// Get user details from server
string UDPMessenger::getUserDetails(string user) {

	HTTPClient* CPDS = new HTTPClient(IP_ADDR"/userdetails");
	CPDS->setParam("user", user);

	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);

		return body;
	}

	return "ERROR - cannot connect to server.";
}

// Logout user
string UDPMessenger::logout() {
	HTTPClient* CPDS = new HTTPClient(IP_ADDR"/logout");
	CPDS->setParam("user", usr->username);
	CPDS->setParam("password", usr->password);

	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);

		return body;
	}

	return "ERROR - cannot connect to server.";
}
