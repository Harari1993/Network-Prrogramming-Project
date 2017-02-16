#include "TCPMessengerClient.h"
#include "TCPMessengerProtocol.h"

#include <string.h>
#include <stdlib.h>

using namespace std;

/**
 * initialize all properties
 */
TCPMessengerClient::TCPMessengerClient() {
	_mainSocket = NULL;
	_isActiveSession = false;
	_isRunning = false;
}

void TCPMessengerClient::run() {
	_isRunning = true;

	do {
		int command = readCommand();
		if (command < 1 || command > 6) {
			continue;
		}

		string message;
		switch (command) {
			case SEND_MSG_TO_PEER:
				message = readDataFromPeer();
				cout << ">>" << message << endl;
				break;
			case CLOSE_SESSION_WITH_PEER:
				cout << "Session was closed by remote peer" << endl;
				_isActiveSession = false;
				break;
			case OPEN_SESSION_WITH_PEER:
				cout << "Session was opened by remote peer " << readDataFromPeer() << endl;
				_isActiveSession = true;
				break;
			case SESSION_ESTABLISHED:
				cout << "Session was established with remote peer" << endl;
				_isActiveSession = true;
				break;
			default:
				cout << "Communication with server was interrupted - connection closed" << endl;
				disconnect();
				_isRunning = false;
				break;
		}
	} while (_isRunning);
}

/**
 * connect to the given server ip (the port is defined in the protocol header file)
 */
bool TCPMessengerClient::connect(string ip) {
	if (isConnected()) {
		disconnect();
	}

	if (ip.find(":") != string::npos) {
		cout << "Invalid server IP format" << endl;
		return false;
	}

	_mainSocket = new TCPSocket(ip, MSNGR_PORT);
	start();

	return isConnected();
}

/**
 * return true if connected to the server
 */
bool TCPMessengerClient::isConnected() {
	return _mainSocket != NULL;
}

/**
 * disconnect from messenger server
 */
bool TCPMessengerClient::disconnect() {
	if (_isRunning) {
		if (isActiveClientSession()) {
			closeActiveSession();
		}

		sendCommand(EXIT);

		_mainSocket->cclose();
		delete _mainSocket;
		_mainSocket = NULL;
	}

	_isRunning = false;
	this->waitForThread();

	return true;
}

/**
 * open session with the given peer address (ip:port)
 */
bool TCPMessengerClient::open(string address) {
	if (isActiveClientSession()) {
		closeActiveSession();
	}

	if (!isConnected()) {
		cout << "Not connected to server" << endl;
		return false;
	}

	cout << "Opening session with: " << address << endl;

	_isActiveSession = sendCommand(OPEN_SESSION_WITH_PEER) && sendData(address);

	return isActiveClientSession();
}

/**
 * return true if a session is active
 */
bool TCPMessengerClient::isActiveClientSession() {
	return _isActiveSession;
}

/**
 * close active session
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
 * Function that helps send the request to print all the users
 */
void TCPMessengerClient::printAllUsers(){
	if(this->state == NOT_CONNECTED) //print all users ONLY when connected
		cout<<"You are not connected"<<endl;
	else
		this->TCPtoServerCommandProtocol(REG_USERS);
}

/*
 * Function that helps send the request to print all of the connected users
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
 * Helper function that sends messages to server
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
 * Helper function that sends commands to the server
 */
void TCPMessengerClient::TCPtoServerCommandProtocol(int protocol)
{
	int command = htonl(protocol);
	_mainSocket->send((char*)&command,4);

}

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
