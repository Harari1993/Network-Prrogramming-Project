/*
 * main.cpp
 *
 *  Created on: Feb 6, 2013
 *      Author: Eliav Menachi
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <iostream>
#include <string.h>
#include "TCPMessengerClient.h"

using namespace std;

void printInstructions() {
	cout << "Welcome to TCP messenger" << endl;
	cout << "To open connect to server type: 						 c <server ip>" << endl;
	cout << "To print the user list from the server type: 			 lu" << endl;
	cout << "To print the connected users list type: 				 lcu" << endl;
	cout << "To print all rooms type: 								 lr" << endl;
	cout << "To print all users in this room type: 					 lru <room name>" << endl;
	cout <<	"To login with the user and password type: 				 login <user> <password>" << endl;
	cout << "To register new user with given password type:          register <user> <password>" << endl;
	cout << "To open a session with the user type: 					 o <username>" << endl;
	cout << "To enter a chat room type:                              or <room name>" << endl;
	cout << "Opening session will close any previously active sessions" << endl;
	cout << "After the session is opened, to send message type type: s <message>" << endl;
	cout << "To print the current status of the client type:         l" << endl;
	cout << "To close opened session type:                           cs" << endl;
	cout << "To close a room type:                                   cr <room name>" << endl;
	cout << "To disconnect from server type:                         d" << endl;
	cout << "To close the app type:                                  x" << endl;
}


int main() {
	printInstructions();

	TCPMessengerClient* messenger = new TCPMessengerClient();

	while (true) {
		string msg;
		string command;
		cin >> command;
		if (command == "c") { 					// open connect to server type
			string ip;
			cin >> ip;
			messenger->connect(ip);
		} else if (command == "lu") { 			// print the user list from the server
			messenger->printAllUsers();
		} else if (command == "lcu") { 			// print the connected users list
			messenger->printConnectedUsers();
		} else if(command == "lr") { 			// print all rooms
			messenger->printAllRooms();
		} else if(command == "lru") {           // print all users in this room
			string roomName; 
			cin>>roomName;
			messenger->printAllUsersInRoom(roomName);
		} else if(command == "login") {         // login with the user and password
			string userName;
			string password;
			cin>>userName;
			cin>>password;
			messenger->loginUser(userName,password);
		} else if(command == "register") {      // register new user with given password
			string userName;
			string password;
			cin>>userName;
			cin>>password;
			messenger->registerUser(userName,password);
		} else if (command == "o") {			// open a session with the user
			string userName;
			cin >> userName;
			if (!messenger->open(userName)) {
				cout << "Failed opening session with client " << userName << endl;
			}
		} else if (command == "or") {			// open a session with room
			string roomName;
			cin >> roomName;
			if (!messenger->open(roomName)) {
				cout << "Failed opening session with room " << roomName << endl;
			}
		} else if (command == "s") {           // send message 
			cin >> msg;

			if (msg.size() > 0 && msg[0] == ' '){
				msg.erase(0, 1);
			}

			if (!messenger->isActiveClientSession()) {
				cout << "Fail: No client session is active" << endl;
			} else {
				messenger->send(msg);
			}
		} else if (command == "l"){           // print the current status of the client
			messenger->printClientStatus();
		} else if (command == "cs") {         // close opened session
			messenger->closeActiveSession();
		} else if (command == "cr") {         // close a room
			string roomName;
			cin >> roomName;
			messenger->closeRoom(roomName);
		} else if (command == "d") {		  // disconnect from server
			messenger->disconnect();
		} else if (command == "x") {          // close the app
			break;

		} else {
			cout << "wrong input" << endl;
			printInstructions();
		}
	}
	messenger->disconnect();
	delete messenger;
	cout << "messenger was closed" << endl;
	return 0;
}
#endif /* MAIN_CPP_ */
