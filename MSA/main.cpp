// Name: main.cpp
// Authors: Danielle Cohen 311240287 & Amit Harari 311328892

#include <iostream>
#include "TCPMessengerServer.h"

using namespace std;

void printMenu()
{
	cout<<"Server Command List:"<<endl;
	cout<<"==============================================="<<endl;
	cout<<"List of Registered Users:         lu"<<endl;
	cout<<"List of Connected Users:          lcu"<<endl;
	cout<<"List Rooms:                       lr"<<endl;
	cout<<"List of Users in Rooms:           lru <name>"<<endl;
	cout<<"List of Sessions:                 ls"<<endl;
	cout<<"Exit:                             x"<<endl;
	cout<<"==============================================="<<endl;
}

int main(){
    cout<<"Welcome to our messenger Server!"<<endl;
    printMenu();

    // TODO:
    // 1. Implement the main loop of the Server
    // 2. Create a TCPMessengerServer 
    // 3. Create a method for each command in the TCPMessengerServer
}