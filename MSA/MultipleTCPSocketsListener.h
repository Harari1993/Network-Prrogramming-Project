
#ifndef MULTIPLETCPSOCKETSLISTENER_H_
#define MULTIPLETCPSOCKETSLISTENER_H_

#include <iostream>
#include <stdio.h>
#include <vector>
#include "TCPSocket.h"

using namespace std;


class MultipleTCPSocketsListener {
	typedef vector<TCPSocket*> tySocketsVector;
	tySocketsVector sockets;

public:
	void addSocket(TCPSocket* socket);
	void addSockets(vector<TCPSocket*> socketVec);
	TCPSocket* listenToSocket(int timeout=0);
};

#endif /* MULTIPLETCPSOCKETSLISTENER_H_ */
