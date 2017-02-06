#pragma once
/* 
   ========================================================================
   $File: Session.h $
   $Date: November 30 2016 $
   $Revision: v1.0 $
   $Creator: Marko Radmanovic $
   ======================================================================== 
*/
#pragma warning(disable : 4996)

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#include <windows.h>

#include <stdlib.h>

#define WM_SOCKET 104
#define nPort 27015
#define MAX_BUFFER_SIZE 128

//Used to keep track of the current amount in the session history
static unsigned int currentCharNumb;

class Session
{
	SOCKET Socket;
	sockaddr sockAddrClient;
	char sessionHistory[10000];

public:
	Session();
	~Session();

	SOCKET GetSocket() { return Socket; }
	void SetSocket(SOCKET s) { Socket = s; }

	sockaddr GetSockAddr() { return sockAddrClient; }

	char* GetHistory() { return sessionHistory; }

	void Move_Left();
	void Move_Right();
	void Move_Up();
	void Move_Down();

	int StartDataCom(HWND* hWnd);
};
