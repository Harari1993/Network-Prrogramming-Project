// Name: main.cpp
// Authors: Danielle Cohen 311240287 & Amit Harari 311328892

#include <iostream>
#include "TCPMessengerServer.h"

using namespace std;

void printMenu()
{
	cout<<"Server Command List:"<<endl;
	cout << "To get list of all Users type:       lu" << endl;
	cout << "To get list of Connected Users type: lcu" << endl;
	cout << "To get list of Sessions type:        ls" << endl;
	cout << "To get list of rooms type:           lr" << endl;
	cout << "To get list of Users in Room type:   lru<roomName>" << endl;
	cout << "To close the app type:               x" << endl;
}

int main(){

		cout<<"Welcome to our messenger Server!"<<endl;
		printMenu();

		TCPMessengerServer* server = new TCPMessengerServer();

		while (true) {
			string command;
			cin >> command;
			if(command == "lu"){
//				server->printAllUsers();;
			} else if(command == "lcu"){
//				server->printAllPeers();
			} else if (command == "ls"){
//				server->printAllSessions();
			} else if (command == "lr"){
//				server->printAllRooms();
			} else if (command == "lru"){
				string roomname;
				cin>>roomname;
//				server->printUserInRoom(roomname);
			} else if (command == "x"){
//				server->notifyShutdown();
				system("sleep 1");
				break;
			} else {
				cout<<"wrong input"<<endl;
				printMenu();
			}

//			server->close();
			delete server;
			cout<<"messenger was closed"<<endl;
			return 0;
		}
    // TODO:
    // 1. Implement the main loop of the Server
    // 2. Create a TCPMessengerServer 
    // 3. Create a method for each command in the TCPMessengerServer
    //djojskjksk
    //sjklsjlsld
}
