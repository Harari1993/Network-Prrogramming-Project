//============================================================================
// Name        : UDPMessenger
// Author      : Eliav Menachi
// Version     :
// Copyright   : Your copyright notice
// Description : UDP Messenger application
//============================================================================

#include "UDPMessenger.h"

using namespace std;
using namespace npl;

void UDPMessenger::run(){
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
	cout<<"closing receiver thread"<<endl;
}

UDPMessenger::UDPMessenger(){
	// init the messenger
	udpSocket = new UDPSocket(MSNGR_PORT);
	running = true;
	this->start();
}

void UDPMessenger::sendTo(const string& msg,const string& ip){
	udpSocket->sendTo(msg,ip,MSNGR_PORT);
}

void UDPMessenger::reply(const string& msg){
	cout<<"reply to "<<udpSocket->fromAddr()<<endl;
	udpSocket->reply(msg);
}

void UDPMessenger::close(){
	running = false;
	udpSocket->close();
	waitForThread();
	delete udpSocket;
	udpSocket = NULL;
}

