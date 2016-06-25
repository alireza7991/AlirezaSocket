#include <iostream>
#include <conio.h>
#include <string>
#include "AlirezaSocket.h"
#include <Windows.h>

using namespace std;

class test : public AlirezaSocket {
	void onConnect(SOCKET socket) {
		sendString(socket,"What is your name?");
		cout << recieveString(socket) << endl;
		cout << recieveString(socket) << endl;
		sendString(socket,"my name is ali");
	}
};

int main(int argc, char *argv[])
{
	try {
		test s;
		s.Connect("127.0.9.1",7991);
	} catch(std::runtime_error& e) {
		cout << e.what() << endl;
	}
	_getch();
}