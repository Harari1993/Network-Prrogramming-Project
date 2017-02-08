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
	cout << "To open connect to server type: c <server ip>" << endl;
	cout << "To open session type with peer: o <peer ip:port>" << endl;
	cout << "Opening session will close any previously active sessions" << endl;
	cout << "After the session is opened, to send message type: s <message>" << endl;
	cout << "To close opened session type: cs" << endl;
	cout << "To disconnect from server type: d" << endl;
	cout << "To exit type: x" << endl;
}


int main() {
	printInstructions();

	TCPMessengerClient* messenger = new TCPMessengerClient();

	while (true) {
		string msg;
		string command;
		cin >> command;
		if (command == "c") {
			string ip;
			cin >> ip;
			messenger->connect(ip);

		} else if (command == "o") {
			string ip;
			cin >> ip;
			if (!messenger->open(ip)) {
				cout << "Failed opening session with client " << ip << endl;
			}

		} else if (command == "s") {
			cin >> msg;

			if (msg.size() > 0 && msg[0] == ' '){
				msg.erase(0, 1);
			}

			if (!messenger->isActiveClientSession()) {
				cout << "Fail: No client session is active" << endl;
			} else {
				messenger->send(msg);
			}

		} else if (command == "cs") {
			messenger->closeActiveSession();

		} else if (command == "d") {
			messenger->disconnect();

		} else if (command == "x") {
			break;

		} else {
			cout << "wrong input" << endl;
		}
	}
	messenger->disconnect();
	delete messenger;
	cout << "messenger was closed" << endl;
	return 0;
}
#endif /* MAIN_CPP_ */