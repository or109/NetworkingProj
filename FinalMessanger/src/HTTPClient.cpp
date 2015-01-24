#include "HTTPClient.h"


HTTPClient::HTTPClient(){
	this->sock = NULL;

}

HTTPClient::HTTPClient(string url){
	char host[64];

	sscanf(url.data(), "%[^/]", host);

	this->sock = new TCPSocket(host,22225);

	this->URL = url;



}

void HTTPClient::setUrl(string url){
	sock->cclose();
	delete sock;
	this->sock = new TCPSocket(url,80);
	this->URL = url;

}

void HTTPClient::setParam(string key, string value){
	params.insert(pair<string,string>(key,value));

}

bool HTTPClient::sendGetRequest(){

	char uri[100] = "",host[100] = "";
	sscanf(URL.data(), "%[^/]%s",host ,uri);

	string 	msg = "GET " + (string)uri + "?" ;

	 for (map<string,string>::iterator param = params.begin(); param != params.end(); ++param)
		    {
				msg+= param->first + "=" + param->second + "&";
		    }
	 	 msg = msg.substr(0, msg.size()-1);

			msg+= " HTTP/1.0\n\n";
	//cout << "sending - " << msg.data() << endl;
	int send = sock->send(msg.data(),msg.length());
	if (send == msg.length()){
		return true;
	}
	return false;
}

bool HTTPClient::sendPostRequest(){
		string msg = "POST " + URL + " HTTP/1.1";
		//msg+=" " + key + "=" + "value";
		cout << "sending - " << msg.data() << endl;
		int send = sock->send(msg.data(),msg.length());
		if (send == msg.length()){
			return true;
		}
		return false;
}

string HTTPClient::response(){
	char buffer[1000];
	memset((void*)buffer,0,1000);
	int rc = sock->recv(buffer,1000);


	return buffer;
}

