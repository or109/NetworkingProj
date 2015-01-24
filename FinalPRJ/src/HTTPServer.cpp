#include "HTTPServer.h"
#include <string.h>
#include <string>

void Tokenize(const string& str, vector<string>& tokens,
		const string& delimiters = " ") {
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

/**
 * The dispatcher server reads incoming commands from open peers and performs the required operations (request - handle - response)
 */

/**
 * constructor that receive a reference to the parent messenger server
 */
PeersRequestsDispatcher::PeersRequestsDispatcher(HTTPServer* server) {
	this->webServer = server;
	this->MTCPListener = NULL;
}

/**
 * The Dispatcher main loop listen to multi peers requests and handl them.
 */
void PeersRequestsDispatcher::run() {
	while (true) {
		MTCPListener = new MultipleTCPSocketsListener();

		MTCPListener->addSockets(webServer->getPeersVec());

		TCPSocket* peer = MTCPListener->listenToSocket();
		if (peer != NULL) {
			webServer->processRequestFromClient(peer);
			//	webServer->peerDisconnect(peer);
		}
	}
}

/**
 * HTTPServer
 */
/**
 * Construct a TCP web server socket
 */
HTTPServer::HTTPServer(int port) {
	this->soket = new TCPSocket(port);
	this->dispatcher = new PeersRequestsDispatcher(this);

}
/**
 * start the server
 */
void HTTPServer::start() {
	MThread::start();
	dispatcher->start();
}
/**
 * close the server
 */
void HTTPServer::kill() {
	soket->cclose();

	dispatcher->~MThread();

	for (tOpenedPeers::iterator iterator = openedPeers.begin();
			iterator != openedPeers.end(); iterator++) {

		peerDisconnect(iterator->second);
	}

	openedPeers.clear();
	this->~MThread();

	delete soket;
	delete dispatcher;
}
/**
 * the web server main loop- this loop waits for incoming clients connection,
 * once a connection is established the new connection is added to the openedPeers
 * on which the dispatcher handle further communication
 */
void HTTPServer::run() {
	while (true) {
		TCPSocket* secondarySock = soket->listenAndAccept();

		string ip = secondarySock->fromAddr();
		string ipport = secondarySock->fromAddr() + ":"
				+ secondarySock->fromPort();

		if (openedPeers.find(ipport) == openedPeers.end()) {
			openedPeers.insert(pair<string, TCPSocket*>(ipport, secondarySock));
		}
	}
}

/**
 * register servlets in the server, the url is just the object path i.e. /home.html
 */
void HTTPServer::registerServlet(string url, HTTPServlet &servlet) {
	servlets.insert(pair<string, HTTPServlet*>(url, &servlet));

}

/**
 * returns the open peers in a vector
 */
vector<TCPSocket*> HTTPServer::getPeersVec() {
	vector<TCPSocket*> sokets;
	for (tOpenedPeers::iterator iterator = openedPeers.begin();
			iterator != openedPeers.end(); iterator++) {

		sokets.push_back(iterator->second);
	}

	return sokets;
}
/**
 * remove and delete the given peer
 */
void HTTPServer::peerDisconnect(TCPSocket* peer) {
	string ipport = peer->fromAddr() + ":" + peer->fromPort();
	openedPeers.erase(ipport);
	peer->cclose();
	delete peer;
}
/**
 * read requet from scoket, process the request according to the ritght servlet
 */
void HTTPServer::processRequestFromClient(TCPSocket* peer) {
	char buffer[1000];
	int rc = 0;
	char query[100] = "", method[20] = "", url[100] = "", httpVer[100] = "",
			uri[100] = "";
	string message = "";
	vector<string> querys;
	char p1[100] = "", p2[100] = "";
	map<string, string> params;

	memset((void*) buffer, 0, 1000);
	rc = peer->recv(buffer, 1000);

	string ipport = peer->fromAddr() + ":" + peer->fromPort();
	cout << "recv msg: from- " << ipport << buffer << endl;

	if (rc != 0) {
		sscanf(buffer, "%s %s %s", method, url, httpVer);

		sscanf(url, "%[^?]?%s", uri, query);

		querys.clear();
		Tokenize(query, querys, "&");

		params.clear();
		for (vector<string>::iterator param = querys.begin();
				param != querys.end(); ++param) {
			sscanf(param->data(), "%[^=]=%s", p1, p2);
			params.insert(pair<string, string>(p1, p2));
		}
		char socket[10];
		sprintf(socket, "%d", peer->sock);
		params.insert(pair<string, string>("source", ipport));
		tHttpServlet::iterator servlet = servlets.find(uri);
		if (servlet != servlets.end()) {
			message = servlet->second->handleRequest(params);
		} else {

			string data =
					"<html><head><title>404 Not Found</title></head><body align=\"center\"><h1>Page not found</h1></body></html>";

			char slen[15];
			sprintf(slen, "%d", data.size());
			message =
					"HTTP/1.1 404 Not Found \r\nContent-Type: text/html; charset=utf8\r\nContent-Length: ";
			message += slen;
			message += "\r\n\r\n";
			message += data;

		}

		cout << "sending message:" << message << endl;
		peer->send(message.data(), message.length());
	} else
		this->peerDisconnect(peer);
}

