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

using namespace std;

// Get local IP from Linux OS
string GetLocalIP() {
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

	// take random port from 8888 to 9999
	port = rand() % 112 + 8888;
	this->rcvClbk = clbk;
	this->usr = new User("", "");

	//create the socket
	soket = new UDPSocket(port);

	//start the thread to receive incoming messages
	this->start();
}

// Read from the socket and print the received messages
void UDPMessenger::run() {
	char buffer[100];

	while (true) {
		memset((void*) buffer, 0, 100);
		soket->recv(buffer, 100);
		// send message to console
		rcvClbk->handleMessage(buffer);
	}
}

void UDPMessenger::sendTo(string msg, string ip, int port) {
	// send the given message to the given destination
	soket->sendTo(msg, ip, port);
}

void UDPMessenger::reply(string msg) {
	// send the message to the address of the last received message
	soket->reply(msg);
}

void UDPMessenger::close() {

	this->logout();

	// Close the thread
	this->~MThread();

	// Close the socket
	soket->cclose();

	// Wait for thread to exit
	this->waitForThread();

	// Delete and free any allocated resources
	delete soket;
	delete rcvClbk;
}

// Register user to server
string UDPMessenger::registerin(string user, string password) {
	HTTPClient* CPDS = new HTTPClient("127.0.0.1/register");
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
	usr->userName = user;
	usr->userPassword = password;

	HTTPClient* CPDS = new HTTPClient("127.0.0.1/login");
	CPDS->setParam("user", user);
	CPDS->setParam("password", password);
	string localIp = GetLocalIP();

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

	HTTPClient* CPDS = new HTTPClient("127.0.0.1/getlogedusers");
	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);

		return body;
	}

	return "ERROR - cannot connect to server";
}

// Get user details from server
string UDPMessenger::getUserDetails(string user) {

	HTTPClient* CPDS = new HTTPClient("127.0.0.1/getuserdetails");
	CPDS->setParam("user", user);

	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);

		return body;
	}

	return "ERROR - cannot connect to server.";
}

// logout user from server
string UDPMessenger::logout() {

	HTTPClient* CPDS = new HTTPClient("127.0.0.1/logout");
	CPDS->setParam("user", usr->userName);
	CPDS->setParam("password", usr->userPassword);

	if (CPDS->sendGetRequest()) {
		string response = CPDS->response();
		string body = response.substr(response.find("\r\n\r\n") + 4);

		return body;
	}

	return "ERROR - cannot connect to server.";
}

