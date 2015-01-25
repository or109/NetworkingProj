#ifndef USER_H_
#define USER_H_

#include <iostream>
#include <string>

using namespace std;

class User {
public:
	string username;
	string password;
	string ip;
	int port;
	bool loggedin;

	User(string name, string password);
	void login(string ip, int port);
	void logout();
	virtual ~User();
};

#endif
