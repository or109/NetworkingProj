#ifndef MAIN_CPP_
#define MAIN_CPP_

#include "UDPMessenger.h"

#include <stdio.h>
#include <string>

using namespace std;

void PrintMenu() {
	cout << "\n=== CLI COMMANDS PROMPT ===" << endl;
	cout << "1. Register:		register <user> <password>" << endl;
	cout << "2. Login:		login   <user> <password>" << endl;
	cout << "3. List all users:	l" << endl;
	cout << "4. Users details:	d <user> " << endl;
	cout << "5. Send message:	s <user> <message>" << endl;
	cout << "6. Exit:		x" << endl;
	cout << "=== CLI COMMANDS PROMPT ===\n" << endl;
}

class OnRecieveClbkImpl: public OnRecieveClbk {
	void handleMessage(string msg) {
		cout << "> " << msg << endl;
	}
};

int main() {
	PrintMenu();

	// Create the messenger object
	OnRecieveClbkImpl* clbk = new OnRecieveClbkImpl();
	UDPMessenger* messenger = new UDPMessenger(clbk);

	string msg;
	string command;
	string user;
	string password;

	// Read the input of the user and act accordingly
	while (true) {
		cin >> command;

		if (command == "register") {
			cin >> user;
			cin >> password;

			cout << messenger->registerin(user, password) << endl;
		} else if (command == "login") {
			cin >> user;
			cin >> password;

			// Check if user logged in
			if (messenger->usr->loggedin)
				cout << "You are already logged in." << endl;
			else
				cout << messenger->login(user, password) << endl;
		} else if (command == "l") {
			cout << messenger->getUserList() << endl;
		} else if (command == "d") {
			cin >> user;
			cout << messenger->getUserDetails(user) << endl;
		} else if (command == "s") {
			cin >> user;
			getline(std::cin, msg);
			if (msg.size() > 0 && msg[0] == ' ')
				msg.erase(0, 1);

			// Check if user is logged in
			if (messenger->usr->loggedin) {
				char data[300] = "", code[10] = "", ip[36] = "";
				int* port;
				sscanf(messenger->getUserDetails(user).data(), "%s %s", code,
						data);

				// Check if result is OK
				if (strcmp(code, "OK") == 0) {
					// Get IP and port and send message
					sscanf(data, "%[^:]:%d", ip, port);
					messenger->sendTo(messenger->usr->userName + " : " + msg,
							ip, *port);
				}
				else {
					cout << code << " " << data << endl;
				}
			}
			// User is not logged in
			else
				cout
						<< "ERROR - for sending messages you must first logging in."
						<< endl;

		} else if (command == "x") {
			break;
		} else {
			cout << "Invalid input" << endl;
			PrintMenu();
		}
	}

	// Close messenger application
	messenger->close();
	delete messenger;
	cout << "Bye bye :)" << endl;

	return 0;
}

#endif
