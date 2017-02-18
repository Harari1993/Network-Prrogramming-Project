//============================================================================
// Name        : TCP Messenger Server
// Author      : Danielle Cohen & Amit Harari
// Date		   : 2/12/2015
// Version     :
// Copyright   :
// Description : TCP Messenger application
//============================================================================
#include <strings.h>
#include <map>
#include <vector>
#include "MThread.h"
#include "TCPSocket.h"
#include "UDPManager.h"
#include "TCPMessengerProtocol.h"


using namespace std;

/**
 * The TCP Messenger client class
 */
class TCPMessengerClient : public MThread {
	//TODO: add class properties

public:
	/**
	 * initialize all properties
	 */
	TCPMessengerClient();

	/**
	 * connect to the given server ip (the port is defined in the protocol header file)
	 */
	bool connect(string ip);

	/**
	 * return true if connected to the server
	 */
	bool isConnected();

	/**
	 * disconnect from messenger server
	 */
	bool disconnect();

	/**
	 * open session with the given peer address (ip:port)
	 */
	bool open(string sessionType, string address);

	/**
	 * return true if a session is active
	 */
	bool isActiveClientSession();

	/**
	 * close active session
	 */
	bool closeActiveSession();

	/**
	 * send the given message to the connected peer
	 */
	bool send(string msg);

	void run();
	void TCPtoServerMessage(string msg,int protocol);
	void TCPtoServerCommandProtocol(int protocol);
	void printAllUsersInRoom(string roomName);
	void printAllRooms();
	void printConnectedUsers();
	void printAllUsers();
	void printData(string data, int numOfIter);
	bool loginUser(string user,string pass);
	bool registerUser(string user,string pass);
	void createNewRoom(string roomName);
	void leaveCurrentRoom();
	void printClientStatus();
	void closeRoom(string roomName);


protected:
	bool sendCommand(int command);
	bool sendData(string message);
	int readCommand();
	string readDataFromPeer();
	int state;
	//TCPSocket* clientSock;
	string inSessionWith;
	string userName;
	string roomName;
private:
	TCPSocket* _mainSocket;
	UDPManager* udpManager;
	bool _isActiveSession;
	bool _isRunning;
};

