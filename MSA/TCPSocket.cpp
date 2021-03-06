/*
 * TCPSocket.cpp
 */

#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include "TCPSocket.h"

using namespace std;

TCPSocket::TCPSocket(int connected_sock, struct sockaddr_in serverAddr,
		struct sockaddr_in peerAddr) {
	//initialize local class properties
	_socket_fd = connected_sock;
	_serverAddr = serverAddr;
	_peerAddr = peerAddr;
}

TCPSocket::TCPSocket(int port) {
	// open TCP socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd < 0) {
		perror("Error opening channel");
	}

	// set the server address for binding
	sockaddr_in serv_name;
	bzero(&serv_name, sizeof(serv_name));
	serv_name.sin_family = AF_INET;
	serv_name.sin_port = htons(port);

	// bind the socket to the address
	if (bind(_socket_fd, (struct sockaddr *) &serv_name, sizeof(serv_name)) < 0) {
		perror("Error naming channel");
	}

	cout << "Server is alive and waiting for socket connection from client."
			<< endl;
}

TCPSocket::TCPSocket(string peerIp, int port) {
	// open TCP socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd < 0) {
		perror("Error opening channel");
	}

	// set the peer address to connect to
	bzero(&_peerAddr, sizeof(_peerAddr));
	_peerAddr.sin_family = AF_INET;
	_peerAddr.sin_addr.s_addr = inet_addr(peerIp.c_str());
	_peerAddr.sin_port = htons(port);

	// connect the socket to the peer server
	if (connect(_socket_fd, (sockaddr *) &_peerAddr, sizeof(_peerAddr)) < 0) {
		perror("Error establishing communications");
		close(_socket_fd);
	}
}

int TCPSocket::getSocketFd(){
	return _socket_fd;
}

TCPSocket* TCPSocket::listenAndAccept() {
	// listen on the socket
	if (listen(_socket_fd, 1) < 0) {
		return NULL;
	}

	// call accept on the socket
	size_t len = sizeof(_peerAddr);
	int connect_sock = accept(_socket_fd, (sockaddr *) &_peerAddr,
			(socklen_t*) &len);

	// return new TCPSocket object holding the new secondary server socket returned from the accept call
	return new TCPSocket(connect_sock, _serverAddr, _peerAddr);
}

int TCPSocket::recv(char* buffer, int length) {
	// read from the socket
	int byteRead = read(_socket_fd, buffer, length);
	cout << "byte " << byteRead << endl;
	return byteRead;
}

int TCPSocket::send(const char* msg, int len) {
	int byteWrite = write(_socket_fd, msg, len);

	return byteWrite;
}

void TCPSocket::cclose() {
	// shutdown and close the socket
	shutdown(_socket_fd, 0);
	close(_socket_fd);
}

string TCPSocket::fromAddr() {
	// return the session destination peer address
	char* lastAddr = inet_ntoa(_peerAddr.sin_addr);
	return string(lastAddr);
}

string TCPSocket::getIpAndPort(){
	// Convert IP addresses to string
	string str = inet_ntoa(_peerAddr.sin_addr);

	str.append(";");
	char buff[10];

	// Convert port to string
	sprintf(buff,"%d",ntohs(_peerAddr.sin_port));
	str.append(buff);

	return str;
}
