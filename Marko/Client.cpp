/*
   ========================================================================
   $File: Client.cpp $
   $Date: November 30 2016 $
   $Revision: v1.0 $
   $Creator: Marko Radmanovic $
   ========================================================================
*/
//Client - Drone

#pragma warning(disable : 4996)

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

#define IDC_DISPLAY	101
#define WM_SOCKET 104

#define nPort 27015

HWND hWndDisplay = NULL;
SOCKET Socket = NULL;
char sessionHistory[10000];

static int drone_coordinate_x;
static int drone_coordinate_y;

void UpdatePosition()
{
	char szBuffer[1024];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	
	strcat(szBuffer, "New Drone Position\r\n   Drone X coordinate : ");
	char buffer[3];
	sprintf(buffer, "%d", drone_coordinate_x);
	strcat(szBuffer, buffer);
	strcat(szBuffer, "\r\n   Drone Y coordinate : ");
	sprintf(buffer, "%d", drone_coordinate_y);
	strcat(szBuffer, buffer);

	send(Socket, szBuffer, strlen(szBuffer), 0);
}

int ResponseToPacket(char * s)
{
	if (strcmp(s, "Initalize Drone") == 0)
	{
		char szBuffer[1024];
		ZeroMemory(szBuffer, sizeof(szBuffer));
		strcpy(szBuffer, "Drone has been initialized\r\n   Barometric Pressure : 1.01325\r\n   Altitude : 1\r\n   Status : ready");
		send(Socket, szBuffer, strlen(szBuffer), 0);
		return 0;
	}
	else if (strcmp(s, "Command - Move\r\n   Direction - LEFT\r\n   Seconds - 1") == 0)
	{
		--drone_coordinate_x;
		UpdatePosition();
		return 0;
	}
	else if (strcmp(s, "Command - Move\r\n   Direction - RIGHT\r\n   Seconds - 1") == 0)
	{
		++drone_coordinate_x; 
		UpdatePosition();
		return 0;
	}
	else if (strcmp(s, "Command - Move\r\n   Direction - UP\r\n   Seconds - 1") == 0)
	{
		++drone_coordinate_y;
		UpdatePosition(); 
		return 0;
	}
	else if (strcmp(s, "Command - Move\r\n   Direction - DOWN\r\n   Seconds - 1") == 0)
	{
		--drone_coordinate_y;
		UpdatePosition(); 
		return 0;
	}
	else if (strcmp(s, "QUIT") == 0)
	{
		return -1;
	}

	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			ZeroMemory(sessionHistory, sizeof(sessionHistory));

			// Create display for incoming packets
			RECT rc;
			GetClientRect(hWnd, &rc);
			hWndDisplay = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
				WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				rc.left, rc.top, rc.right, rc.bottom, hWnd, (HMENU)IDC_DISPLAY,
				GetModuleHandle(NULL), NULL);
	
			if (!hWndDisplay)
			{
				MessageBox(hWnd, TEXT("Could not create disp;ay."), TEXT("Error"), MB_OK | MB_ICONERROR);
			}
	
			HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hWndDisplay, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Attempting to connect to ground control");
	
			//Start data com
			WSADATA WsaDat;
			int nResult = WSAStartup(MAKEWORD(2, 2), &WsaDat);
			if (nResult != 0)
			{
				MessageBox(hWnd, TEXT("Winsock initialization failed"), TEXT("Critical Error"), MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}	
				
			Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (Socket == INVALID_SOCKET)
			{
				MessageBox(hWnd, TEXT("Socket creation failed"), TEXT("Critical Error"), MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}
	
			nResult = WSAAsyncSelect(Socket, hWnd, WM_SOCKET, (FD_CLOSE | FD_READ));
			if (nResult)
			{
				MessageBox(hWnd, TEXT("WSAAsyncSelect failed"), TEXT("Critical Error"), MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}
	
			struct hostent *host;
			if ((host = gethostbyname("localhost")) == NULL)
			{
				MessageBox(hWnd, TEXT("Unable to resolve host name"), TEXT("Critical Error"), MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			SOCKADDR_IN SockAddr;
			SockAddr.sin_port = htons(nPort);
			SockAddr.sin_family = AF_INET;
			SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
				
			connect(Socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
		}
		break;
		
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			shutdown(Socket, SD_BOTH);
			closesocket(Socket);
			WSACleanup();
			return 0;
		}
		break;
	
		case WM_SOCKET:
		{
			if (WSAGETSELECTERROR(lParam))
			{
				MessageBox(hWnd, TEXT("Connection to server failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}
	
			switch (WSAGETSELECTEVENT(lParam))
			{
				case FD_READ:
				{
					char szIncoming[1024];
					ZeroMemory(szIncoming, sizeof(szIncoming));
		
					int inDataLength = recv(Socket, (char*)szIncoming, sizeof(szIncoming) / sizeof(szIncoming[0]), 0);
					int n = ResponseToPacket(szIncoming);

					if (n == -1)
					{
						SendMessage(hWnd, WM_DESTROY, NULL, NULL);
						break;
					}

					strncat(sessionHistory, szIncoming, inDataLength);
					strcat(sessionHistory, "\r\n");

					SendMessage(hWndDisplay, WM_SETTEXT, sizeof(szIncoming) - 1, (LPARAM)(&sessionHistory));
				}
				break;

				case FD_CLOSE:
				{	
					MessageBox(hWnd, "Server closed connection", "Connection closed!", MB_ICONINFORMATION | MB_OK);
					closesocket(Socket);
					SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				}
				break;
			}
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = hInst;
	wClass.lpfnWndProc = (WNDPROC)WinProc;
	wClass.lpszClassName = "Window Class";
	wClass.lpszMenuName = NULL;
	wClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wClass))
	{
		int nResult = GetLastError();
		MessageBox(NULL, TEXT("Window class creation failed\r\nError code:"), TEXT("Window Class Failed"), MB_ICONERROR);
	}

	HWND hWnd = CreateWindowEx(NULL, TEXT("Window Class"), TEXT("Drone - Display"),
		WS_OVERLAPPEDWINDOW, 200, 200, 530, 480, NULL, NULL, hInst, NULL);

	if (!hWnd)
	{
		int nResult = GetLastError();
		MessageBox(NULL, TEXT("Window creation failed\r\nError code:"), TEXT("Window Creation Failed"), MB_ICONERROR);
	}

	ShowWindow(hWnd, nShowCmd);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
