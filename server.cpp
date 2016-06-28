#include <iostream>
#include <string>
#include <conio.h>
#include "AlirezaSocket.h"
#include <Windows.h>

char username [5][100] = {
	"alireza",
	"alireza2",
	"alireza3",
	"alireza4",
	"alireza5",
};

char password [5][100] = {
	"1234",
	"4567",
	"1111",
	"7991",
	"0000",
};

class test : public AlirezaSocket {
	void onConnect(SOCKET s) {
		string uname = recieveString(s);
		string passwd = recieveString(s);
		for(int i=0;i<5;i++) {
			if(username[i]==uname && password[i]==passwd) {
				sendString(s,"Logged in successfully!");
				return;
			}
		}
		sendString(s,"Wrong username or password!");
	}
};

int main(int argc, char *argv[])
{
	try {
		test s;
		s.Listen(1);
	} catch(runtime_error e) {
		cout << e.what() << endl;
	}
	_getch();
}