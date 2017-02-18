#include "Login.h"
#include <iostream>
#include <fstream>



Login::Login(TCPMessengerServer* server){
	this->_server = server;
	_listener = NULL;
	_isOn = true;
}

Login::~Login(){};

void Login::run(){
	cout<< "Login"<<endl;

	while (_isOn) {
//		string buffer;
		//initialize multiple socket listener
		if (_listener != NULL){
			delete _listener;
		}

		_listener = new MultipleTCPSocketsListener();
		_listener->addSockets(_server->_openPeerVector);

		//Get the socket for a user
		TCPSocket* currentUser = _listener->listenToSocket(2);
		if(currentUser==NULL) {
			continue;
		}

		//we handle the current socket by ip and port
		string currentConnectedIPandPort = currentUser->getIpAndPort();
		cout << "Incoming login req "<< currentConnectedIPandPort << endl;

		//makes the sockets wait for incoming messages and parses commands if sent
		switch(_server->recieveCommandFromTCP(currentUser))
		{
			case USER_LOGIN_REQUEST:
			{
				cout << "in USER_LOGIN_REQUEST"<< endl;
				userLogin(currentUser);
				break;

			}
			case CREATE_NEW_USER:
			{
				cout << "in CREATE_NEW_USER"<< endl;
				createNewUser(currentUser);
				break;
			}
			case DISCONNECT:
			{
				cout << "in DISCONNECT"<< endl;
				disconnect(currentUser);
				break;
			}
			default:{
				cout << "no macthing" << endl;
			}
		}
	}
}

void Login::userLogin(TCPSocket* user){
	char* newUser = _server->recieveMessageFromTCP(user);

	//parsing the messages for login
	string userName = strtok(newUser," ");
	string password = strtok(NULL," ");
	cout << "Trying to connect user: " << userName << endl;

	//tempAuthentication used to assure the user can connect
	int tempAuthentication = login(userName,password);

	if(tempAuthentication==0){
		bool isAlreadyConnected=false;
		string tempName;
		//checking if the user is already connected
		for(unsigned int i =0 ; i< _server->_openPeerVector.size(); i++) {
			cout << "open sock: " << i << endl;
			tempName = _server->ipToName(_server->_openPeerVector.at(i)->getIpAndPort());
			if(userName==tempName) {
				isAlreadyConnected=true;
				cout << "user is already connected: " << userName << endl;
				break;
			}
		}

		if(!isAlreadyConnected){

			cout<<"User: "<<userName<<" Is Connected to server"<<endl;

			//we insert the newly connected user to a vector so that we can remember he is connected
			_server->_openPeerVector.push_back(user);
			string tempIptoName=userName+" "+user->getIpAndPort();

			//We store the details of the user, with a name
			_server->getIpToClientName().push_back(tempIptoName);

			//Informs the client that the login has been approved, get ready for UDPManager settings
			_server->sendCommandToTCP(LOGIN_APPROVE_RESPONSE,user);//LOGIN_RESPONSE_APPROVE
			_server->sendMsgToTCP(user->getIpAndPort(),user);//Sends UDPManager settings

			int indexUser = _server->getSocketIndex(_server->_openPeerVector,user->getIpAndPort());
			_server->_openPeerVector.erase(_server->_openPeerVector.begin() + indexUser);
		}
		else
		{
			//Informs the client that the login has failed
			_server->sendCommandToTCP(LOGIN_ERROR_RESPONSE,user);//LOGIN_RESPONSE_ERROR
		}
	}
	else if(tempAuthentication==1)
	{
		//Fail to login, userName was not found in the database
		cout<<"the user name: "<<userName<<" does not exist"<<endl;
		_server->sendCommandToTCP(LOGIN_ERROR_RESPONSE,user);//LOGIN_RESPONSE_ERROR
	}
	else{
		//Fail to login, password does not match the userName
		cout<<"the password: "<<password<<" is not correct for the user: "<<userName<<endl;
		_server->sendCommandToTCP(LOGIN_ERROR_RESPONSE,user);//LOGIN_RESPONSE_ERROR
	}
}

