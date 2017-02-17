//============================================================================
// Name        : UDPManager
// Author      : Eliav Menachi
// Version     :
// Copyright   : Your copyright notice
// Description : UDP Messenger application
//============================================================================

#include "UDPManager.h"
#include "TCPMessengerProtocol.h"

using namespace std;

void UDPManager::run(){
	// receiver thread...
	while(running){
		char buff[1024];
		int rc = udpSocket->recv(buff,sizeof(buff));
		if (rc < 0){
			break;
		}
		buff[rc] = '\0';
		cout<<endl<<udpSocket->fromAddr()<<">"<<buff<<endl;
	}
	udpSocket->cclose();
}

UDPManager::UDPManager(string myUserName,string myIpandPort){
	this->myUserName = myUserName;
	char* port = strdup(myIpandPort.c_str());
	string tempPort = strtok(port,":");
	tempPort = strtok(NULL,":");
	udpSocket = new UDPSocket(atoi(tempPort.c_str()));
	running=true;
}

/*
 * Sending a message to a peer
 */
void UDPManager::sendToPeer(string msg) {
	 string finalmsg = "["+myUserName+"]"+" " + msg;
	 udpSocket->sendTo(finalmsg,destIp,atoi(destPort.c_str()));
}

void UDPManager::reply(const string& msg){
	cout<<"reply to "<<udpSocket->fromAddr()<<endl;
	udpSocket->reply(msg);
}

void UDPManager::close(){
	running = false;
	udpSocket->cclose();
	waitForThread();
	delete udpSocket;
	udpSocket = NULL;
}

/*
 * Set the destination
 */
void UDPManager::setDestinationMessage(string dest){

}

void UDPManager::sendToRoom(string msg) {
	//Looping on all users in the my room
	for (unsigned int i=0; i < listOfUsersInRoom.size(); i++) {
		//Gets IP:Port for each user from listOfUsersInRoom
		string tempdest = listOfUsersInRoom.at(i);
		//Initializes the destIp & destPort data members
		setDestinationMessage(tempdest);
		//Message to user (UDP)
		this->sendToPeer(msg);
	}
}
