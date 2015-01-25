#include "User.h"

const std::string currentDateTime1() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

User::User(string name, string password) {
	this->username = name;
	this->password = password;
	this->loggedin = false;
	this->ip = "";
	this->port = 0;
	this->source = "";
}

void User::login(string ip, int port, string socket) {
	this->ip = ip;
	this->port = port;
	this->loggedin = true;
	this->source = socket;
	this->lastLoginTime = currentDateTime1();
	cout << "[" << this->lastLoginTime << "] new user on port: " << this->port;
}

void User::logout() {
	this->ip = "";
	this->port = 0;
	this->loggedin = false;
	this->source = "";
}

User::~User() {
	// TODO Auto-generated destructor stub
}

