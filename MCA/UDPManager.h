#ifndef UDPMANAGER_H_
#define UDPMANAGER_H_

#include <pthread.h>
#include "UDPSocket.h"
#include "MThread.h"
#include <vector>
#include <stdlib.h>

using namespace std;

class UDPManager: public MThread{
	UDPSocket* udpSocket;
	string destIp;
	string destPort;
	string myUserName;

public:
	bool running;
	vector<string> listOfUsersInRoom;
	UDPManager(string myUserName, string myIpandPort);

	/**
	 * sends the given message to the given peer specified by IP
	 */
	void sendToPeer(string msg);

	/**
	 * reply to an incoming message, this method will send the given message
	 * the peer from which the last message was received.
	 */
	void reply(const string& msg);

	/**
	 * close the messenger and all related objects (socket)
	 */
	void close();

	/**
	 * This method runs in a separate thread, it reads the incoming messages
	 * from the socket and prints the content on the terminal.
	 * The thread should exist when the socket is closed
	 */
	void run();

	void setDestinationMessage(string dest);
	void sendToRoom(string msg);
};

#endif /* UDPMANAGER_H_ */
