/*
 * ========================================================================
 * $File: M1Server.cpp $
 * $Date: January 30 2017 $
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
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <errno.h>
#include "PktDef.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>

static short vid_quality = 1;
static short filter = 1;
static short rottation = 1;
static short fps = 1;
static short brigthness = 1;

std::string GetVideoQuality(short vid_quality)
{
	std::string t;
	
	switch (vid_quality)
	{
	case 1: t = "1080p"; break;
	case 2: t = "720p"; break;
	case 3: t = "480p"; break;
	case 4: t = "360p"; break;
	}

	return t;
}

std::string GetFilter(short filter)
{
	std::string t;

	switch (filter)
	{
	case 1: t = "Normal"; break;
	case 2: t = "Black and white"; break;
	case 3: t = "Negative"; break;
	}

	return t;
}

std::string GetRotation(short rottation)
{
	std::string t;

	switch (rottation)
	{
	case 1: t = "0"; break;
	case 2: t = "90"; break;
	case 3: t = "180"; break;
	case 4: t = "270"; break;
	}

	return t;
}

std::string GetFrameRate(short fps)
{
	std::string t;

	switch (fps)
	{
	case 1: t = "15"; break;
	case 2: t = "24"; break;
	case 3: t = "28"; break;
	case 4: t = "30"; break;
	}

	return t;
}

std::string GetBrigthness(short brigthness)
{
	std::string t;

	switch (brigthness)
	{
	case 1: t = "0"; break;
	case 2: t = "25"; break;
	case 3: t = "50"; break;
	case 4: t = "75"; break;
	case 5: t = "100"; break;
	}

	return t;
}

void recordVideo()
{
	std::string line;
	std::chrono::duration<double> elapsed_seconds;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	system("python3 vid_record.py");
	//check when video finishes
	start = std::chrono::system_clock::now();
	while(1)
	{
		end = std::chrono::system_clock::now();
		elapsed_seconds = end - start;
		if (elapsed_seconds.count() > 6)
			break;
	}
	//send packet with name of files
	//get strings
    /*system("ls vids/ > files.txt");

    std::ifstream file ("files.txt");
    if (file.is_open()) {
        while(getline(file,line))
            ++i;
        file.close();
    }
    std::ifstream file2 ("files.txt");
    std::string file_names[i];
    if (file2.is_open()) {
        while(getline(file2,line)){
            --i;
            file_names[i] = line;
        }
        file.close();
    }*/
	
	//send strings to client
	system("sshpass -p 'password123' scp ~/' + s + ' Aaron@169.254.227.33:~/Desktop/ps/BTP600/videos/");
	//send ack to signify end
	//sendpkt(newsocked, ACK);
}

void error(char *msg)
{
	perror(msg);
	exit(1);
}

void sendcommand(int sockfd)
{
	PktDef spacket;
	spacket.SetCmd((CmdType)COMMAND);

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
				else if (RxPacket.GetCmd() == COMMAND)
				{
					sendpkt(newsockfd, COMMAND); // Let the client know server is expecting command
					std::string s;

					int n;
					char buf[256];

					n = read(newsockfd, buf, sizeof(buf));
					s.append(buf, buf + n);

					if (n < 0)
					{
						error("Failed to recieve string from client");
						return 1; // error
					}

					std::cout << "Command or script to be run :  " << s.c_str() << std::endl;

					system(s.c_str());
					sendpkt(newsockfd, ACK);
				}
				else if (RxPacket.GetCmd() == SETTINGS)
				{
					std::cout << "Old Settings"
						<< "\n Video Quality : " << GetVideoQuality(vid_quality)
						<< "\n Filter        : " << GetFilter(filter)
						<< "\n Rotation      : " << GetRotation(rottation)
						<< "\n Framerate     : " << GetFrameRate(fps)
						<< "\n Brigthness    : " << GetBrigthness(brigthness);

					char setval[2];
					
					n = read(newsockfd, setval, sizeof(setval));
					vid_quality = atoi(setval);

					n = read(newsockfd, setval, sizeof(setval));
					filter = atoi(setval);

					n = read(newsockfd, setval, sizeof(setval));
					rottation = atoi(setval);

					n = read(newsockfd, setval, sizeof(setval));
					fps = atoi(setval);

					n = read(newsockfd, setval, sizeof(setval));
					brigthness = atoi(setval);

					std::cout << "\nNew Settings"
						<< "\n Video Quality : " << GetVideoQuality(vid_quality)
						<< "\n Filter        : " << GetFilter(filter)
						<< "\n Rotation      : " << GetRotation(rottation)
						<< "\n Framerate     : " << GetFrameRate(fps)
						<< "\n Brigthness    : " << GetBrigthness(brigthness)
						<< "\n";

					sendpkt(newsockfd, ACK);
				}
				else if (RxPacket.GetCmd() == VIDEO)
				{
					std::cout << "Video recording started..." << std::endl;
					//Video recording thread
					std::thread record(recordVideo);
					sendpkt(newsockfd, ACK);
					record.join();
					std::cout << "Video finished recording." << std::endl;
					
				} 
			}
		}
	}

	return 0;
}
