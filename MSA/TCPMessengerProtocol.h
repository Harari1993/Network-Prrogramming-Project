/*
 * TCPMessengerProtocol.h
 *
 *  Created on: Feb 13, 2013
 *      Author: efi
 */

#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
//3346
#define MSNGR_PORT                  3346

#define CLOSE_SESSION_WITH_PEER 	1
#define OPEN_SESSION_WITH_PEER 		2
#define EXIT						3
#define SEND_MSG_TO_PEER			4
#define SESSION_REFUSED				5
#define SESSION_ESTABLISHED			6
#define CONNECTED              		7
#define NOT_CONNECTED          		8
#define LOGGED_IN                   9
#define IN_SESSION                  10
#define IN_ROOM                     11
#define USER_LOGIN_REQUEST      	12
#define CREATE_NEW_USER  			13
#define NEW_USER_APPROVED			14
#define NEW_USER_DENIED				15
#define REQUEST_USER_LOGIN      	16
#define LOGIN_ERROR_RESPONSE        17
#define LOGIN_APPROVE_RESPONSE      18
#define REG_USERS 					19
#define CONNECTED_USERS				20
#define USERS_IN_ROOM				21
#define EXISTED_ROOMS				22
#define CREATE_NEW_ROOM 			23
#define ROOM_NAME_EXISTS            24
#define CREATE_ROOM_APPROVED		25
#define NO_SUCH_ROOM_NAME			26
#define JOIN_ROOM                   27
#define JOIN_ROOM_ARPROVED          28
#define ROOM_STATUS_CHANGED         29
#define LEAVE_ROOM                  30
#define CLOSE_ROOM_REQUEST			31
#define ROOM_CLOSED					32
#define CLOSE_ROOM_DENIED			33
#define PRINT_DATA_FROM_SERVER		34
#define DISCONNECT                  35
#define SERVER_DISCONNECT			36
#define SESSION_REFUSED_ROOM		37
#define SESSION_REFUSED_SESSION		38
#define NO_ROOMS					39
#define LEFT_ROOM					40
#define NO_USERS					41







#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"



#endif /* TCPMESSENGERPROTOCOL_H_ */
