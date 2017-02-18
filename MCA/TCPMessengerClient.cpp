#include "TCPMessengerClient.h"
#include "TCPMessengerProtocol.h"

#include <string.h>
#include <stdlib.h>

//using namespace std;

/**
 * Initialize all properties
 */
TCPMessengerClient::TCPMessengerClient() {
	_mainSocket = NULL;
	_isActiveSession = false;
	_isRunning = false;
	udpManager = NULL;
	this->state= NOT_CONNECTED;
	this->roomName = "none";
	this->userName = "none";
}

void TCPMessengerClient::run() {
	_isRunning = true;

	// Temp structure to hold date recieved from socket
	char* buffer =new char[1024];

	do {
		int command = readCommand();
		if (command < 1 || command > 6) {
			continue;
		}
		bzero(buffer,1024);
		string message;
		switch (command) {
			case SESSION_ESTABLISHED: {
				cout << "Session was established with remote peer" << endl;
				_isActiveSession = true;
				int msgLen;
				_mainSocket->recv((char*)&msgLen,4);
				msgLen = ntohl(msgLen);
				_mainSocket->recv(buffer, msgLen);
				cout<<"connected to "<<buffer<<endl;
				//Opens UDP session with another user
				if (udpManager != NULL){
					udpManager->setDestinationMessage(strtok(NULL, " "));
				}
				inSessionWith = strtok(buffer, " ");
				state=IN_SESSION;
				break;
			}
			case SESSION_REFUSED:{
				puts("Could not Open Session - The user is not found or disconnected");
				break;
			}
			case SESSION_REFUSED_ROOM:{
				puts("Could not Open Session - The user is in a room");
				break;
			}
			case CLOSE_SESSION_WITH_PEER:{
				puts("Your Session was disconnected");
				state = LOGGED_IN;
				inSessionWith="none";
				break;
			}
			case SESSION_REFUSED_SESSION:{
				puts("Could not Open Session - The user is in session with another user");
				break;
			}
			case OPEN_SESSION_WITH_PEER:{
				TCPtoServerCommandProtocol(state);
				break;
			}
			case LOGIN_ERROR_RESPONSE:{
				puts("Login error");
				userName="none";
				break;
			}
			case LOGIN_APPROVE_RESPONSE:{
				//Server approved login, waiting for UDPManager settings
				int msgLen;
				_mainSocket->recv((char*)&msgLen,4);
				msgLen = ntohl(msgLen);
				_mainSocket->recv(buffer, msgLen);

				//Sets UDPManager
				udpManager = new UDPManager(userName,buffer);
				//Start listening on UDP
				udpManager->start();
				state = LOGGED_IN;
				cout<<"Connected to server as "<<userName<<endl;
				break;
			}
			case ROOM_NAME_EXISTS:{
				this->roomName="none";
				puts("Room name already exists! please choose another room name");
				break;
			}
			case CREATE_ROOM_APPROVED:{
				this->state= IN_ROOM;
				puts("Room created");
				break;
			}
			case JOIN_ROOM_ARPROVED: {
				int msgLen;
				_mainSocket->recv((char*)&msgLen,4);
				msgLen = ntohl(msgLen);
				_mainSocket->recv(buffer, msgLen);
				cout<<"You have been joined to room: "<<buffer<<endl;
				this->state = IN_ROOM;
				this->roomName = buffer;
				break;
			}
			case NO_SUCH_ROOM_NAME:{
				puts("no room with such name");
				break;
			}
			case ROOM_STATUS_CHANGED:{ //update room members
				int msgLen1;
				_mainSocket->recv((char*)&msgLen1,4);
				msgLen1 = ntohl(msgLen1);
				_mainSocket->recv(buffer, msgLen1);
				cout<<buffer<<endl;

				int numofUsersInString;
				_mainSocket->recv((char*)&numofUsersInString,4);
				numofUsersInString = ntohl(numofUsersInString);

				bzero(buffer,1024);

				int msgLen2;
				_mainSocket->recv((char*)&msgLen2,4);
				msgLen2 = ntohl(msgLen2);
				_mainSocket->recv(buffer, msgLen2);

				udpManager->listOfUsersInRoom.clear();
				string tempUserInRoom = strtok(buffer," ");

				for(int i =0; i<numofUsersInString-1;i++){
					udpManager->listOfUsersInRoom.push_back(tempUserInRoom);
					tempUserInRoom=strtok(NULL," ");
				}

				udpManager->listOfUsersInRoom.push_back(tempUserInRoom);
				break;
			}
			case CLOSE_ROOM_DENIED:{
				puts("You're not allowed to close this room");
				break;
			}
			case ROOM_CLOSED:{
				puts("Your room was closed by its owner");
				//Cleans my saved users list in that room (UDPManager)
				udpManager->listOfUsersInRoom.clear();
				this->state=LOGGED_IN;
				this->roomName="none";
				break;
			}
			case NO_ROOMS:{
				cout<<"No rooms opened on server for now."<<endl;
				break;
			}
			case NO_USERS:{
				cout<<"No users have been registered to the server."<<endl;
				break;
			}
			case LEFT_ROOM:{
				puts("You have left the room");
				break;
			}
			case PRINT_DATA_FROM_SERVER:{
				int numOfIter;
				_mainSocket->recv((char*)&numOfIter,4);
				numOfIter = ntohl(numOfIter);

				int msgLen;
				_mainSocket->recv((char*)&msgLen,4);
				msgLen = ntohl(msgLen);
				_mainSocket->recv(buffer, msgLen);

				this->printData(buffer,numOfIter);
				break;
			}
			case NEW_USER_DENIED:{
				puts(" Failed - User name already exists");
				break;
			}
			case NEW_USER_APPROVED:{
				puts("User registered");
				break;
			}
			case SERVER_DISCONNECT:{
				puts("server closed");
				if(state==IN_ROOM)
				{
					this->leaveCurrentRoom();
				}
				if(state==IN_SESSION)
				{
					this->closeActiveSession();
				}
				system("sleep 1");
				_isRunning=false;
				_isActiveSession=false;
				this->udpManager->running=false;
					_mainSocket->cclose();
				this->state=NOT_CONNECTED;

				break;
			}
		}
	} while (_isRunning);
}

