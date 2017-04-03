/*
 * ========================================================================
 * $File: PktDef.h $
 * $Date: January 30 2017 $
 * $Revision: v1.0 $
 * $Creator: Marko Radmanovic $
 * ========================================================================
 */

#pragma once

#define HEADERSIZE 6

enum CmdType { COMMAND, SLEEP, ACK };

struct Header
{
	unsigned int PktCount;
	bool Command : 1;
	bool Sleep : 1;
	bool Ack : 1;
	bool : 5;
	unsigned char Length;
};

class PktDef
{
	struct CmdPacket 
	{
		Header header;
		short CRC : 8;
	};
	CmdPacket cmdPacket;
	char *RawBuffer;

public:
	PktDef();
	PktDef(char *);
	void SetCmd(CmdType);
	void SetPktCount(int);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	int GetPktCount();
	bool CheckCRC(char *, int);
	void CalcCRC();
	char *GenPacket();
	void DeleteRawBuffer();
};
