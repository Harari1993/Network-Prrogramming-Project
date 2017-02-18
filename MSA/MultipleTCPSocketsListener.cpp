#include "MultipleTCPSocketsListener.h"

using namespace std;

/*
 * Adding a socket to be listened on
 */
void MultipleTCPSocketsListener::addSocket(TCPSocket* socket){
	sockets.push_back(socket);
}

/*
 * Adding a vector of sockets to listen on
 */
void MultipleTCPSocketsListener::addSockets(vector<TCPSocket*> socketVec){
	sockets = socketVec;
}

/*
 * Listen on the sockets given before and wait until one of them is ready for reading.
 * Return: The socket ready for reading
 */
TCPSocket* MultipleTCPSocketsListener::listenToSocket(int timeout){
	struct timeval tv = {timeout, 0};
	tySocketsVector::iterator iter = sockets.begin();
	tySocketsVector::iterator endIter = sockets.end();

	//fd_set for select and pselect
	fd_set fdset;
	FD_ZERO(&fdset);
	int highfd = 0;

	//fill the set with file descriptors
	for (;iter != endIter;iter++) {
		highfd++;
		FD_SET((*iter)->getSocketFd(), &fdset);
	}

	int returned;
	if (timeout>0){
		returned = select(sizeof(fdset)*8, &fdset, NULL, NULL, &tv);
	}else{
		returned = select(sizeof(fdset)*8, &fdset, NULL, NULL, NULL);
	}

	// if there is a ready socket
	if (returned == 0) {
		for (int i = 0; i < highfd; i++) {
			TCPSocket* tmpSocket = sockets[i];

			//  tests to see if a file descriptor is part of the set
			if (FD_ISSET(tmpSocket->getSocketFd(), &fdset) == 0) {
				return tmpSocket;
			}
		}
	}

	return NULL;
}
