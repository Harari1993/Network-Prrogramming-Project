#ifndef TCPMESSENGERSERVER_H__
#define TCPMESSENGERSERVER_H__

using namespace std;

#include <string.h>
#include "MThread.h"
#include "TCPSocket.h"
#include "Dispatcher.h"
#include "Room.h"
#include "Login.h"

class Dispatcher;
class Login;

class TCPMessengerServer : public MThread{
public:
	TCPSocket* 			_serverSocket;
	Dispatcher* 		_dispatcher;
	vector<TCPSocket*> 	_openPeerVector; 	// Connected users sockets
	vector<string> 		_ipToClientName; 	// Mapping between ip to name of connected users
	vector<string> 		_initiatorSession;
	vector<string> 		_wantedSession;
	vector<Room*> 		_rooms;
	Login* 				_login;

	/**
	 * Build the Messenger server
	 */
	TCPMessengerServer();
	~TCPMessengerServer();

	//	int posInVec;
	//	vector<TCPSocket*> connectedSocketsVect; //_openPeerVector

	void run();
	void printAllPeers();
	int recieveCommandFromTCP(TCPSocket * tmpTCP);
	char * recieveMessageFromTCP(TCPSocket * tmpTCP);
	void sendCommandToTCP(int protocol,TCPSocket * tmpTCP);
	void sendMsgToTCP(string msg, TCPSocket* tmpTCP);
	//	void insertToOpenVector(TCPSocket* temp_soc);
	int getSocketIndex(vector<TCPSocket*> vector, string address);
	//	void ClientListConnedtedUsers(TCPSocket * tmpTCP);
	string nameToIp(string name);
	void notifyShutdown();
	string ipToName(string ip);
	void printAllSessions();
	void printAllRooms();
	int getRoomIndex(string roomName);
	void printUserInRoom(string name);
	string usersInRoomToString(string roomName);
	void sendMsgToAllUsersInRoom(int msgType,string roomName, string userName);
	vector<string> getUserNamesFromFile();
	void printAllUsers();
	void close();
};
#endif
