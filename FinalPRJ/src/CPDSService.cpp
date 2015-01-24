#include "CPDSService.h"

// Transform string to int
bool String2Int(const std::string& str, int& result) {
	return sscanf(str.c_str(), "%d", &result) == 1;
}

CPDSService::CPDSService() {
	// Register all the servlets
	httpserver = new HTTPServer(22225);
	httpserver->registerServlet("/register", *(new Register(this)));
	httpserver->registerServlet("/login", *(new LogIn(this)));
	httpserver->registerServlet("/getlogedusers", *(new GetLoggedUsers(this)));
	httpserver->registerServlet("/getuserdetails", *(new GetUserDetails(this)));
	httpserver->registerServlet("/logout", *(new LogOut(this)));
	httpserver->registerServlet("/webportal.html", *(new Portal(this)));
	httpserver->registerServlet("/getjson", *(new GetJson(this)));

	// Start HTTP server
	httpserver->start();
}

CPDSService::~CPDSService() {
	// kill HTTP server
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

//Init CPDS to servlet
Register::Register(CPDSService* CPDS) {
	this->CPDS = CPDS;
}

// Add new user to user list if not exist
string Register::handleRequest(map<string, string> params) {
	map<string, string>::iterator user, pass;
	string data;
	user = params.find("user");
	pass = params.find("password");
	if (user == params.end() || pass == params.end()) {
		data = "ERROR need to send user and password!";
	} else {
		if (this->CPDS->userList.find(user->second)
				== this->CPDS->userList.end()) {
			this->CPDS->userList.insert(
					pair<string, User*>(user->second,
							new User(user->second, pass->second)));
			data = "OK user sign in";
		} else
			data = "ERROR user is already exist!";
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
LogIn::LogIn(CPDSService* CPDS) {
//Init CPDS to servlet
	this->CPDS = CPDS;
}
// Login to user with password ip and port if user exist and not logged in
string LogIn::handleRequest(map<string, string> params) {
	string data;
	map<string, string>::iterator user, pass, ip, port, source;
	CPDS->UpdateConnections();
	user = params.find("user");
	pass = params.find("password");
	source = params.find("source");
	ip = params.find("ip");
	port = params.find("port");
	if (user == params.end() || pass == params.end() || ip == params.end()
			|| port == params.end()) {
		data = "ERROR need to send user, password, ip and port!";
	} else {
		if (this->CPDS->userList.find(user->second)
				== this->CPDS->userList.end()) {
			data = "ERROR user is not exist!";
		} else if (this->CPDS->userList.find(user->second)->second->loggedin)
			data = "ERROR user is already Logged In!";
		else if (this->CPDS->userList.find(user->second)->second->userPassword
				!= pass->second)
			data = "ERROR user password is incorrect";
		else {
			int iport = 0;
			String2Int(port->second, iport);
			this->CPDS->userList.find(user->second)->second->login(ip->second,
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

GetLoggedUsers::GetLoggedUsers(CPDSService* CPDS) {
//Init CPDS to servlet
	this->CPDS = CPDS;
}

// Return list of logged in users
string GetLoggedUsers::handleRequest(map<string, string> params) {

	string result = "OK";
	CPDS->UpdateConnections();
	for (map<string, User*>::iterator iterator = CPDS->userList.begin();
			iterator != CPDS->userList.end(); iterator++) {
		if (iterator->second->loggedin)
			result += "\n" + iterator->second->userName;
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

GetJson::GetJson(CPDSService* CPDS) {
//Init CPDS to servlet
	this->CPDS = CPDS;
}

// Return list of logged in users
string GetJson::handleRequest(map<string, string> params) {

	string result = "[";
	bool first = true;

	CPDS->UpdateConnections();

	for (map<string, User*>::iterator iterator = CPDS->userList.begin();
			iterator != CPDS->userList.end(); iterator++) {
		if (!first) {
			result += ", ";
			first = false;
		}

		if (iterator->second->loggedin)
			result += "user:" + iterator->second->userName;
	}

	char slen[15];
	result += "]";

	// Get the Content-Length
	sprintf(slen, "%d", (int) result.size());

	string message =
			"HTTP/1.1 200 OK \r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: ";
	message += slen;
	message += "\r\n\r\n";
	message += result;

	return message;
}

GetUserDetails::GetUserDetails(CPDSService* CPDS) {
//Init CPDS to servlet
	this->CPDS = CPDS;
}

// Return user ip and port if exist and logged in
string GetUserDetails::handleRequest(map<string, string> params) {
	CPDS->UpdateConnections();
	map<string, string>::iterator user;
	string result = "OK ";

	user = params.find("user");
	if (user == params.end()) {
		result = "ERROR need to send user !";
	} else {
		if (this->CPDS->userList.find(user->second)
				== this->CPDS->userList.end()) {
			result = "ERROR user is not exist!";
		} else if (this->CPDS->userList.find(user->second)->second->loggedin) {

			char sport[10] = "";
			sprintf(sport, "%d",
					this->CPDS->userList.find(user->second)->second->port);
			result += this->CPDS->userList.find(user->second)->second->ip + ":"
					+ sport;

		} else
			result = "ERROR user is not logged in";
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

LogOut::LogOut(CPDSService* CPDS) {
//Init CPDS to servlet
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
		data = "ERROR need to send user, password!";
	} else {
		if (this->CPDS->userList.find(user->second)
				== this->CPDS->userList.end()) {
			data = "ERROR user is not exist!";
		} else if (this->CPDS->userList.find(user->second)->second->userPassword
				!= pass->second)
			data = "ERROR user password is incorrect";
		else if (this->CPDS->userList.find(user->second)->second->loggedin) {

			this->CPDS->userList.find(user->second)->second->logout();
			data = "OK user logged out";
		} else {
			data = "ERROR user is already Logged Out!";
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
Portal::Portal(CPDSService* CPDS) {
//Init CPDS to servlet
	this->CPDS = CPDS;
}
// Return html portal for web browser that show all users and status
string Portal::handleRequest(map<string, string> params) {
	CPDS->UpdateConnections();

	string data =
			"<!DOCTYPE html><html><head><title>Portal</title></head><body><table border=\"1\"><tr><th>User Name</th><th>Status</th></tr>";

	for (map<string, User*>::iterator iterator = CPDS->userList.begin();
			iterator != CPDS->userList.end(); iterator++) {

		data += "<tr><td>" + iterator->second->userName + "</td><td>";
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

