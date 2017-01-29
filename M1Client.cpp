/*
 * ========================================================================
 * $File: M1Client.cpp $
 * $Date: January 30 2016 $
 * $Revision: v1.0 $
 * $Creator: Marko Radmanovic $
 * ========================================================================
 */

#include <windows.networking.sockets.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "pktdef.h"

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char *argv[])
{
	struct sockaddr_in svraddr;
	SOCKET ClientSocket;
	WORD wVersionRequested;
	WSADATA wsaData;
	PktDef txpacket;
	unsigned int pktcount = 0;

	wVersionRequested = MAKEWORD(2, 3);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		return 1;
	}

	if ((ClientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return 1;
	}

	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(51717);
	svraddr.sin_addr.s_addr = inet_addr("192.168.1.3");

	if ((connect(ClientSocket, (struct sockaddr *)&svraddr, sizeof(svraddr))) == SOCKET_ERROR)
	{
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	char buffer[256];
	int data;
	int current_packet = 0;
	
	std::cout << "\nCommands\n   0 - sends packet\n   1 - sends sleep packet (terminates connection)\n   2 - sends ack packet";

	while (1)
	{
		PktDef RxPacket;
		RxPacket.SetPktCount(current_packet);
		++current_packet;

		std::cout << "\nenter command : ";
		int t;
		std::cin >> t;
		CmdType b = (CmdType)t;
		RxPacket.SetCmd(b);
		RxPacket.CalcCRC();

		char rxbuffer_ackpacket[7];

		send(ClientSocket, RxPacket.GenPacket(), RxPacket.GetLength(), 0);
		RxPacket.DeleteRawBuffer();

		int n;
		n = recv(ClientSocket, (char *)&rxbuffer_ackpacket, 7, 0);
		memcpy(&RxPacket, rxbuffer_ackpacket, 7);

		if (RxPacket.CheckCRC(rxbuffer_ackpacket, 7))
		{
			std::cout << "\nCRC accepted" << std::endl;
			if (RxPacket.GetAck())
			{
				std::cout << "acknowledgement received" << std::endl;
			}
			else if (RxPacket.GetCmd() == SLEEP)
			{
				std::cout << "MSG Rx (SLEEP) terminating all socket connections" << std::endl;
				closesocket(ClientSocket);
				break;
			}
		}
	}

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}
