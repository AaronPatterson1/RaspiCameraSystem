/*
   ========================================================================
   $File: Session.cpp $
   $Date: March 13 2017 $
   $Revision: v1.0 $
   $Creator: Marko Radmanovic $
   ========================================================================
*/

#include "Session.h"
#include <thread>

#define MAX_BUFFER_SIZE 128

Session::Session()
{
	buffer = (char **)malloc(MAX_BUFFER_SIZE * sizeof(char));;
}

Session::~Session()
{
	if (buffer != NULL)
	{
		for (int i = 0; i < currentCharNumb; ++i)
		{
			free(buffer[i]);
		}
		free(buffer);
	}
    // Closes sockets
    closesocket(clientSocketL);
	closesocket(clientSocketR);
    
	WSACleanup();
}

char** Session::GetBuffer()
{
	return buffer;
}

PktDef Session::GetPacket()
{
	return txPacket;
}

//make default ctor int a = 51717
// also take in the s_addr
int Session::StartDataCom()
{
	wVersionRequested = MAKEWORD(2, 3);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		return -1;
	}
	
	if ((clientSocketL = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return -1;
	}

	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(51717);
    //SvrAddr.sin_addr.s_addr = INADDR_ANY;//"192.168.1.3"
	SvrAddr.sin_addr.s_addr = inet_addr("169.254.164.4");

    if ((connect(clientSocketL, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
    {
        closesocket(clientSocketL);
        WSACleanup();
        return -1;
    }
}

inline static
bool hasDigits(std::string s) 
{
	return (s.find_first_not_of("0123456789") == std::string::npos);
}

void Session::SendSettingValue(std::string s)
{
	send(clientSocketL, s.c_str(), 2, 0);
}

void videoRecord() {

}

void Session::SendPacket(std::string s)
{
	if (hasDigits(s))
	{
		txPacket.SetPktCount(pktCount);
		++pktCount;

		CmdType b = (CmdType)atoi(s.c_str());
		txPacket.SetCmd(b);

		txPacket.CalcCRC();

		send(clientSocketL, txPacket.GenPacket(), txPacket.GetLength(), 0);
		txPacket.DeleteRawBuffer();
	
	}
	else
	{
		send(clientSocketL, s.c_str(), 256, 0);
	}
}

int Session::RecievePacket(HWND* hWndPktInfo, HWND* ghWnd, HWND* hWndBuffer)
{
	char temp[7];
	int n = recv(clientSocketL, temp, 7, 0);
	
	if (txPacket.CheckCRC(temp, 7))
	{
        DisplayMsgInHWND("\nCRC Accepted\n", hWndPktInfo, ghWnd);
			
		if (txPacket.GetAck())
		{
			DisplayMsgInHWND("Acknowledgement Received\n", hWndPktInfo, ghWnd);
		}

		if (txPacket.GetCmd() == COMMAND)
        {
			DisplayMsgInHWND("Enter name of command or script to be run : ", hWndPktInfo, ghWnd);
        }
		else if (txPacket.GetCmd() == SETTINGS)
        {
			DisplayMsgInHWND("Settings were succesfully sent", hWndPktInfo, ghWnd);
		}
		else if (txPacket.GetCmd() == SLEEP)
        {
			DisplayMsgInHWND("Msg Rx (SLEEP) terminating all socket connections\n", hWndPktInfo, ghWnd);
            closesocket(clientSocketL);
        }
 		else if (txPacket.GetCmd() == VIDEO)
		{
			DisplayMsgInHWND("Video is recording.", hWndPktInfo, ghWnd);
		}
        return 1;
	}
	else
	{
		DisplayMsgInHWND("Failure to retrieve packet", hWndPktInfo, ghWnd);
	}
    
	return 0;
}
