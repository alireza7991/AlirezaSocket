#include <iostream>
#include <string>
#include <conio.h>
#include "AlirezaSocket.h"
#include <Windows.h>
class test : public AlirezaSocket {
	void onConnect(SOCKET socket) {
		cout << "a client connected!" << endl;
		cout << recieveString(socket) << endl;
		sendString(socket,"my name is alireza");
		sendString(socket,"What about you?");
		cout << recieveString(socket) << endl;
	}
};

int main(int argc, char *argv[])
{
	test s;
	s.Listen(7991);
	_getch();
}