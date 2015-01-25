#ifndef CPDSSERVICE_H_
#define CPDSSERVICE_H_
#define LISTENING_PORT 22225

#include "HTTPServer.h"
#include <map>
#include "User.h"

class CPDSSeever {
public:

	HTTPServer* httpserver;
	typedef map<string, User*> tUserList;
	tUserList userList;
	map<string, string> portList;

	CPDSSeever();
	virtual ~CPDSSeever();
	void UpdateConnections();
	void PrintAllUsedPorts();
};

// Servlets classes for the HTTP server register.

class Register: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	Register(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~Register() {
	}
	;
};

class LogIn: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	LogIn(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~LogIn() {
	}
	;
};

class GetOnlineUsers: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	GetOnlineUsers(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~GetOnlineUsers() {
	}
	;
};

// TESTTTTTTTTTTTTTTTTtt
class GetJson: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	GetJson(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~GetJson() {
	}
	;
};

// TESTTTTTTTTTTTTTTTTtt
class GetAllUsers: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	GetAllUsers(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~GetAllUsers() {
	}
	;
};

class GetUserDetails: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	GetUserDetails(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~GetUserDetails() {
	}
	;
};

class LogOut: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	LogOut(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~LogOut() {
	}
	;
};
class WebPortal: public HTTPServlet {
public:
	CPDSSeever* CPDS;
	WebPortal(CPDSSeever* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~WebPortal() {
	}
	;
};
#endif /* CPDSSERVICE_H_ */
