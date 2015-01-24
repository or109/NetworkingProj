#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <iostream>
#include <string.h>
#include "TCPSocket.h"
#include "MThread.h"
#include <map>

using namespace std;

class HTTPClient{
private:
	TCPSocket* sock;
	string URL;
	map<string,string> params;

public:
	HTTPClient();
	HTTPClient(string url);
	void setUrl(string url);
	void setParam(string key, string value);
	bool sendGetRequest();
	bool sendPostRequest();
	string response();

};

#endif
