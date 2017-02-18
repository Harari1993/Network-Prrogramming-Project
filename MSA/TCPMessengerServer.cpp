#include "TCPMessengerServer.h"
#include "TCPMessengerProtocol.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

/*
 *Basic constructor, initializes the dispatcher and creates the socket for the server
 */
TCPMessengerServer::TCPMessengerServer(){
	//posInVec=0; //<----
	_serverSocket = new TCPSocket(MSNGR_PORT);
	_dispatcher = new Dispatcher(this);
	_login = new Login(this);
	this->start();
}

TCPMessengerServer::~TCPMessengerServer(){};

/*
 * The servers main loop
 */
void TCPMessengerServer::run(){

	//Starts the dispatcher
	_dispatcher->start();
	_login->start();

	cout<< "TCPMessengerServer"<<endl;
	while (true) {
		//Waits for incoming clients connection
		TCPSocket* tmpTCP= _serverSocket->listenAndAccept();
		if (tmpTCP != NULL) {
			_openPeerVector.push_back(tmpTCP);
		}
	}
}
///*
// *This forwards messages to all the users in a room
// */
void TCPMessengerServer::sendMsgToAllUsersInRoom(int msgType,string roomName, string userName)
{
	int userIndex;
	int roomIndex = this->getRoomIndex(roomName);
	//This loop is runs on all users in the room and informs if a user has joined or left the room
	for(unsigned int i=0;i<this->_rooms.at(roomIndex)->_usersInRoom.size();i++)
	{
		string tempIPtosend = _rooms.at(roomIndex)->_usersInRoom.at(i);
		userIndex=this->getSocketIndex(_openPeerVector, tempIPtosend);
		this->sendCommandToTCP(ROOM_STATUS_CHANGED,this->_openPeerVector.at(userIndex));
		switch(msgType)
		{
			case JOIN_ROOM:
			{
				//Informs all users about the new user who joined the room
				string tempMsg = userName;
				tempMsg.append(" Has Joined the room");

				int numOfUsers = _rooms.at(roomIndex)->_usersInRoom.size();
				this->sendMsgToTCP(tempMsg,this->_openPeerVector.at(userIndex));
				this->sendCommandToTCP(numOfUsers,this->_openPeerVector.at(userIndex));
				string usersVectorString = this->usersInRoomToString(roomName);

				//Sends the updated users list to all peers in the roomName
				this->sendMsgToTCP(usersVectorString,this->_openPeerVector.at(userIndex));
				break;
			}
			case LEAVE_ROOM:
			{
				//Informs all users about the user that left
				string tempMsg = userName;
				tempMsg.append(" Has Left the room");
				int numOfUsers =_rooms.at(roomIndex)->_usersInRoom.size();
				this->sendMsgToTCP(tempMsg,this->_openPeerVector.at(userIndex));
				this->sendCommandToTCP(numOfUsers,this->_openPeerVector.at(userIndex));
				string usersVectorString = this->usersInRoomToString(roomName);

				//Sends the updated users list to all peers in the roomName
				this->sendMsgToTCP(usersVectorString,this->_openPeerVector.at(userIndex));
				break;
			}
		}
	}
}

/*
 * Simple function that returns the names of the users in the room in a string
 */
string TCPMessengerServer::usersInRoomToString(string roomName)
{
	string tempIp;
	int roomIndex = this->getRoomIndex(roomName);
	for(unsigned int i=0;i<this->_rooms.at(roomIndex)->_usersInRoom.size();i++)
	{
		tempIp.append(this->_rooms.at(roomIndex)->_usersInRoom.at(i));
		if(i!=this->_rooms.at(roomIndex)->_usersInRoom.size()-1)
			tempIp.append(" ");
	}

	return tempIp;
}
//
//
//void TCPMessengerServer::PrintOpenPeerVector()
//{
//	for(unsigned int i=0;i<this->openPeerVect.size();i++)
//	{
//		cout<<"PrintOpenPeerVector peer : "<<i<<" "<<openPeerVect.at(i)->destIpAndPort()<<endl;
//	}
//}


/*
 * prints the peers
 */
void TCPMessengerServer::printAllPeers(){
	if(_openPeerVector.size()==0)
	{
		cout<<"no peers connected"<<endl;
	}
	else
	{
		if(_openPeerVector.size()!=0)
		{
			for(unsigned int i = 0;i<_openPeerVector.size();i++)
			{
				if(_openPeerVector.at(i)!=NULL)
				{
					string tmp = _ipToClientName.at(i);
					cout << tmp <<endl;
				}
			}
		}
	}
}

void TCPMessengerServer::printAllRooms(){
	if(_rooms.size()>0)
	{
		for(unsigned int i = 0 ; i<_rooms.size();i++)
			cout<<i+1<<"."<<_rooms.at(i)->_roomName<<endl;
		}
	else
	{
		cout<<"No Opened Rooms."<<endl;
	}
}
/*
 * Simply prints all the users from the file
 */
void TCPMessengerServer::printAllUsers()
{
	//GetUserNamesFromData actually returns a vector from the file
	vector<string> tempVect = this->getUserNamesFromFile();
	int numOfUsers = tempVect.size();
	for(int i=0 ; i< numOfUsers ; i++)
	{
		cout<<i+1<<". "<<tempVect.at(i)<<endl;
	}
}

//void TCPMessengerServer::insertToOpenVector(TCPSocket* temp_soc)
//{
//	openPeerVect.push_back(temp_soc);
//}

/*
 * Sends a message on the tcp connection
 */
