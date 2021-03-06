// Lab 9
// The server will open a TCP server socket and start listening for
// incoming connections from clients.
// 2. Once a connection with a client is established and a new peer socket is
// created the peer socket is added to a container of peers sockets which
// are managed by the “Dispatcher”.
// 3. The Dispatcher is a new class that listen to all peers and handle the
// incoming commands from the peers.
// 4. At this stage you need to implement all the commands up to sending
// messages between peers.
using namespace std;

#include "Dispatcher.h"
#include "TCPMessengerProtocol.h"

/*
 * The constructor recieves the TCPMessengerServer
 */
Dispatcher::Dispatcher(TCPMessengerServer* tcpMS): MThread(){
	_listener = NULL;
	_server = tcpMS;
	_isON = true;
}

Dispatcher::~Dispatcher() {}

/*
 * Take care important class attributes
 */
bool Dispatcher::isOn(){
	return _isON;
}

void Dispatcher::setIfOn(bool isOn){
	_isON = isOn;
}

void Dispatcher::run(){
	while (isOn()) {

//		string buffer;

		//initialize multiple socket listener
		if (_listener != NULL){
			delete _listener;
		}

		_listener = new MultipleTCPSocketsListener();
		_listener->addSockets(_server->_openPeerVector);


		//Get the socket for a user
		TCPSocket* currentUser = _listener->listenToSocket(2);
		if(currentUser == NULL) {
			// If there isn't a user - end the iteration
			continue;
		}

		// FOR DEBUG
		//we handle the current socket by ip and port
//		string currentConnectedIPandPort = currentUser->destIpAndPort();
//		cout<<"Incoming message from "<<currentConnectedIPandPort<<endl;



		//makes the sockets wait for incoming messages
		switch(_server->recieveCommandFromTCP(currentUser))
		{
			case OPEN_SESSION_WITH_PEER:
			{
				this->openSessionWithPeer(currentUser);
				break;
			}

			case JOIN_ROOM: //Receive room name and add user to that room
			{
				this->joinRoom(currentUser);
				break;
			}

			case CLOSE_SESSION_WITH_PEER:
			{
				closeSeesionWithPeer(currentUser);
				break;
			}

			case CREATE_NEW_ROOM:
			{
				createNewRoom(currentUser);
				break;
			}
			case LEAVE_ROOM:
			{
				leaveRoom(currentUser);
				break;
			}
			case CLOSE_ROOM_REQUEST:
			{
				closeRoomRequest(currentUser);
				break;
			}
			case CONNECTED_USERS:
			{
				getConnectedUsers(currentUser);
				break;
			}
			case USERS_IN_ROOM:
			{
				getUsersInRoom(currentUser);
				break;
			}
			case EXISTED_ROOMS:
			{
				getExistedRooms(currentUser);
				break;
			}
			case REG_USERS:
			{
				getRegisteredUsers(currentUser);
				break;
			}
			case DISCONNECT:
			{
				disconnect(currentUser);
				break;
			}
			default: {
				cout << "no match dispacher" << endl;
			}
		}
	}
}

void Dispatcher::openSessionWithPeer(TCPSocket* user){
	string requested_username = _server->recieveMessageFromTCP(user);
	string requested_userIP = _server->nameToIp(requested_username);

	//Check to see if the user is connected
	if(strcmp(requested_userIP.c_str(),"UserName was not found")==0){
		//Sends SESSION_REFUSED message to the initiator (wanted userName was not found)
		_server->sendCommandToTCP(SESSION_REFUSED,user);
	}
	else{

		//Get the relevant socket from the open peer vector
		int indexOfrequestedPeer = _server->getSocketIndex(_server->_openPeerVector, requested_userIP);
		TCPSocket* peerToConnect = _server->_openPeerVector.at(indexOfrequestedPeer);

		//Sends OPEN_SESSION_WITH_PEER command to the wantedUserName
		_server->sendCommandToTCP(OPEN_SESSION_WITH_PEER,peerToConnect);

		//Check the wantedUserName's state
		int requestPeerCommand = _server->recieveCommandFromTCP(peerToConnect);

		if(requestPeerCommand == IN_SESSION){
			_server->sendCommandToTCP(SESSION_REFUSED_SESSION, user);
		}
		else if(requestPeerCommand == IN_ROOM){
			_server->sendCommandToTCP(SESSION_REFUSED_ROOM,user);
		}
		else if (requestPeerCommand == LOGGED_IN){
			//Sends SESSION_ESTABLISHED message to the initiator with the UDP settings
			_server->sendCommandToTCP(SESSION_ESTABLISHED,user);
			_server->sendMsgToTCP(requested_username+" "+requested_userIP,user);

			//Gets the initiator's userName
			string userName = _server->ipToName(user->getIpAndPort());

			//Sends SESSION_ESTABLISHED message to the wanted with the UDP settings
			_server->sendCommandToTCP(SESSION_ESTABLISHED,peerToConnect);
			_server->sendMsgToTCP(userName+" "+user->getIpAndPort(),peerToConnect);

			//Extracts initiator and wanted peers to the strings vector
			_server->_initiatorSession.push_back(user->getIpAndPort());
			//_server->_wantedSession.push_back(peerToConnect->getIpAndPort());
			puts("User is available");
		}
	}
}

