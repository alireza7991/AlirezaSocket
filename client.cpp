#include <iostream>
#include <conio.h>
#include <string>
#include "AlirezaSocket.h"
#include <Windows.h>

using namespace std;

class test : public AlirezaSocket {
	void onConnect(SOCKET s) {
		cout << "Conected to the server" << endl;
		cout << "Enter username and password\n";
		string username,password;
		cin >> username>> password; 
		sendString(s,username);
		sendString(s,password);
		cout << recieveString(s) << endl;
	}

};

int main(int argc, char *argv[])
{
	try {
		test s;
		s.Connect("129.0.0.1",7991);
	} catch(std::runtime_error& e) {
		cout << e.what() << endl;
	}
	_getch();
}