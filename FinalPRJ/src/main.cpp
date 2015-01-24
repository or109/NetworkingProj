#include <iostream>
#include "CPDSService.h"
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
static string GetLocalIP() {
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
	cout << "=== Server REST API ===" << endl;
	cout << "http://" + ip + "/register?user=USERNAME&password=PASS" << endl;
	cout << "http://" + ip + "/login?user=USERNAME&password=PASS" << endl;
	cout << "http://" + ip + "/getonlineusers" << endl;
	cout << "http://" + ip + "/getuserdetails?user=USERNAME" << endl;
	cout << "http://" + ip + "/logout?user=USERNAME&password=PASS" << endl;
	cout << "http://" + ip + "/webportal.html" << endl;
	cout << "http://" + ip + "/getjson" << endl;
}

int main() {
	CPDSService* CPDS = new CPDSService();

	printInstructions(GetLocalIP());

	while (true) {
		string msg;
		cin >> msg;

		if (msg == "x") {
			break;
		}
	}
	CPDS->~CPDSService();

	return 0;
}
