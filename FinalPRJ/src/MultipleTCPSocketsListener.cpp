

#include "MultipleTCPSocketsListener.h"

using namespace std;


/*
 * Add the given socket to the socket list to be listen on
 */
void MultipleTCPSocketsListener::addSocket(TCPSocket* socket){
	clientsSockets.push_back(socket);
}

/*
 * Set the given sockets to the socket list to be listen on
 */
void MultipleTCPSocketsListener::addSockets(vector<TCPSocket*> socketVec){
	for(std::vector<TCPSocket*>::iterator it = socketVec.begin(); it != socketVec.end(); ++it) {
		MultipleTCPSocketsListener::addSocket(*it);
	 }
}

/*
 * This method checks to see if any of the previous given sockets are ready for reading
 * It returns one of the Sockets that are ready.
 */
TCPSocket* MultipleTCPSocketsListener::listenToSocket(){
	//TODO: create local set for the select function (fd_set)
	fd_set fdSet;
	int maxFileDescriptor = -1;
	int currFileDescriptor;

	FD_ZERO(&fdSet);
	//TODO: fill the set with file descriptors from the socket list using (FD_SET macro)
	for(std::vector<TCPSocket*>::iterator it = clientsSockets.begin(); it != clientsSockets.end(); ++it) {
		currFileDescriptor = (*it)->getFileDescriptor();
		FD_SET(currFileDescriptor, &fdSet);

		if(currFileDescriptor > maxFileDescriptor){
			maxFileDescriptor = currFileDescriptor;
		}
	 }

	timeval* timeout = new timeval();
	timeout->tv_sec = 5 ;
	//TODO: perform the select
	int selectResult = select(maxFileDescriptor + 1, &fdSet, NULL, NULL, timeout);

	//TODO: check the returned value from the select to find the socket that is ready
	if(selectResult < 0){
		printf("select error");
	}
	else{
		if(selectResult > 0){
			for(std::vector<TCPSocket*>::iterator it = clientsSockets.begin(); it != clientsSockets.end(); ++it) {
				currFileDescriptor = (*it)->getFileDescriptor();
				if(FD_ISSET(currFileDescriptor, &fdSet)){
					return (*it);
				}
			}
		}
	}
	//TODO: if select return a valid socket return the matching TCPSocket object otherwise return NULL
	return NULL;
}