void Dispatcher::joinRoom(TCPSocket* user){
	string roomName = _server->recieveMessageFromTCP(user);

	//checking if the room exists
	int roomIndex=_server->getRoomIndex(roomName);

	// If the room is not exist
	if(roomIndex ==-1){
		//Sending a failure message in case the room doesnt exist
		_server->sendCommandToTCP(NO_SUCH_ROOM_NAME,user);
	}
	// If the room exist
	else{
		//Sends a command to the client that approves its' joining
		_server->sendCommandToTCP(JOIN_ROOM_ARPROVED,user);

		//Sends a message to the client with the room's name
		_server->sendMsgToTCP(roomName,user);

		//Adds the new user to the room's users list(ip:port)
		_server->_rooms.at(roomIndex)->addUserToRoom(user->getIpAndPort());

		//Gets the user's name by the client's IP
		string userNameToSend = _server->ipToName(user->getIpAndPort());

		//Informs all users in the room that the new user has been joined
		_server->sendMsgToAllUsersInRoom(JOIN_ROOM,roomName,userNameToSend);
	}
}

void Dispatcher::closeSeesionWithPeer(TCPSocket* user){
	unsigned int i;
	string closeRequestPeer = user->getIpAndPort();

	int indexInOpenVect;
	for(i=0;i<_server->_initiatorSession.size();i++){
		//Returns the location of the session
		if(_server->_initiatorSession.at(i) == closeRequestPeer){
			indexInOpenVect = _server->getSocketIndex(_server->_openPeerVector, _server->_wantedSession.at(i));
			break;
		}
		if(_server->_wantedSession.at(i)==closeRequestPeer){
			indexInOpenVect = _server->getSocketIndex(_server->_openPeerVector, _server->_initiatorSession.at(i));
			break;
		}
	}

	//Remove the sessions from wantedSession+initiatorSession Vectors
	_server->_initiatorSession.erase(_server->_initiatorSession.begin()+i);
	_server->_wantedSession.erase(_server->_wantedSession.begin()+i);

	//Sends CLOSE_SESSION_WITH_PEER to the other peer
	_server->sendCommandToTCP(CLOSE_SESSION_WITH_PEER,_server->_openPeerVector.at(indexInOpenVect));
}

void Dispatcher::createNewRoom(TCPSocket* user){
	string roomName = _server->recieveMessageFromTCP(user);

	//Check if that roomName already exists
	if(_server->getRoomIndex(roomName) != -1){
		//There is already a room with that name. Sends ROOM_NOT_UNIQUE message to currentUser
		_server->sendCommandToTCP(ROOM_NAME_EXISTS,user);
	}
	else{
		//roomName is available, defines a owner (currentUser)
		string owner = _server->ipToName(user->getIpAndPort());

		//Creates new room with the Ip&Port of the current user and push it to the Rooms Vector
		_server->_rooms.push_back(new Room(roomName,user->getIpAndPort(),owner));

		//Sends CREATE_ROOM_APPROVED command to owner
		_server->sendCommandToTCP(CREATE_ROOM_APPROVED,user);
	}
}

void Dispatcher::leaveRoom(TCPSocket* user){
	//Gets the roomName to be closed
	string roomNametoLeave = _server->recieveMessageFromTCP(user);

	//Returns the index of the desired roomName from Rooms vector
	int roomIndex = this->_server->getRoomIndex(roomNametoLeave);
	_server->_rooms.at(roomIndex)->removeUserFromRoom(user->getIpAndPort());

	//Gets the name of the leaving user
	string tempNameFromIp= _server->ipToName(user->getIpAndPort());

	//Sends a message to all users in the room that that user has been removed from the room
	_server->sendMsgToAllUsersInRoom(LEAVE_ROOM,roomNametoLeave,tempNameFromIp);
	_server->sendCommandToTCP(LEFT_ROOM, user);
}

