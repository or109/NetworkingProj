#include "CPDSService.h"

// Transform string to int
bool String2Int(const std::string& str, int& result) {
	return sscanf(str.c_str(), "%d", &result) == 1;
}

CPDSService::CPDSService() {
	httpserver = new HTTPServer(LISTENING_PORT);

	// Register all the servlets
	httpserver->registerServlet("/register", *(new Register(this)));
	httpserver->registerServlet("/login", *(new LogIn(this)));
	httpserver->registerServlet("/getonlineusers", *(new GetOnlineUsers(this)));
	httpserver->registerServlet("/getuserdetails", *(new GetUserDetails(this)));
	httpserver->registerServlet("/logout", *(new LogOut(this)));
	httpserver->registerServlet("/webportal.html", *(new WebPortal(this)));
	httpserver->registerServlet("/getjson", *(new GetJson(this)));

	// Start HTTP server
	httpserver->start();
}

// kill HTTP server
CPDSService::~CPDSService() {
	this->httpserver->kill();
}

void CPDSService::UpdateConnections() {
	for (map<string, User*>::iterator iterator = userList.begin();
			iterator != userList.end(); iterator++) {
		if (iterator->second->loggedin)
			if (httpserver->openedPeers.find(iterator->second->source)
					== httpserver->openedPeers.end())
				iterator->second->logout();
	}
}

// Init CPDS to servlet
Register::Register(CPDSService* CPDS) {
	this->CPDS = CPDS;
}

// Add new user to user list if not exist
string Register::handleRequest(map<string, string> params) {
	map<string, string>::iterator user = params.find("user");
	map<string, string>::iterator pass = params.find("password");
	string data;

	if (user == params.end() || pass == params.end()) {
		data = "ERROR - please enter user and password";
	} else {
		if (this->CPDS->userList.find(user->second)
				== this->CPDS->userList.end()) {
			this->CPDS->userList.insert(
					pair<string, User*>(user->second,
							new User(user->second, pass->second)));
			data = "OK user registered";
		} else
			data = "ERROR - user already exists";
	}

	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf8\r\nContent-Length: ";
	char slen[15];
	sprintf(slen, "%d", (int) data.size());
	message += slen;
	message += "\r\n\r\n";
	message += data;

	return message;
}

// Init CPDS to servlet
LogIn::LogIn(CPDSService* CPDS) {
	this->CPDS = CPDS;
}



// Login to user with password, ip and port
string LogIn::handleRequest(map<string, string> params) {
	string data;
	map<string, string>::iterator user;
	map<string, string>::iterator pass;
	map<string, string>::iterator ip;
	map<string, string>::iterator port;
	map<string, string>::iterator source;
	map<string, string>::iterator portInUse;
	int iport = 0;

	CPDS->UpdateConnections();
	user = params.find("user");
	pass = params.find("password");
	source = params.find("source");
	ip = params.find("ip");
	port = params.find("port");
	string username = user->second;
	string ipPort = ip->second + ":" + port->second;

	portInUse = this->CPDS->portList.find(ip->second + ":" + port->second);

	if (user == params.end() || pass == params.end() || ip == params.end()
			|| port == params.end()) {
		data = "ERROR - please enter user, password, IP and port";
	} else {
		if (this->CPDS->userList.find(username) == this->CPDS->userList.end()) {
			data = "ERROR - user does not exist";
		} else if (this->CPDS->userList.find(username)->second->loggedin)
			data = "ERROR user is already logged in";
		else if (this->CPDS->userList.find(username)->second->password
				!= pass->second)
			data = "ERROR - user password is incorrect";
		else if (portInUse != this->CPDS->portList.end()) // TODO: get port list and scan it
			data = "ERROR - port " + ipPort + " is already in use";
		else {
			this->CPDS->portList.insert(
					pair<string, string>(ipPort, "ip:port"));

			String2Int(port->second, iport);
			this->CPDS->userList.find(username)->second->login(ip->second,
					iport, source->second);
			data = "OK user logged in";
		}
	}
	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf8\r\nContent-Length: ";
	char slen[15];
	sprintf(slen, "%d", (int) data.size());
	message += slen;
	message += "\r\n\r\n";
	message += data;

	return message;
}

// Init CPDS to servlet
GetOnlineUsers::GetOnlineUsers(CPDSService* CPDS) {
	this->CPDS = CPDS;
}

// Return list of logged in users
string GetOnlineUsers::handleRequest(map<string, string> params) {
	string result = "OK";
	CPDS->UpdateConnections();
	for (map<string, User*>::iterator iterator = CPDS->userList.begin();
			iterator != CPDS->userList.end(); iterator++) {
		if (iterator->second->loggedin)
			result += "\n" + iterator->second->username;
	}
	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf8\r\nContent-Length: ";
	char slen[15];
	sprintf(slen, "%d", (int) result.size());
	message += slen;
	message += "\r\n\r\n";
	message += result;
	return message;
}

