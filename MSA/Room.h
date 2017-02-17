#ifndef ROOM_H_
#define ROOM_H_

using namespace std;

#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>

class Room{
	vector<string> _usersInRoom; //IP and port
	string _roomName;
	string _hostName;

public:
	Room(string roomName,string hostIp_Port,string hostName);
	virtual ~Room();

	string getRoomName();
	string getHostName();
	vector<string> getUsersInRoom();

	void addUserToRoom(string user);
	void removeUserFromRoom(string user);
};

#endif /* ROOM_H_ */
