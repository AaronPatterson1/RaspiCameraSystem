/*
   ========================================================================
   $File: Session.cpp $
   $Date: November 30 2016 $
   $Revision: v1.0 $
   $Creator: Marko Radmanovic $
   ========================================================================
*/

#include "Session.h"

Session::Session()
{
	ZeroMemory(sessionHistory, sizeof(sessionHistory));
	Socket = NULL;
}

Session::~Session()
{
	shutdown(Socket, SD_BOTH);
	closesocket(Socket);
	WSACleanup();
}

void Session::Move_Left()
{
	char szBuffer[52];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	strcpy(szBuffer, "Command - Move\r\n   Direction - LEFT\r\n   Seconds - 1");
	send(Socket, szBuffer, strlen(szBuffer), 0);
}

void Session::Move_Right()
{
	char szBuffer[53];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	strcpy(szBuffer, "Command - Move\r\n   Direction - RIGHT\r\n   Seconds - 1");
	send(Socket, szBuffer, strlen(szBuffer), 0);
}

void Session::Move_Up()
{
	char szBuffer[50];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	strcpy(szBuffer, "Command - Move\r\n   Direction - UP\r\n   Seconds - 1");
	send(Socket, szBuffer, strlen(szBuffer), 0);
}

void Session::Move_Down()
{
	char szBuffer[52];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	strcpy(szBuffer, "Command - Move\r\n   Direction - DOWN\r\n   Seconds - 1");
	send(Socket, szBuffer, strlen(szBuffer), 0);
}

int Session::StartDataCom(HWND* hWnd)
{
	WSADATA WsaDat;
	int nResult = WSAStartup(MAKEWORD(2, 2), &WsaDat);
	if (nResult != 0)
	{
		MessageBox(*hWnd, TEXT("Winsock initialization failed"), TEXT("Critical Error"), MB_ICONERROR);
		SendMessage(*hWnd, WM_DESTROY, NULL, NULL);
		return -1;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		MessageBox(*hWnd, TEXT("Socket creation failed"), TEXT("Critical Error"), MB_ICONERROR);
		SendMessage(*hWnd, WM_DESTROY, NULL, NULL);
		return -1;
	}

	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(nPort);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(Socket, (LPSOCKADDR)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR)
	{
		MessageBox(*hWnd, TEXT("Unable to bind socket"), TEXT("Error"), MB_OK);
		SendMessage(*hWnd, WM_DESTROY, NULL, NULL);
		return -1;
	}

	nResult = WSAAsyncSelect(Socket, *hWnd, WM_SOCKET, (FD_CLOSE | FD_ACCEPT | FD_READ));
	if (nResult)
	{
		MessageBox(*hWnd, TEXT("WSAAsyncSelect failed"), TEXT("Critical Error"), MB_ICONERROR);
		SendMessage(*hWnd, WM_DESTROY, NULL, NULL);
		return -1;
	}

	if (listen(Socket, (1)) == SOCKET_ERROR)
	{
		MessageBox(*hWnd, TEXT("Unable to listen!"), TEXT("Error"), MB_OK);
		SendMessage(*hWnd, WM_DESTROY, NULL, NULL);
		return -1;
	}

	return 0;
}