/**
 * Connect to the given server ip (the port is defined in the protocol header file)
 */
bool TCPMessengerClient::connect(string ip) {
	//First we check if the user is already connected
	if (state == NOT_CONNECTED) {
		_mainSocket = new TCPSocket(ip,MSNGR_PORT);
		start();
		state = CONNECTED;
		cout << "Connection successfully to: " << ip << endl;;
		return true;
	} else {
		puts("Connection already opened");
		return false;
	}
}

/**
 * Return true if connected to the server
 */
bool TCPMessengerClient::isConnected() {
	return _mainSocket != NULL;
}

/**
 * Disconnect from messenger server
 */
bool TCPMessengerClient::disconnect() {
	if(state!=NOT_CONNECTED)
		{
			if(state==IN_ROOM) {
				this->leaveCurrentRoom();
			}
			if(state==IN_SESSION) {
				this->closeActiveSession();
			}

			this->TCPtoServerCommandProtocol(DISCONNECT);
			system("sleep 1");
			if (udpManager != NULL) {
				this->udpManager->running=false;
			}

			_mainSocket->cclose();
			delete _mainSocket;
			_mainSocket = NULL;
			this->state = NOT_CONNECTED;
			return true;
		}
		else
		{
			puts("You are not connected to the server");
			return true;
		}
		return false;
}

/**
 * Open session with the given peer address (ip:port)
 */
bool TCPMessengerClient::open(string sessionType, string name) {
	//First we check if you connected to the server
	if (isConnected()) {
		//Then we check if you are not already in a session
		if(state != CONNECTED) {
			if(strcmp(sessionType.c_str(),"user")==0) {
				if (state == IN_SESSION) {
					this->closeActiveSession();
				}
				else if (state == IN_ROOM) {
					this->leaveCurrentRoom();
				}
				this->TCPtoServerMessage(name, OPEN_SESSION_WITH_PEER);
			} else if(strcmp(sessionType.c_str(),"room")==0) {
				if (state == IN_SESSION) {
					this->closeActiveSession();
				}
				else if (state == IN_ROOM) {
					this->leaveCurrentRoom();
				}
			this->TCPtoServerMessage(name, JOIN_ROOM);
			} else {
				puts("Invalid input");
				return false;
			}
			return true;
		} else {
			puts("You are not logged in to the chat");
			return false;
		}
	} else {
		puts("You are not connected to server");
		return false;
	}
}


void TCPMessengerClient::printData(string data, int numOfIter){

	char* tempCahrFromData = strdup(data.c_str());
	int i;
	string dataString = strtok(tempCahrFromData," ");
	for(i =0; i<numOfIter-1;i++){
		cout<<i+1<<"."<<dataString<<endl;
		dataString=strtok(NULL," ");
	}
	cout<<i+1<<"."<<dataString<<endl;
	free(tempCahrFromData);
}

/**
 * return true if a session is active
 */
bool TCPMessengerClient::isActiveClientSession() {
	return _isActiveSession;
}

/**
 * Close active session
 */
bool TCPMessengerClient::closeActiveSession() {
	if (!isConnected()) {
		cout << "Not connected to server" << endl;
		return false;
	}

	if (sendCommand(CLOSE_SESSION_WITH_PEER)) {
		_isActiveSession = false;
		return true;
	}

	return false;
}

/*
 * Function that send the request to print all of the connected users
 */
void TCPMessengerClient::printConnectedUsers()
{
	//print all connected users ONLY when connected
	if(this->state == NOT_CONNECTED)
		cout<<"You are not connected"<<endl;
	else
		this->TCPtoServerCommandProtocol(CONNECTED_USERS);

}

/*
 * Sends the request to the server to print all the rooms
 */
void TCPMessengerClient::printAllRooms(){
	//print all connected users ONLY when connected
	if(this->state == NOT_CONNECTED)
		cout<<"You are not connected"<<endl;
	else
		this->TCPtoServerCommandProtocol(EXISTED_ROOMS);
}

