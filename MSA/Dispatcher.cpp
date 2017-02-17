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
Dispatcher::Dispatcher(TCPMessengerServer* tcpMS){
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
		_listener->addSockets(_server->getOpenPeerVector());


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
			case 2: //OPEN_SESSION_WITH_PEER
			{
				this->openSessionWithPeer(currentUser);
				break;
			}

			case 3://JOIN_ROOM: //Receive room name and add user to that room
			{
				this->joinRoom(currentUser);
				break;
			}

			case CLOSE_SESSION_WITH_PEER:
			{
				closeSeesionWithPeer(currentUser);
				break;
			}

			case 12://CREATE_NEW_ROOM
			{
				createNewRoom(currentUser);
				break;
			}
			case 18://LEAVE_ROOM
			{
				leaveRoom(currentUser);
				break;
			}
			case 19://CLOSE_ROOM_REQUEST
			{
				closeRoomRequest(currentUser);
				break;
			}
			case 41: //simple print all connected users//CONNECTED_USERS
			{
				getConnectedUsers(currentUser);
				break;
			}
			case 42://USERS_IN_ROOM
			{
				getUsersInRoom(currentUser);
				break;
			}
			case 43://EXISTED_ROOMS
			{
				getExistedRooms(currentUser);
				break;
			}
			case 40://REG_USERS
			{
				getRegisteredUsers(currentUser);
				break;
			}
			case 26://DISCONNECT
			{
				disconnect(currentUser);
				break;
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
		int indexOfrequestedPeer = _server->getSocketIndex(_server->getOpenPeerVector(), requested_userIP);
		TCPSocket* peerToConnect = _server->getOpenPeerVector().at(indexOfrequestedPeer);

		//Sends OPEN_SESSION_WITH_PEER command to the wantedUserName
		_server->sendCommandToTCP(OPEN_SESSION_WITH_PEER,peerToConnect);

		//Check the wantedUserName's state
		int requestPeerCommand = _server->recieveCommandFromTCP(peerToConnect);

		if(requestPeerCommand == 34){//IN_SESSION
			_server->sendCommandToTCP(29, user);//SESSION_REFUSED_SESSION
		}
		else if(requestPeerCommand == 35){//IN_ROOM
			_server->sendCommandToTCP(28,user);//SESSION_REFUSED_ROOM
		}
		else if (requestPeerCommand == 33){//LOGGED_IN
			//Sends SESSION_ESTABLISHED message to the initiator with the UDP settings
			_server->sendCommandToTCP(SESSION_ESTABLISHED,user);
			_server->sendMsgToTCP(requested_username+" "+requested_userIP,user);

			//Gets the initiator's userName
			string userName = _server->ipToName(user->getIpAndPort());

			//Sends SESSION_ESTABLISHED message to the wanted with the UDP settings
			_server->sendCommandToTCP(SESSION_ESTABLISHED,peerToConnect);
			_server->sendMsgToTCP(userName+" "+user->getIpAndPort(),peerToConnect);

			//Extracts initiator and wanted peers to the strings vector
			_server->getInitiatorSessions().push_back(user->getIpAndPort());
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
		_server->sendCommandToTCP(15,user);//NO_SUCH_ROOM_NAME
	}
	// If the room exist
	else{
		//Sends a command to the client that approves its' joining
		_server->sendCommandToTCP(16,user);//JOIN_ROOM_ARPROVED

		//Sends a message to the client with the room's name
		_server->sendMsgToTCP(roomName,user);

		//Adds the new user to the room's users list(ip:port)
		_server->getRooms().at(roomIndex)->addUserToRoom(user->getIpAndPort());

		//Gets the user's name by the client's IP
		string userNameToSend = _server->ipToName(user->getIpAndPort());

		//Informs all users in the room that the new user has been joined
		_server->sendMsgToAllUsersInRoom(3,roomName,userNameToSend);//JOIN_ROOM
	}
}

void Dispatcher::closeSeesionWithPeer(TCPSocket* user){
	unsigned int i;
	string closeRequestPeer = user->getIpAndPort();

	int indexInOpenVect;
	for(i=0;i<_server->getInitiatorSessions().size();i++){
		//Returns the location of the session
		if(_server->getInitiatorSessions().at(i) == closeRequestPeer){
			indexInOpenVect = _server->getSocketIndex(_server->getOpenPeerVector(), _server->getWantesSession().at(i));
			break;
		}
		if(_server->getWantesSession().at(i)==closeRequestPeer){
			indexInOpenVect = _server->getSocketIndex(_server->getOpenPeerVector(), _server->getInitiatorSessions().at(i));
			break;
		}
	}

	//Remove the sessions from wantedSession+initiatorSession Vectors
	_server->getInitiatorSessions().erase(_server->getInitiatorSessions().begin()+i);
	_server->getWantesSession().erase(_server->getWantesSession().begin()+i);

	//Sends CLOSE_SESSION_WITH_PEER to the other peer
	_server->sendCommandToTCP(CLOSE_SESSION_WITH_PEER,_server->getOpenPeerVector().at(indexInOpenVect));
}

void Dispatcher::createNewRoom(TCPSocket* user){
	string roomName = _server->recieveMessageFromTCP(user);

	//Check if that roomName already exists
	if(_server->getRoomIndex(roomName) != -1){
		//There is already a room with that name. Sends ROOM_NOT_UNIQUE message to currentUser
		_server->sendCommandToTCP(13,user);//ROOM_NOT_UNIQUE
	}
	else{
		//roomName is available, defines a owner (currentUser)
		string owner = _server->ipToName(user->getIpAndPort());

		//Creates new room with the Ip&Port of the current user and push it to the Rooms Vector
		_server->getRooms().push_back(new Room(roomName,user->getIpAndPort(),owner));

		//Sends CREATE_ROOM_APPROVED command to owner
		_server->sendCommandToTCP(14,user);//CREATE_ROOM_APPROVED
	}
}

void Dispatcher::leaveRoom(TCPSocket* user){
	//Gets the roomName to be closed
	string roomNametoLeave = _server->recieveMessageFromTCP(user);

	//Returns the index of the desired roomName from Rooms vector
	int roomIndex = this->_server->getRoomIndex(roomNametoLeave);
	_server->getRooms().at(roomIndex)->removeUserFromRoom(user->getIpAndPort());

	//Gets the name of the leaving user
	string tempNameFromIp= _server->ipToName(user->getIpAndPort());

	//Sends a message to all users in the room that that user has been removed from the room
	_server->sendMsgToAllUsersInRoom(18,roomNametoLeave,tempNameFromIp);//LEAVE_ROOM
	_server->sendCommandToTCP(61, user);//LEFT_ROOM
}

void Dispatcher::closeRoomRequest(TCPSocket* user){
	string roomNametoClose = _server->recieveMessageFromTCP(user);
	int roomIndex = this->_server->getRoomIndex(roomNametoClose);

	//Checks if the currentsUser is the owner of roomNametoClose
	string userName=_server->ipToName(user->getIpAndPort());
	string ownerName=_server->getRooms().at(roomIndex)->getHostName();

	// If it's not the owner
	if(userName != ownerName){
		_server->sendCommandToTCP(21,user);//CLOSE_ROOM_DENIED
	}
	// If it's the owner
	else{
		//Loops on all users in the room and sends ROOM_CLOSED command
		for(unsigned int i = 0; i<this->_server->getRooms().at(roomIndex)->getUsersInRoom().size();i++)
		{
			string tempIptoSendClose = this->_server->getRooms().at(roomIndex)->getUsersInRoom().at(i);
			int userIndex = _server->getSocketIndex(_server->getOpenPeerVector(), tempIptoSendClose);
			_server->sendCommandToTCP(20,_server->getOpenPeerVector().at(userIndex));//ROOM_CLOSED

		}
		//Remove the room from Rooms Vector
		_server->getRooms().erase(_server->getRooms().begin()+roomIndex);
	}
}

void Dispatcher::getConnectedUsers(TCPSocket* user){
	string usersName;

	int numberOfUsers= _server->getOpenPeerVector().size();

	// Move on all the connected users
	for(unsigned int i=0;i < numberOfUsers; i++)
	{
		// concate ip and port
		usersName.append(_server->ipToName(_server->getOpenPeerVector().at(i)->getIpAndPort()));

		// concate space
		if(i != numberOfUsers -1){
			usersName.append(" ");
		}
	}

	if(numberOfUsers > 0)
	{
		_server->sendCommandToTCP(22,user);//PRINT_DATA_FROM_SERVER
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
		_server->sendCommandToTCP(15,user);//NO_SUCH_ROOM_NAME
	}
	else{
		string usersName;
		string tempNameFromIp;

		int numOfUsersInRoom = _server->getRooms().at(roomIndex)->getUsersInRoom().size();

		//Loops on all users in the room
		for(unsigned int i=0;i< numOfUsersInRoom;i++)
		{
			//Convert Ip&Port to user name
			tempNameFromIp=_server->ipToName(_server->getRooms().at(roomIndex)->getUsersInRoom().at(i));
			//Adds the user name to the string by location(i)
			usersName.append(tempNameFromIp);
			if(i!=_server->getRooms().at(roomIndex)->getUsersInRoom().size()-1)
				usersName.append(" ");
		}

		_server->sendCommandToTCP(22,user);//PRINT_DATA_FROM_SERVER
		//Sends the numOfUsersInRoom as numOfIter
		_server->sendCommandToTCP(numOfUsersInRoom,user);
		_server->sendMsgToTCP(usersName,user);

	}
}

void Dispatcher::getExistedRooms(TCPSocket* user){
	int numOfRoom = _server->getRooms().size();
	string rooms;

	//Loops on all rooms in server
	for(unsigned int i=0; i< numOfRoom ;i++)
	{
		//Adds the room name to the string by location(i)
		rooms.append(_server->getRooms().at(i)->getRoomName());

		// Add space
		if(i!=_server->getRooms().size()-1){
			rooms.append(" ");
		}
	}
	if(numOfRoom > 0)
	{
		_server->sendCommandToTCP(22,user);//PRINT_DATA_FROM_SERVER
		//Sends the numOfRoom as numOfIter
		_server->sendCommandToTCP(numOfRoom,user);
		_server->sendMsgToTCP(rooms,user);
	}
	else{
		_server->sendCommandToTCP(60, user);//NO_ROOMS
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
		_server->sendCommandToTCP(22,user);//PRINT_DATA_FROM_SERVER

		//Sends the numOfRoom as numOfIter
		_server->sendCommandToTCP(numOfUsers,user);
		_server->sendMsgToTCP(users,user);
	}
	else
	{
		_server->sendCommandToTCP(62, user);//NO_USERS
	}
}

void Dispatcher::disconnect(TCPSocket* user){
	//Gets the user's index from openPeerVect and erase it
	int indexUser = _server->getSocketIndex(_server->getOpenPeerVector(),user->getIpAndPort());
	_server->getOpenPeerVector().erase(_server->getOpenPeerVector().begin() + indexUser);
}
