#ifndef LOGIN_H_
#define LOGIN_H_

using namespace std;

#include "MThread.h"
#include "TCPMessengerServer.h"
#include "MultipleTCPSocketsListener.h"
#include "TCPMessengerProtocol.h"
#include <stdlib.h>

class TCPMessengerServer;
class MultipleTCPSocketsListener;

class Login : public MThread{

	void userLogin(TCPSocket* user);
	int login(string userName, string password);
	void createNewUser(TCPSocket* user);
	void disconnect(TCPSocket* user);
	int Register(string user);

public:
	TCPMessengerServer* _server;
	MultipleTCPSocketsListener* _listener;
	bool _isOn;


	Login(TCPMessengerServer* server);
	virtual ~Login();

	void run();
};

#endif /* LOGIN_H_ */
