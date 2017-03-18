/*
   ========================================================================
   $File: Session.cpp $
   $Date: March 13 2017 $
   $Revision: v1.0 $
   $Creator: Marko Radmanovic $
   ========================================================================
*/

#include "Session.h"

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
	SvrAddr.sin_addr.s_addr = inet_addr("192.168.1.3");

    if ((connect(clientSocketL, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
    {
        closesocket(clientSocketL);
        WSACleanup();
        return -1;
    }
}

void Session::SendPacket(std::string s)
{
	txPacket.SetPktCount(pktCount);
	++pktCount;

	CmdType b = (CmdType)atoi(s.c_str());
	txPacket.SetCmd(b);

	txPacket.CalcCRC();

	send(clientSocketL, txPacket.GenPacket(), 7, 0);
	txPacket.DeleteRawBuffer();
}

int Session::RecievePacket(HWND* hWndPktInfo, HWND* ghWnd)
{
	int n = recv(clientSocketL, txPacket.GetRawBuffer(), 7, 0);

	if (txPacket.GetRawBuffer() != NULL)
	{
		if (txPacket.CheckCRC(txPacket.GetRawBuffer(), 7))
		{
            DisplayMsgInHWND("\nCRC Accepted\n", hWndPktInfo, ghWnd);
			
			if (txPacket.GetAck())
			{
				DisplayMsgInHWND("Acknowledgement Received\n", hWndPktInfo, ghWnd);
			}

			if (txPacket.GetCmd() == COMMAND)
            {
				DisplayMsgInHWND("Enter name of command or script to be run : ", hWndPktInfo, ghWnd);

				//IMPORTANT
				//TODO(marko) : update pktinfo buffer and use send instead of this
                std::string filename;
                std::cin >> filename;
                send(clientSocketL, filename.c_str(), 256, 0);

                // Server will send ACK packet after script or command is run as confirmation
                n = recv(clientSocketL, txPacket.GetRawBuffer(), 7, 0);
                //memcpy(&RxPacket, rxbuffer_ackpacket, 7);

                if (txPacket.CheckCRC(txPacket.GetRawBuffer(), 7))
                {
					DisplayMsgInHWND("Python script was ran", hWndPktInfo, ghWnd);
                }
            }
            else if (txPacket.GetCmd() == SLEEP)
            {
				DisplayMsgInHWND("Msg Rx (SLEEP) terminating all socket connections\n", hWndPktInfo, ghWnd);
                closesocket(clientSocketL);
            }
            
            return 1;
		}
	}
	else
	{
		DisplayMsgInHWND("Failure to retrieve packet", hWndPktInfo, ghWnd);
	}
    
	return 0;
}