void TCPMessengerServer::sendCommandToTCP(int protocol,TCPSocket * tcp)
{
	// Convert from long to TCP/IP network byte
	protocol = htonl(protocol);
	tcp->send((char*)&protocol,4);
}

/*
 * Sends a message on the tcp connection
 */
void TCPMessengerServer::notifyShutdown()
{
	for(unsigned int i =0 ; i<_openPeerVector.size();i++)
	{
		sendCommandToTCP(SERVER_DISCONNECT,_openPeerVector.at(i));
	}
}


/*
 * Return the vector's index of the received address
 * Return -1 if not found
 */
int TCPMessengerServer::getSocketIndex(vector<TCPSocket*> vector, string address)
{
	unsigned int i = 0;

	while (i != vector.size()){
		if (address == vector.at(i)->getIpAndPort()){
			return i;
		}
	}
	return -1;
}

/*
 * Recieves the commands
 */
int TCPMessengerServer::recieveCommandFromTCP(TCPSocket * tmpTCP) {
	int command;
	tmpTCP->recv((char*)&command,4);

	//converts a u_long from host to TCP/IP network byte order
	command = htonl(command);
	cout << "command: " << command << endl;
	return command;
}

/*
 * Recieves messages
 */
char * TCPMessengerServer::recieveMessageFromTCP(TCPSocket * tmpTCP)
{
	int msgLen;
	char* buffer =new char[1024];

	// Initialize - copies n bytes, each with a value of zero, into string s
	bzero(buffer,1024);

	//Read from socket
	tmpTCP->recv((char *)&msgLen,4);

	//Converts the unsigned integer netlong from network byte order to host byte order.
	msgLen = ntohl(msgLen);

	tmpTCP->recv(buffer,msgLen);

	cout << "recive: " << buffer << endl;
	return buffer;
}

/*
 * Returns the ip of a certain user
 */
string TCPMessengerServer::nameToIp(string userName)
{
	for(unsigned int i=0; i<_ipToClientName.size();i++)
	{
		// Copy the string into temp
		char* temp = strdup(_ipToClientName[i].c_str());

		// Splite the temp string
		string user = strtok(temp," ");
		string ip = strtok(NULL, " ");

		free(temp);

		// If the user name found
		if(strcmp(user.c_str(),userName.c_str())==0)
		{
			return ip;
		}
	}

	return "UserName was not found";
}


/*
 * Lists all the sessions
 */
void  TCPMessengerServer::printAllSessions() {
	if (this->_initiatorSession.size() == 0) {
		cout << "There are no open sessions" << endl;
	} else {
		for(unsigned int i = 0; i < this->_initiatorSession.size(); i++) {
			cout << this->ipToName(_initiatorSession.at(i)) << " || " << this->ipToName(_wantedSession.at(i)) << endl;
		}
	}
}

/*
 * Sends messages
 */
void TCPMessengerServer::sendMsgToTCP(string msg, TCPSocket* tcp){
	//convert long to TCP/IP network byte
	int msglen = htonl(msg.length());

	tcp->send((char*)&msglen, 4);
	tcp->send(msg.c_str(), msg.length());
}

/*
 * Return the vector's index of the received room name
 * Return -1 if not found
 */
int TCPMessengerServer::getRoomIndex(string roomName)
{
	bool foundroom=false;
		unsigned int roomIndex;
		for(roomIndex=0;roomIndex<this->_rooms.size();roomIndex++) {
			if(roomName==this->_rooms.at(roomIndex)->_roomName) {
				foundroom=true;
				break;
			}
		}
		if(foundroom)
			return roomIndex;
		else
			return -1;
}


/*
 * Matches a given IP to a user, and returns it's name
 */
string TCPMessengerServer::ipToName(string ip){

	for(unsigned int i=0; i<_ipToClientName.size();i++)
	{
		// Copy the string into temp
		char* tempCharStar = strdup(_ipToClientName[i].c_str());

		// Splite the strings
		string user = strtok(tempCharStar," ");
		string ip = strtok(NULL, " ");

		free(tempCharStar);

		// If the ip found
		if(strcmp(ip.c_str(),ip.c_str())==0)
		{
			return user;
		}
	}

	return NULL;
}
/*
 * Prints all the users in a room
 */
void TCPMessengerServer::printUserInRoom(string roomname)
{
	int roomIndex = this->getRoomIndex(roomname);
	if(roomIndex!=-1)
	{
		for(unsigned int i=0;i<this->_rooms.at(roomIndex)->_usersInRoom.size();i++)
		{
			string tempname = this->ipToName(_rooms.at(roomIndex)->_usersInRoom.at(i));
			cout<<i+1<<"."<<tempname<<endl;
		}
	}
	else
	{
		cout<<"No such room"<<endl;
	}
}
void TCPMessengerServer::close(){

	_serverSocket->cclose();
	this->_openPeerVector.clear();
	this->_dispatcher->_isON = false;
	this->_login->_isOn = false;

	delete this->_dispatcher->_listener;
	delete _login->_listener;
}


/*
 * This function gets all the user names from the file and returns them in a vector
 */
vector<string> TCPMessengerServer::getUserNamesFromFile(){

	//Push all user names to a vector and returns it.
	ifstream userFile;
	userFile.open("users.txt");
	vector<string> userNameBuffer;
	string output;

	if (userFile.is_open())
	{
		while (userFile >> output)
		{
			userNameBuffer.push_back(output);

		}
	}

	userFile.close();
	return userNameBuffer;

}
