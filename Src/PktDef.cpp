/*
 * ========================================================================
 * $File: PktDef.cpp $
 * $Date: January 30 2016 $
 * $Revision: v1.0 $
 * $Creator: Marko Radmanovic $
 * ========================================================================
 */

#include "PktDef.h"
#include <cstring>

PktDef::PktDef()
{
	cmdPacket.header        = {};
	cmdPacket.header.Length = 7;
	RawBuffer               = nullptr;
	cmdPacket.CRC           = '\0';
}

PktDef::PktDef(char *arg)
{
	cmdPacket.header.PktCount = (arg[3] << 24) | (arg[2] << 16) |
								(arg[1] << 8)  | arg[0];

	cmdPacket.header.Command  = (arg[4] & (1 << 0)) != 0;
	cmdPacket.header.Sleep    = (arg[4] & (1 << 1)) != 0;
	cmdPacket.header.Ack      = (arg[4] & (1 << 2)) != 0;
	cmdPacket.header.Video    = (arg[4] & (1 << 3)) != 0;
	cmdPacket.header.Settings = (arg[4] & (1 << 4)) != 0;

	cmdPacket.header.Length = arg[5];
	cmdPacket.CRC           = arg[6];
}

void PktDef::SetCmd(CmdType arg)
{
	if (COMMAND == arg)
	{
		cmdPacket.header.Command = 1;
		cmdPacket.header.Length  = 7;
		cmdPacket.header.Sleep   = 0;
		cmdPacket.header.Settings = 0;
		cmdPacket.header.Video = 0;
	}
	else if (SLEEP == arg)
	{
		cmdPacket.header.Sleep   = 1;
		cmdPacket.header.Length  = 7;
		cmdPacket.header.Command = 0;
		cmdPacket.header.Settings = 0;
		cmdPacket.header.Video = 0;
	}
	else if (ACK == arg)
	{
		cmdPacket.header.Ack = 1;
	}
	else if (VIDEO == arg)
	{
		cmdPacket.header.Video = 1;
		cmdPacket.header.Length = 7;//To be changed probably
		cmdPacket.header.Command = 0;
		cmdPacket.header.Sleep = 0;
		cmdPacket.header.Settings = 0;
	}
	else if (SETTINGS == arg)
	{
		cmdPacket.header.Settings = 1;
		cmdPacket.header.Length = 7;
		cmdPacket.header.Command = 0;
		cmdPacket.header.Sleep = 0;
		cmdPacket.header.Video = 0;
	}
}

void PktDef::SetPktCount(int arg)
{
	cmdPacket.header.PktCount = arg;
}

CmdType PktDef::GetCmd()
{
	if (cmdPacket.header.Command)          return COMMAND;
	else if (cmdPacket.header.Sleep)       return SLEEP;
	else if (cmdPacket.header.Ack)         return ACK;
	else if (cmdPacket.header.Video)       return VIDEO;
	else if (cmdPacket.header.Settings)    return SETTINGS;
}

bool PktDef::GetAck()
{
	return cmdPacket.header.Ack;
}

int PktDef::GetLength()
{
	return cmdPacket.header.Length;
}

int PktDef::GetPktCount()
{
	return cmdPacket.header.PktCount;
}

bool PktDef::CheckCRC(char *buffer, int buffer_size)
{
	PktDef check;

	if (buffer_size == 7)
	{
		memcpy(&check.cmdPacket, buffer, buffer_size);
		check.cmdPacket.CRC = buffer[6];
	}

	unsigned int x = check.cmdPacket.header.PktCount;

	int crc_check;
	for (crc_check = 0; x != 0; x >>= 1)
		if (x & 01)
			crc_check++;

	if (check.cmdPacket.header.Command)  crc_check++;
	if (check.cmdPacket.header.Sleep)    crc_check++;
	if (check.cmdPacket.header.Ack)      crc_check++;
	if (check.cmdPacket.header.Video)    crc_check++;
	if (check.cmdPacket.header.Settings) crc_check++;

	unsigned char y = check.cmdPacket.header.Length;
	for (; y != 0; y >>= 1)
		if (y & 01)
			crc_check++;

	int conc = check.cmdPacket.CRC;

	return (crc_check == conc);
}

void PktDef::CalcCRC()
{
	int crc_check;

	unsigned int x = cmdPacket.header.PktCount;
	for (crc_check = 0; x != 0; x >>= 1)
		if (x & 01)
			crc_check++;
	
	if (cmdPacket.header.Command)  crc_check++;
	if (cmdPacket.header.Sleep)    crc_check++;
	if (cmdPacket.header.Ack)      crc_check++;
	if (cmdPacket.header.Video)    crc_check++;
	if (cmdPacket.header.Settings) crc_check++;

	unsigned char y = cmdPacket.header.Length;
	for (; y != 0; y >>= 1)
		if (y & 01)
			crc_check++;

	cmdPacket.CRC = crc_check;
}

char * PktDef::GenPacket()
{
	RawBuffer = new char[7];

	unsigned int value = cmdPacket.header.PktCount;

	RawBuffer[0] = value         & 0xFF;
	RawBuffer[1] = (value >> 8)  & 0xFF;
	RawBuffer[2] = (value >> 16) & 0xFF;
	RawBuffer[3] = (value >> 24) & 0xFF;

	RawBuffer[4] = ((cmdPacket.header.Command << 0)   |
					(cmdPacket.header.Sleep << 1)     |
					(cmdPacket.header.Ack << 2)       |
		            (cmdPacket.header.Video << 3)     |
		            (cmdPacket.header.Settings << 4)) & 0xFF;

	RawBuffer[5] = cmdPacket.header.Length;
	RawBuffer[6] = cmdPacket.CRC;

	return RawBuffer;
}

void PktDef::DeleteRawBuffer()
{
	if (RawBuffer != nullptr)
	{
		delete[] RawBuffer;
		RawBuffer = nullptr;
	}
}

char* PktDef::GetRawBuffer()
{
	return RawBuffer;
}