void Dispatcher::closeRoomRequest(TCPSocket* user){
	string roomNametoClose = _server->recieveMessageFromTCP(user);
	int roomIndex = this->_server->getRoomIndex(roomNametoClose);

	//Checks if the currentsUser is the owner of roomNametoClose
	string userName=_server->ipToName(user->getIpAndPort());
	string ownerName=_server->_rooms.at(roomIndex)->_hostName;

	// If it's not the owner
	if(userName != ownerName){
		_server->sendCommandToTCP(CLOSE_ROOM_DENIED,user);
	}
	// If it's the owner
	else{
		//Loops on all users in the room and sends ROOM_CLOSED command
		for(unsigned int i = 0; i<this->_server->_rooms.at(roomIndex)->_usersInRoom.size();i++)
		{
			string tempIptoSendClose = this->_server->_rooms.at(roomIndex)->_usersInRoom.at(i);
			int userIndex = _server->getSocketIndex(_server->_openPeerVector, tempIptoSendClose);
			_server->sendCommandToTCP(ROOM_CLOSED,_server->_openPeerVector.at(userIndex));

		}
		//Remove the room from Rooms Vector
		_server->_rooms.erase(_server->_rooms.begin()+roomIndex);
	}
}

void Dispatcher::getConnectedUsers(TCPSocket* user){
	string usersName;

	int numberOfUsers= _server->_openPeerVector.size();

	// Move on all the connected users
	for(unsigned int i=0;i < numberOfUsers; i++)
	{
		// concate ip and port
		usersName.append(_server->ipToName(_server->_openPeerVector.at(i)->getIpAndPort()));

		// concate space
		if(i != numberOfUsers -1){
			usersName.append(" ");
		}
	}

	if(numberOfUsers > 0)
	{
		_server->sendCommandToTCP(PRINT_DATA_FROM_SERVER,user);
		_server->sendCommandToTCP(numberOfUsers,user);
		_server->sendMsgToTCP(usersName,user);
	}
	else
		_server->sendCommandToTCP(52, user);
}

void Dispatcher::getUsersInRoom(TCPSocket* user){
	string roomName = _server->recieveMessageFromTCP(user);
	int roomIndex=_server->getRoomIndex(roomName);

	if(roomIndex == -1){
		//roomName was not found in Room's vector
		_server->sendCommandToTCP(NO_SUCH_ROOM_NAME,user);
	}
	else{
		string usersName;
		string tempNameFromIp;

		int numOfUsersInRoom = _server->_rooms.at(roomIndex)->_usersInRoom.size();

		//Loops on all users in the room
		for(unsigned int i=0;i< numOfUsersInRoom;i++)
		{
			//Convert Ip&Port to user name
			tempNameFromIp=_server->ipToName(_server->_rooms.at(roomIndex)->_usersInRoom.at(i));
			//Adds the user name to the string by location(i)
			usersName.append(tempNameFromIp);
			if(i!=_server->_rooms.at(roomIndex)->_usersInRoom.size()-1)
				usersName.append(" ");
		}

		_server->sendCommandToTCP(PRINT_DATA_FROM_SERVER,user);
		//Sends the numOfUsersInRoom as numOfIter
		_server->sendCommandToTCP(numOfUsersInRoom,user);
		_server->sendMsgToTCP(usersName,user);

	}
}

void Dispatcher::getExistedRooms(TCPSocket* user){
	int numOfRoom = _server->_rooms.size();
	string rooms;

	//Loops on all rooms in server
	for(unsigned int i=0; i< numOfRoom ;i++)
	{
		//Adds the room name to the string by location(i)
		rooms.append(_server->_rooms.at(i)->_roomName);

		// Add space
		if(i!=_server->_rooms.size()-1){
			rooms.append(" ");
		}
	}
	if(numOfRoom > 0)
	{
		_server->sendCommandToTCP(PRINT_DATA_FROM_SERVER,user);
		//Sends the numOfRoom as numOfIter
		_server->sendCommandToTCP(numOfRoom,user);
		_server->sendMsgToTCP(rooms,user);
	}
	else{
		_server->sendCommandToTCP(NO_ROOMS, user);
	}
}

void Dispatcher::getRegisteredUsers(TCPSocket* user){
	//Push all userNames to a vector from users.txt
	vector<string> usersFromFile = _server->getUserNamesFromFile();

	int numOfUsers = usersFromFile.size();
	string users;
	for(int i = 0; i <numOfUsers;i++){
		// Create the users string
		users.append(usersFromFile.at(i));

		if(i!=numOfUsers-1){
			users.append(" ");
		}
	}

	if (numOfUsers>0)
	{
		_server->sendCommandToTCP(PRINT_DATA_FROM_SERVER,user);

		//Sends the numOfRoom as numOfIter
		_server->sendCommandToTCP(numOfUsers,user);
		_server->sendMsgToTCP(users,user);
	}
	else
	{
		_server->sendCommandToTCP(NO_USERS, user);
	}
}

void Dispatcher::disconnect(TCPSocket* user){
	//Gets the user's index from openPeerVect and erase it
	int indexUser = _server->getSocketIndex(_server->_openPeerVector,user->getIpAndPort());
	_server->_openPeerVector.erase(_server->_openPeerVector.begin() + indexUser);
}