/*
 * Function that sends messages to server
 */
void TCPMessengerClient::TCPtoServerMessage(string msg,int protocol){
	this->TCPtoServerCommandProtocol(protocol);

	int msglen=htonl(msg.length());
	//Sends the command to the server
	_mainSocket->send((char*)&msglen,4);
	//Sends the message to the server
	_mainSocket->send(msg.c_str(),(msg.length()));
}
/*
 * This function send to server the protocol of the next message
 */
void TCPMessengerClient::TCPtoServerCommandProtocol(int protocol)
{
	int command = htonl(protocol);
	_mainSocket->send((char*)&command,4);
}

// FUnction that send login request to the server
bool TCPMessengerClient::loginUser(string user,string pass) {
	if (state == CONNECTED) {
		string msg = user + " " + pass;
		this->TCPtoServerMessage(msg,USER_LOGIN_REQUEST);
		//Sets this client as the userName
		userName=user;
		return true;
	} else if (state == NOT_CONNECTED) {
		puts("You are not connected to any server");
		return false;
	} else {
		puts("You must disconnect from server first");
		return false;
	}
}

bool TCPMessengerClient::registerUser(string user,string pass) {
	if (state == CONNECTED) {
		string msg = user + " " + pass;
		this->TCPtoServerMessage(msg,CREATE_NEW_USER);
		//Sets this client as the userName
		userName=user;
		return true;
	} else if (state == NOT_CONNECTED) {
		puts("You are not connected to any server");
		return false;
	} else {
		puts("You must disconnect from server first");
		return false;
	}
}
/*
 * Helper function for closing a room
 */
void TCPMessengerClient::closeRoom(string roomName){

	if(state == IN_ROOM)
		this->TCPtoServerMessage(roomName,CLOSE_ROOM_REQUEST);

}

/*
 * functions that sends a request to create a new roo,
 */
void TCPMessengerClient::createNewRoom(string roomName)
{
	//Sends CREATE_NEW_ROOM command to the main server with the room name
	this->TCPtoServerMessage(roomName,CREATE_NEW_ROOM);
	//this->roomName= roomName;

}

void TCPMessengerClient::printAllUsers(){
	if(this->state == NOT_CONNECTED) //print all users ONLY when connected
		cout<<"You are not connected"<<endl;
	else
		this->TCPtoServerCommandProtocol(REG_USERS);
}

/*
 * Sends the request to print all the users in a room
 */
void TCPMessengerClient::printAllUsersInRoom(string roomName){
	if(this->state == NOT_CONNECTED)
		cout<<"You must be connected"<<endl;
	else
		this->TCPtoServerMessage(roomName,USERS_IN_ROOM);
}

void TCPMessengerClient::leaveCurrentRoom()
{
	if(state==IN_ROOM)
	{
		this->TCPtoServerMessage(this->roomName,LEAVE_ROOM);
		this->state=LOGGED_IN;
		this->roomName="none";
	}
	else
		puts("You're not in a room");
}

/*
 * Prints the status of the client, connected,in a room, in a session.
 */
void  TCPMessengerClient::printClientStatus(){
	if(this->state==LOGGED_IN){
		cout<<"You are connected to server as: "<<this->userName<<endl;
	}
	else if(this->state==IN_SESSION){
		cout<<"You are in session with: "<<inSessionWith<<endl;
	}
	else
		cout<<"You are currently not connected to any server"<<endl;
}
/**
 * send the given message to the connected peer
 */
bool TCPMessengerClient::send(string msg) {
	if (!isActiveClientSession()) {
		return false;
	}

	return sendCommand(SEND_MSG_TO_PEER) && sendData(msg);
}

bool TCPMessengerClient::sendCommand(int command) {
	int commandToSend = htonl(command);
	int commandBytesSent = _mainSocket->send((char*) &commandToSend, 4);

	return commandBytesSent == 4;
}

bool TCPMessengerClient::sendData(string message) {
	int msgLen = htonl(message.length());
	int msgLenBytesSent = _mainSocket->send((char*) &msgLen, 4);
	int msgBytesSent = _mainSocket->send(message.data(), message.length());

	return msgLenBytesSent + msgBytesSent == 4 + (int) message.length();
}

int TCPMessengerClient::readCommand() {
	int command;
	_mainSocket->recv((char*) &command, 4);
	command = ntohl(command);
	cout << "command client: " << command << endl;
	return command;
}

string TCPMessengerClient::readDataFromPeer() {
	string message;

	int msgLen;
	int msgLenBytesRead = _mainSocket->recv((char*) &msgLen, 4);
	if (msgLenBytesRead < 4) {
		return "";
	}

	msgLen = ntohl(msgLen);

	char* buffer = new char[msgLen];
	memset((void*) buffer, 0, msgLen);

	int bytesRead = 0;
	while (bytesRead < msgLen) {
		bytesRead += _mainSocket->recv(buffer, msgLen - bytesRead);
	}

	message = string(buffer);
	delete[] buffer;

	return message;
}
