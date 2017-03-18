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

    buffer[currentCharNumb] = (char*)malloc(sizeof("Waiting for client connection"));
    strcpy(buffer[currentCharNumb], "Waiting for client connection");
    ++currentCharNumb;
    buffer[currentCharNumb] = (char*)malloc(sizeof("\nCommands\n   0 - sends packet\n   1 - sends sleep packet (terminates connection)\n   2 - sends ack packet"));
    strcpy(buffer[currentCharNumb], "\nCommands\n   0 - sends packet\n   1 - sends sleep packet (terminates connection)\n   2 - sends ack packet");
    ++currentCharNumb;
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
            // make inlined function that covers this 
			buffer[currentCharNumb] = (char*)malloc(sizeof("\nCRC Accepted\n"));
			strcpy(buffer[currentCharNumb], "\nCRC Accepted\n");
			++currentCharNumb;
            //
            
			if (txPacket.GetAck())
			{
				buffer[currentCharNumb] = (char*)malloc(sizeof("Acknowledgement Received\n"));
				strcpy(buffer[currentCharNumb], "Acknowledgement Received\n");
				++currentCharNumb;
			}

			if (txPacket.GetCmd() == COMMAND)
            {
                buffer[currentCharNumb] = (char*)malloc(sizeof("Enter name of command or script to be run : "));
				strcpy(buffer[currentCharNumb], "Enter name of command or script to be run : ");
				++currentCharNumb;
                
                std::string filename;
                std::cin >> filename;
                send(clientSocketL, filename.c_str(), 256, 0);

                // Server will send ACK packet after script or command is run as confirmation
                n = recv(clientSocketL, txPacket.GetRawBuffer(), 7, 0);
                //memcpy(&RxPacket, rxbuffer_ackpacket, 7);

                if (txPacket.CheckCRC(txPacket.GetRawBuffer(), 7))
                {
                    buffer[currentCharNumb] = (char*)malloc(sizeof("Python script was ran"));
                    strcpy(buffer[currentCharNumb], "Python script was ran");
                    ++currentCharNumb;
                }
             
                //char RxBuffer_StatusPacket[11];
                //n = recv(ConnectionSocket, (char *)&RxBuffer_StatusPacket, 11, 0);
                //memcpy(&txPacket, RxBuffer_StatusPacket, 11);
                
                // std::string nc = std::to_string((unsigned int)txPacket.GetSeconds());
                // std::string str = "Packet Length " + nc + " \n";
                // buffer[currentCharNumb] = (char*)malloc(sizeof(str));
                // strcpy(buffer[currentCharNumb], str.c_str());
                // ++currentCharNumb;
            }
            
            else if (txPacket.GetCmd() == SLEEP)
            {
                buffer[currentCharNumb] = (char*)malloc(sizeof("Msg Rx (SLEEP) terminating all socket connections\n"));
                strcpy(buffer[currentCharNumb], "Msg Rx (SLEEP) terminating all socket connections\n");
                ++currentCharNumb;
                closesocket(clientSocketL);
            }
            
            return 1;
		}
	}
	else
	{
		HDC hdc = GetDC(*hWndPktInfo);
		char edittxt[1024];

		RECT rc;
		GetClientRect(*hWndPktInfo, &rc);
		ExtTextOut(hdc, rc.left, rc.top, ETO_OPAQUE, &rc, 0, 0, 0);
		char * text = "Failure to retrieve packet";
		
		rc.top += pktInfoBufferOffset;
		pktInfoBufferOffset += 20;

		DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);
		ReleaseDC(*hWndPktInfo, hdc);

		UpdateWindow(*ghWnd);
	}
    
	return 0;
}