int Login::login(string userName, string password)
{
	ifstream usersFile;
	ifstream passwordsFile;
	string userNameBuffer[256];
	string passwordBuffer[256];
	int location;
	int numOfUsers=0;
	int numOfPasswords=0;

	usersFile.open("users.txt");
	passwordsFile.open("passwords.txt");

	//Extracts all user names from a text to a strings array and stores the size of it in numOfUsers
	if (usersFile.is_open()) {
		while (!usersFile.eof()) {
			usersFile >> userNameBuffer[numOfUsers];
			numOfUsers++;
		}
	}
	//Extracts all passwords from a text to a strings array and stores the size of it in numOfPasswords
	if (passwordsFile.is_open()) {
		while (!passwordsFile.eof()) {
			passwordsFile >> passwordBuffer[numOfPasswords];
			numOfPasswords++;
		}
	}
	//Checks if the userName exists in the users' database
	for(location=0;location<numOfUsers;location++)
	{
		if (strcmp(userName.c_str(), userNameBuffer[location].c_str()) ==0){
			{
				break;
			}
		}
	}
	//userName does not appear in database, meaning we broke the loop and location didnt iterate all the way to numOfUsers
	if(location==numOfUsers)
	{
		usersFile.close();
		passwordsFile.close();
		return 1;
	}
	//Checks if the password suits the userName's password (by the userName's location)
	if (strcmp(password.c_str(), passwordBuffer[location].c_str()) ==0)
	{
		usersFile.close();
		passwordsFile.close();
		return 0;
	}
	//The given password does not match the userName's password
	else
	{
		usersFile.close();
		passwordsFile.close();
		return 2;
	}
}

void Login::createNewUser(TCPSocket* user){
	//Starts the creation process for a new user
	string newUser = _server->recieveMessageFromTCP(user);
	cout << "Trying to create new user: " << newUser << endl;

	//Checks if that user already exists
	int status = Register(newUser);

	if(status !=1 ){
		//User name already exists, send failure message to the client
		_server->sendCommandToTCP(NEW_USER_DENIED,user);
		cout << "failed to register" << endl;
		_server->sendMsgToTCP(user->getIpAndPort(),user);
	}
	else {
		//User name approved to be a new one and added to the database
		cout << newUser << " has been Registered and Connected to server" <<endl;

		// copy the string
		char* tempUserAndPass = strdup(newUser.c_str());

		// splite strings
		string userName = strtok(tempUserAndPass," ");
		string password = strtok(NULL, " ");


		//we insert the newly connected user to a vector so that we can remember he is connected
		_server->_openPeerVector.push_back(user);
		string tempIptoName=userName+" "+user->getIpAndPort();

		//We store the details of the user, with a name
		_server->getIpToClientName().push_back(tempIptoName);

		//Informs the client that the login has been approved, get ready for UDPManager settings
		_server->sendCommandToTCP(LOGIN_APPROVE_RESPONSE, user);//LOGIN_RESPONSE_APPROVE
		_server->sendMsgToTCP(user->getIpAndPort(),user);//Sends UDPManager settings

		int indexUser = _server->getSocketIndex(_server->_openPeerVector,user->getIpAndPort());
		_server->_openPeerVector.erase(_server->_openPeerVector.begin() + indexUser);

	}
}

void Login::disconnect(TCPSocket* user){
	// Get the user index
	int indexUser = _server->getSocketIndex(_server->_openPeerVector,user->getIpAndPort());

	// Remove the item in the user index
	_server->_openPeerVector.erase(_server->_openPeerVector.begin() + indexUser);
}

int Login::Register(string user){
	// copy string
	char* tempUserAndPass = strdup(user.c_str());

	// splite strings
	string userName = strtok(tempUserAndPass," ");
	string password = strtok(NULL, " ");

	//Extracts all user names from a text to a strings array and checks if a given userName is already exists.
	ifstream usersFile;
	usersFile.open("users.txt");

	string userNameBuffer[256];
	int location;
	int numOfUsers=0;

	if (usersFile.is_open()) {

		while (!usersFile.eof()) {
			usersFile >> userNameBuffer[numOfUsers];
			numOfUsers++;
		}
	}

	for(location=0;location<numOfUsers;location++)
	{
		// Check if the user is already exsit
		if (strcmp(userName.c_str(), userNameBuffer[location].c_str()) ==0)
		{
			usersFile.close();
			return 0;
		}
	}
	usersFile.close();

	//Registers the new userName to the users' text file
	ofstream myWriteFile;
	myWriteFile.open("users.txt",ofstream::app);
	myWriteFile<<userName<<endl;
	myWriteFile.close();
	//Registers the new user name's password to the passwords' text file
	myWriteFile.open("passwords.txt",ofstream::app);
	myWriteFile<<password<<endl;
	myWriteFile.close();

	free(tempUserAndPass);
	return 1;
}
