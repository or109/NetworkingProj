#include "User.h"

User::User(string name, string password) {
	this->userName = name;
	this->userPassword = password;
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

