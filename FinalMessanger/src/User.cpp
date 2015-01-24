#include "User.h"

User::User(string name, string password) {
	this->userName = name;
	this->userPassword = password;
	this->loggedin = false;
	this->ip = "";
	this->port = 0;

}
void User::login(string ip, int port){
	this->ip = ip;
	this->port = port;
	this->loggedin = true;


}
void User::logout(){
	this->ip = "";
	this->port = 0;
	this->loggedin = false;

}

User::~User() {
	// TODO Auto-generated destructor stub
}

