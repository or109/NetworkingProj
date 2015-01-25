#include <iostream>
#include "CPDSServer.h"
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
#include <sstream>

using namespace std;

const std::string GetLocalIP() {
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

void printInstructions(string ip) {
	stringstream str;
	str << LISTENING_PORT;
	string portStr = str.str();

	cout << "\n=== Server REST API ===" << endl;
	cout << "http://" + ip + ":" + portStr + "/register?user=USERNAME&password=PASS" << endl;
	cout << "http://" + ip + ":" + portStr + "/login?user=USERNAME&password=PASS" << endl;
	cout << "http://" + ip + ":" + portStr + "/onlineusers" << endl;
	cout << "http://" + ip + ":" + portStr + "/userdetails?user=USERNAME" << endl;
	cout << "http://" + ip + ":" + portStr + "/logout?user=USERNAME&password=PASS" << endl;
	cout << "http://" + ip + ":" + portStr + "/webportal.html" << endl;
	cout << "http://" + ip + ":" + portStr + "/getjson" << endl;
}

int main() {
	CPDSSeever* CPDS = new CPDSSeever();

	printInstructions(GetLocalIP());

	while (true) {
		string msg;
		cin >> msg;

		if (msg == "x") {
			break;
		}
	}
	CPDS->~CPDSSeever();

	return 0;
}
