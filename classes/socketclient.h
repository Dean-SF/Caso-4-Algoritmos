#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include "winsock2.h"
#include "ws2tcpip.h"
#include <string.h>
#include <iostream>

#define PORT 4000
#define IP_ADDRESS "127.0.0.1"

using namespace std;

class socketclient {
	private: 
		int sock = 0, valread; 
		struct sockaddr_in serv_addr;
		WSAData wsaData;

	public:
		void init() {
			if(WSAStartup(MAKEWORD(2, 2),&wsaData) != 0) {
				cout << "\n WSAStartup ERROR \n" << endl;
				cout << WSAGetLastError() << endl;
			}

			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(PORT);
			serv_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				cout << "\n Socket creation error \n" << endl;
				cout << " " << WSAGetLastError() << endl;
			}

			if (connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))< 0) {
				cout << "\nConnection Failed \n" << endl;
				cout << WSAGetLastError() << endl;
			}
		}

		// alpha are numbers from 0 to 255
		void paintLine(int pRed, int pGreen, int pBlue, int pAlpha, int x0, int y0, int x1, int y1) {
			string message = "line,"+to_string(pRed)+","+to_string(pGreen)+","+to_string(pBlue)+","+to_string(pAlpha)+",";
			message += to_string(x0)+","+to_string(y0)+","+to_string(x1)+","+to_string(y1)+"\n";
			const char* msg = message.c_str();
			send(sock,msg, strlen(msg), 0);
		}

		void paintDot(int pRed, int pGreen, int pBlue, int pAlpha, int x0, int y0, int pRadius) {
			
			string message = "dot,"+to_string(pRed)+","+to_string(pGreen)+","+to_string(pBlue)+","+to_string(pAlpha)+",";
			message += to_string(x0)+","+to_string(y0)+","+to_string(pRadius)+"\n";
			const char* msg = message.c_str();
			send(sock,msg, strlen(msg), 0);
		}

		void clear() {
			char* msgclear;
			string msgclearString = "clear\n";
			msgclear = &msgclearString[0];;
			send(sock, msgclear, strlen(msgclear), 0);
		}

		void closeConnection() {
			closesocket(sock);
		}
};



#endif