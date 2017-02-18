#include "Room.h"

/*
 * A constructor for a room, stores the name of the room, the hosts ip and port and the name of the host
 */
Room::Room(string roomName,string hostIp_Port,string roomHostName) {
	this->_roomName=roomName;
	//hostName=roomHostName;
	//this->hostIp_Port=hostIp_Port;
	//AddUserToRoom(this->hostIp_Port);
	cout<<"The Room ["<<roomName<<"] Has been Created"<<endl;
}

Room::~Room(){};

void Room::addUserToRoom(string user){
	_usersInRoom.push_back(user);
}

void Room::removeUserFromRoom(string user){
	for (int i = 0; i < _usersInRoom.size(); i++){
		if (_usersInRoom.at(i) == user){
			_usersInRoom.erase(_usersInRoom.begin() + i);
			break;
		}
	}
}
