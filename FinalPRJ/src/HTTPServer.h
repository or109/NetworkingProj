#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <iostream>
#include <string>
#include "MThread.h"
#include "TCPSocket.h"
#include "MultipleTCPSocketsListener.h"
#include <map>
#include <vector>

using namespace std;

class HTTPServer;
class HTTPServlet;

/**
 * The dispatcher server reads incoming commands from open peers and performs the required operations (request - handle - response)
 */
class PeersRequestsDispatcher: public MThread
{
    HTTPServer* webServer;
public:
    MultipleTCPSocketsListener* MTCPListener;
    /**
     * constructor that receive a reference to the parent messenger server
     */
    PeersRequestsDispatcher(HTTPServer* server);

    /**
     * The Dispatcher main loop listen to multi peers requests and handl them.
     */
    void run();
};

/**
 * HTTPServer
 */
class HTTPServer: public MThread
{
    friend class PeersRequestsDispatcher;
    PeersRequestsDispatcher* dispatcher;

    typedef map<string, HTTPServlet*> tHttpServlet;
    tHttpServlet servlets;

private:

TCPSocket* soket;
public:
typedef map<string, TCPSocket*> tOpenedPeers;
  tOpenedPeers openedPeers;

    /**
     * Construct a TCP web server socket
     */
	HTTPServer(int port = 80);
    /**
     * start the server
     */
	void start();
    /**
     * close the server
     */
	void kill();
    /**
     * the web server main loop- this loop waits for incoming clients connection,
     * once a connection is established the new connection is added to the openedPeers
     * on which the dispatcher handle further communication
     */
	void run();

    /**
     * register servlets in the server, the url is just the object path i.e. /home.html
     */
    void registerServlet(string url, HTTPServlet &servlet);
private:
    /**
     * returns the open peers in a vector
     */
    vector<TCPSocket*> getPeersVec();
    /**
     * remove and delete the given peer
     */
    void peerDisconnect(TCPSocket* peer);
    /**
     * read requet from scoket, process the request according to the ritght servlet
     */
    void processRequestFromClient(TCPSocket* peer);

};

/**
 * The servlet prototype class
 */
class HTTPServlet{
public:
    virtual string handleRequest(map<string,string> params)=0;
    virtual ~HTTPServlet(){};
};

#endif