//Init CPDS to servlet
GetJson::GetJson(CPDSService* CPDS) {
	this->CPDS = CPDS;
}

// Return list of logged in users json format
string GetJson::handleRequest(map<string, string> params) {
	string result = "{\"users\":[";
	bool first = true;

	CPDS->UpdateConnections();

	for (map<string, User*>::iterator iterator = CPDS->userList.begin();
			iterator != CPDS->userList.end(); iterator++) {
		if (iterator->second->loggedin) {
			char portStr[10] = "";
			sprintf(portStr, "%d", iterator->second->port);

			if (!first) {
				result += ", ";
			}
			first = false;

			result += "{\"name\": \"" + iterator->second->username + "\"";
			result += ",\"password\": \"" + iterator->second->password + "\"";
			result += ",\"ip\": \"" + iterator->second->ip + "\"";
			result += ",\"port\":";
			result += portStr;
			result += ",\"lastLoginTime\": \"" + iterator->second->lastLoginTime + "\"";
			result += "}";
		}
	}

	char slen[15];
	result += "]}";

	// Get the Content-Length
	sprintf(slen, "%d", (int) result.size());

	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: ";
	message += slen;
	message += "\r\n\r\n";
	message += result;

	return message;
}

// Init CPDS to servlet
GetUserDetails::GetUserDetails(CPDSService* CPDS) {
	this->CPDS = CPDS;
}

// Return user ip and port if exist and logged in
string GetUserDetails::handleRequest(map<string, string> params) {
	CPDS->UpdateConnections();
	map<string, string>::iterator user;
	string result = "OK ";
	string username = "";

	user = params.find("user");
	if (user == params.end()) {
		result = "ERROR - please enter user";
	} else {
		username = user->second;
		if (this->CPDS->userList.find(username) == this->CPDS->userList.end()) {
			result = "ERROR - user does not exist";
		} else if (this->CPDS->userList.find(username)->second->loggedin) {
			username = user->second;
			char portStr[10] = "";
			sprintf(portStr, "%d",
					this->CPDS->userList.find(username)->second->port);

			result += "User: " + username + ", IP: "
					+ this->CPDS->userList.find(username)->second->ip
					+ ", Port:" + portStr;

		} else
			result = "ERROR - user is not logged in";
	}

	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf8\r\nContent-Length: ";
	char slen[15];
	sprintf(slen, "%d", (int) result.size());
	message += slen;
	message += "\r\n\r\n";
	message += result;

	return message;
}

//Init CPDS to servlet
LogOut::LogOut(CPDSService* CPDS) {
	this->CPDS = CPDS;
}

// Login to user if user exist and logged in
string LogOut::handleRequest(map<string, string> params) {
	CPDS->UpdateConnections();
	map<string, string>::iterator user, pass;
	string data;
	user = params.find("user");
	pass = params.find("password");

	if (user == params.end() || pass == params.end()) {
		data = "ERROR - please enter user and password";
	} else {
		if (this->CPDS->userList.find(user->second)
				== this->CPDS->userList.end()) {
			data = "ERROR - user is not exist!";
		} else if (this->CPDS->userList.find(user->second)->second->password
				!= pass->second)
			data = "ERROR - user password is incorrect";
		else if (this->CPDS->userList.find(user->second)->second->loggedin) {

			this->CPDS->userList.find(user->second)->second->logout();
			data = "OK user logged out";
		} else {
			data = "ERROR - user is already logged out";
		}
	}
	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf8\r\nContent-Length: ";
	char slen[15];
	sprintf(slen, "%d", (int) data.size());
	message += slen;
	message += "\r\n\r\n";
	message += data;

	return message;
}

// Init CPDS to servlet
WebPortal::WebPortal(CPDSService* CPDS) {
	this->CPDS = CPDS;
}

// Return web portal for web browser with all users and status
string WebPortal::handleRequest(map<string, string> params) {
	CPDS->UpdateConnections();

	string data =
			"<!DOCTYPE html><html><head><title>WebPortal</title></head><body><table border=\"1\"><tr><th>User Name</th><th>Status</th></tr>";

	for (map<string, User*>::iterator iterator = CPDS->userList.begin();
			iterator != CPDS->userList.end(); iterator++) {

		data += "<tr><td>" + iterator->second->username + "</td><td>";
		if (iterator->second->loggedin)
			data += "Connected";
		else
			data += "Disconnected";
		data += "</td></tr>";
	}
	data += "</table></body></html>";
	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: text/html; charset=utf8\r\nContent-Length: ";
	char slen[15];
	sprintf(slen, "%d", (int) data.size());
	message += slen;
	message += "\r\n\r\n";
	message += data;

	return message;
}

