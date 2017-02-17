/*
 * TCPSocket.h
 *
 *  Created on: Nov 18, 2015
 *      Author: Ben Grynhaus & Liel Cohen
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

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

using namespace std;

class TCPSocket {
	//TODO: declare class properties

private:
	int _socket_fd; //fd will typically be the socket file descriptor of a network connection
	sockaddr_in _serverAddr;
	sockaddr_in _peerAddr;

	/**
	 * private constructor to create a secondary server socket to communicate with a remote peer
	 */
	TCPSocket(int connected_sock, struct sockaddr_in serverAddr,
			struct sockaddr_in peerAddr);

public:
	/**
	 * Constructor create a TCP server socket
	 */
	TCPSocket(int port);

	/**
	 * Constructor creates TCP client socket
	 */
	TCPSocket(string peerIp, int port);

	/*
	 * Get the socket
	 */
	int getSocketFd();


	/**
	 * Perform listen and accept on server socket
	 */
	TCPSocket* listenAndAccept();

	/**
	 * Read from socket into the given buffer up to the buffer given length.
	 * return the number of bytes read
	 */
	int recv(char* buffer, int length);

	/**
	 * send the given buffer to the socket
	 */
	int send(const char* msg, int len);

	/**
	 * close the socket and free all resources
	 */
	void cclose();

	/**
	 * return the address of the connected peer
	 */
	string fromAddr();

	/*
	 * Return ip and port concatenate
	 */
	string getIpAndPort();
};

#endif
