#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include "MThread.h"
#include "MultipleTCPSocketsListener.h"
#include "TCPMessengerServer.h"

class TCPMessengerServer;

class Dispatcher : public MThread {
private:
	MultipleTCPSocketsListener* _listener;
	TCPMessengerServer* _server;
	bool _isON;

	void openSessionWithPeer(TCPSocket* user);
	void joinRoom(TCPSocket* user);
	void closeSeesionWithPeer(TCPSocket* user);
	void createNewRoom(TCPSocket* user);
	void leaveRoom(TCPSocket* user);
	void closeRoomRequest(TCPSocket* user);
	void getConnectedUsers(TCPSocket* user);
	void getUsersInRoom(TCPSocket* user);
	void getExistedRooms(TCPSocket* user);
	void getRegisteredUsers(TCPSocket* user);
	void disconnect(TCPSocket* user);

public:
	Dispatcher(TCPMessengerServer* tcpMS);
	virtual ~Dispatcher();

	bool isOn();
	void setIfOn(bool isOn);

	void run();
};

#endif /* DISPATCHER_H_ */
