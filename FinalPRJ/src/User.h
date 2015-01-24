#ifndef USER_H_
#define USER_H_

#include <iostream>
#include <string>

using namespace std ;


class User {
public:
	string userName;
	string userPassword;
	string  ip;
	int     port;
	bool    loggedin;
	string     source;

	User(string name, string password);
	void login(string ip, int port,string source);
	void logout();
	virtual ~User();
};

#endif /* USER_H_ */
