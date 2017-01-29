/*
 * ========================================================================
 * $File: M1Server.cpp $
 * $Date: January 30 2016 $
 * $Revision: v1.0 $
 * $Creator: Marko Radmanovic $
 * ========================================================================
 */

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "PktDef.h"
#include <iostream>

void error(char *msg)
{
	perror(msg);
	exit(1);
}

void sendstatus(int sockfd)
{
	PktDef spacket;
	spacket.SetCmd((CmdType)STATUS);

	spacket.CalcCRC();
	int n;
	if ((n = write(sockfd, spacket.GenPacket(), 11)) < 0)
	{
		error(const_cast<char *>("error writing to socket"));
		exit(1);
	}
	spacket.DeleteRawBuffer();
}

void sendpkt(int sockfd, CmdType arg)
{
	PktDef ackpacket;
	ackpacket.SetCmd(arg);
	ackpacket.CalcCRC();

	int n;
	if ((n = write(sockfd, ackpacket.GenPacket(), 7)) < 0)
	{
		error(const_cast<char *>("error writing to socket"));
		exit(1);
	}
	ackpacket.DeleteRawBuffer();
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno = 51717, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int data;

	std::cout << "using port " << portno << std::endl;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error(const_cast<char *>("error opening socket"));
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error(const_cast<char *>("error on binding"));
	}
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	int current_packet = 0;

	while (1)
	{
		std::cout << "waiting for new client...\n";
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen)) < 0)
		{
			error(const_cast<char *>("error on accept"));
		}
		std::cout << "opened new communication with client\n";

		while (1)
		{
			PktDef RxPacket;
			char rxbuffer_ackpacket[7];
			RxPacket.SetPktCount(current_packet);
			++current_packet;
			int n;

			if ((n = read(newsockfd, (char *)&rxbuffer_ackpacket, 7)) < 0)
			{
				error(const_cast<char *>("error reading from socket"));
			}
			memcpy(&RxPacket, rxbuffer_ackpacket, 7);
			if (RxPacket.CheckCRC(rxbuffer_ackpacket, 7))
			{
				std::cout << "\nCRC accepted" << std::endl;
				if (RxPacket.GetAck())
				{
					std::cout << "acknowledgement received" << std::endl;
					sendpkt(newsockfd, ACK);
				}
				else if (RxPacket.GetCmd() == SLEEP)
				{
					std::cout << "MSG Rx (sleep) terminating all socket connections" << std::endl;
					sendpkt(newsockfd, SLEEP);
					close(newsockfd);
					return 0;
				}
				else
				{
					sendpkt(newsockfd, ACK);
				}
			}
		}
	}

	return 0;
}
