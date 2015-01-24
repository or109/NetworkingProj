#ifndef CPDSSERVICE_H_
#define CPDSSERVICE_H_
#define LISTENING_PORT 22225

#include "HTTPServer.h"
#include <map>
#include "User.h"

class CPDSService {
public:

	HTTPServer* httpserver;
	typedef map<string, User*> tUserList;
	tUserList userList;

	CPDSService();
	virtual ~CPDSService();
	void UpdateConnections();
};

// Servlets classes for the HTTP server register.

class Register: public HTTPServlet {
public:
	CPDSService* CPDS;
	Register(CPDSService* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~Register() {
	}
	;
};

class LogIn: public HTTPServlet {
public:
	CPDSService* CPDS;
	LogIn(CPDSService* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~LogIn() {
	}
	;
};

class GetLoggedUsers: public HTTPServlet {
public:
	CPDSService* CPDS;
	GetLoggedUsers(CPDSService* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~GetLoggedUsers() {
	}
	;
};

// TESTTTTTTTTTTTTTTTTtt
class GetJson: public HTTPServlet {
public:
	CPDSService* CPDS;
	GetJson(CPDSService* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~GetJson() {
	}
	;
};

class GetUserDetails: public HTTPServlet {
public:
	CPDSService* CPDS;
	GetUserDetails(CPDSService* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~GetUserDetails() {
	}
	;
};

class LogOut: public HTTPServlet {
public:
	CPDSService* CPDS;
	LogOut(CPDSService* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~LogOut() {
	}
	;
};
class WebPortal: public HTTPServlet {
public:
	CPDSService* CPDS;
	WebPortal(CPDSService* CPDS);
	virtual string handleRequest(map<string, string> params);
	virtual ~WebPortal() {
	}
	;
};
#endif /* CPDSSERVICE_H_ */
