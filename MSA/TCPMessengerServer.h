#ifndef TCPMESSENGERSERVER_H__
#define TCPMESSENGERSERVER_H__

using namespace std;

#include <string.h>
#include "MThread.h"
#include "TCPSocket.h"
#include "Dispatcher.h"
#include "Room.h"

class Dispatcher;

class TCPMessengerServer : public MThread{
	TCPSocket* 			_serverSocket;
	Dispatcher* 		_dispatcher;
	vector<TCPSocket*> 	_openPeerVector; 	// Connected users sockets
	vector<string> 		_ipToClientName; 	// Mapping between ip to name of connected users
	vector<string> 		_initiatorSession;
	vector<string> 		_wantedSession;
	vector<Room*> 		_rooms;
public:
	/**
	 * Build the Messenger server
	 */
	TCPMessengerServer();
	~TCPMessengerServer();

	vector<TCPSocket*> getOpenPeerVector();
	vector<string> getIpToClientName();
	vector<string> getInitiatorSessions();
	vector<string> getWantesSession();
	vector<Room*> getRooms();


	//	int posInVec;

	//	LoginManager* myLoginManager;


	//	vector<TCPSocket*> connectedSocketsVect; //_openPeerVector

	//
	//
	//	void run();
	//	void listPeers();
	//	void PrintOpenPeerVector();
	//
		int recieveCommandFromTCP(TCPSocket * tmpTCP);
		char * recieveMessageFromTCP(TCPSocket * tmpTCP);
		void sendCommandToTCP(int protocol,TCPSocket * tmpTCP);
		void sendMsgToTCP(string msg, TCPSocket* tmpTCP);
	//	void insertToOpenVector(TCPSocket* temp_soc);
		int getSocketIndex(vector<TCPSocket*> vector, string address);
	//	void ClientListConnedtedUsers(TCPSocket * tmpTCP);
		string nameToIp(string name);
		string ipToName(string ip);
	//	void ListSessions();
		int getRoomIndex(string roomName);
	//	void printUserinRoom(string name);
	//	string UsersInRoomToString(string roomName);
		void sendMsgToAllUsersInRoom(int msgType,string roomName, string userName);
		vector<string> getUserNamesFromFile();
	//	void printAllUsers();
	//	void close();
};
#endif
