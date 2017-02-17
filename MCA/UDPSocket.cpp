/*
 * UDPSocket.cpp
 *
 *  Created on: Feb 8, 2013
 *      Author: Eliav Menachi
 */

#include "UDPSocket.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

UDPSocket::UDPSocket(int port){
	// creates a UDP socket, AF_INET - IPv4 Internet protocols
	// SOCK_DGRAM - UDP, 0 - default protocol type fo UDP
	socket_fd = socket (AF_INET, SOCK_DGRAM, 0);
	if(port != 9999){
		struct sockaddr_in  s_in;
		bzero((char *) &s_in, sizeof(s_in));
		s_in.sin_family = (short)AF_INET;
		s_in.sin_addr.s_addr = htonl(INADDR_ANY);    /* WILDCARD any system ip*/
		s_in.sin_port = htons(port);

		//bind the socket on the specified address
		if (bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in))<0){
			cout<<"Error naming channel"<<endl;
		}
	}
}

int UDPSocket::recv(char* buffer, int length){
	printf("UDP server receive ...\n");
	socklen_t fromSize = sizeof(from);
	int rc = recvfrom(socket_fd, buffer, length, 0,(sockaddr*)&from,&fromSize);
	return rc;
}


int UDPSocket::sendTo(const string& msg,const string& ip, int port){
	struct sockaddr_in  s_in;
	bzero((char *) &s_in, sizeof(s_in));
	s_in.sin_family = (short)AF_INET;
	s_in.sin_addr.s_addr = inet_addr(ip.c_str());
	s_in.sin_port = htons(port);

	int rc = sendto(socket_fd,msg.c_str(), msg.length(),0,(struct sockaddr *)&s_in,sizeof(s_in));
	return rc;
}

int UDPSocket::reply(const string& msg){
	cout<<"reply "<<msg<<" "<< endl;
	socklen_t fromSize = sizeof(from);
	int rc = sendto(socket_fd,msg.c_str(), msg.length(),0,(struct sockaddr *)&from,fromSize);
	return rc;
}

void UDPSocket::cclose(){
	cout<<"closing socket"<<endl;
	shutdown(socket_fd,SHUT_RDWR);
	::close(socket_fd);
}

string UDPSocket::fromAddr(){
	return inet_ntoa(from.sin_addr);

}



