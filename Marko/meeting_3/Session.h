#pragma once
//#if !defined(PACKET_H)
/* 
   ========================================================================
   $File: Session.h $
   $Date: March 13 2017 $
   $Revision: v1.0 $
   $Creator: Marko Radmanovic $
   ======================================================================== 
*/

//gets rid of annoying vs warning about strcpy
#pragma warning(disable : 4996)

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <string>
#include <vector>
#include "pktdef.h"
#include <iostream>

static int pktInfoBufferOffset;
//Used to keep track of the current amount in the buffer
static unsigned int currentCharNumb;

class Session
{
	struct sockaddr_in SvrAddr;
	SOCKET clientSocketL, clientSocketR;
	WORD wVersionRequested;
	WSADATA wsaData;
	unsigned int pktCount = 0;

	PktDef txPacket;
	char** buffer;

public:
	Session();
	~Session();

	char** GetBuffer();
	PktDef GetPacket();

	int StartDataCom();
	void SendPacket(std::string s);
	int RecievePacket(HWND*, HWND*);
};

//#define PACKET_H
//#endif