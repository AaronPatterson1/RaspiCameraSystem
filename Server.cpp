/*
   ========================================================================
   $File: Server.cpp $
   $Date: November 30 2016 $
   $Revision: v1.0 $
   $Creator: Marko Radmanovic $
   ========================================================================
*/

#include "Session.h"

#define IDC_DISPLAY	101
#define IDC_BUFFER 102

#define IDC_SEND_BUTTON   200
#define IDC_INIT_BUTTON   201
#define IDC_GEN_BUTTON    202
#define IDC_QUIT_BUTTON   203
#define IDC_MLEFT_BUTTON  204
#define IDC_MRIGHT_BUTTON 205
#define IDC_MUP_BUTTON    206
#define IDC_MDOWN_BUTTON  207

Session * GSession = NULL;

HWND hWndDisplay = NULL;
HWND hWndBuffer = NULL;
HWND hWndInit = NULL;
HWND hWndSend = NULL;
HWND hWndGenLog = NULL;
HWND hWndQuit = NULL;
HWND hWndMoveL = NULL;
HWND hWndMoveR = NULL;
HWND hWndMoveU = NULL;
HWND hWndMoveD = NULL;

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_SEND_BUTTON:
				{
					char szBuffer[1024];
					ZeroMemory(szBuffer, sizeof(szBuffer));

					SendMessage(hWndBuffer, WM_GETTEXT, sizeof(szBuffer), (LPARAM)(szBuffer));
					send(GSession->GetSocket(), szBuffer, strlen(szBuffer), 0);
					SendMessage(hWndBuffer, WM_SETTEXT, NULL, (LPARAM)"");
				}	
				break;

				case IDC_INIT_BUTTON: 
				{
					EnableWindow(hWndSend, TRUE);
					//EnableWindow(hWndGenLog, TRUE);
					//EnableWindow(hWndQuit, TRUE);
					EnableWindow(hWndMoveL, TRUE);
					EnableWindow(hWndMoveR, TRUE);
					EnableWindow(hWndMoveU, TRUE);
					EnableWindow(hWndMoveD, TRUE);

					char szBuffer[1024];
					ZeroMemory(szBuffer, sizeof(szBuffer));

					strcpy(szBuffer, "Initalize Drone");
					send(GSession->GetSocket(), szBuffer, strlen(szBuffer), 0);
					SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"");
				}
				break;
			
				case IDC_GEN_BUTTON: 
				{
					if (currentCharNumb == 0)
					{
						SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Nothing has been stored yet Failed to generate log");
					}
					else if (currentCharNumb > 0)
					{
						SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Log is being generated");
						HANDLE hFile;
						DWORD dwBytesToWrite = (DWORD)strlen(GSession->GetHistory());
						DWORD dwBytesWritten = 0;
						BOOL bErrorFlag = FALSE;

						hFile = CreateFile(TEXT("Log.txt"), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

						if (hFile == INVALID_HANDLE_VALUE)
						{
							SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Unable to open file log for write");
							break;;
						}

						bErrorFlag = WriteFile(hFile, GSession->GetHistory(), dwBytesToWrite, &dwBytesWritten, NULL);

						if (FALSE == bErrorFlag)
						{
							SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Unable to write to log file");
							break;
						}
						else
						{
							if (dwBytesWritten != dwBytesToWrite)
							{
								SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Not everything to be written has been done");
							}
							else
							{
								SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Wrote Log file Succesfully");
							}
						}

						CloseHandle(hFile);
					}
				}
				break;
			
				case IDC_QUIT_BUTTON: 
				{
					char szBuffer[1024];
					ZeroMemory(szBuffer, sizeof(szBuffer));

					strcpy(szBuffer, "QUIT");
					send(GSession->GetSocket(), szBuffer, strlen(szBuffer), 0);

					MessageBox(hWnd, TEXT("Server closed connection"), TEXT("Connection closed!"), MB_ICONINFORMATION | MB_OK);
					closesocket(GSession->GetSocket());
					SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				}
				break;
			
				case IDC_MLEFT_BUTTON: 
				{
					GSession->Move_Left();
				}
				break;
			
				case IDC_MRIGHT_BUTTON: 
				{
					GSession->Move_Right();
				}
				break;
				
				case IDC_MUP_BUTTON: 
				{
					GSession->Move_Up();
				} 
				break;
				
				case IDC_MDOWN_BUTTON: 
				{
					GSession->Move_Down();
				}
				break;
			}
		}
		break;
	
		case WM_CREATE:
		{
			// Create Display to view packet info
			hWndDisplay = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
				WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				50, 120, 400, 200, hWnd, (HMENU)IDC_DISPLAY, GetModuleHandle(NULL), NULL);

			if (!hWndDisplay)
			{
				MessageBox(hWnd, TEXT("Could not create incoming edit box."), TEXT("Error"), MB_OK | MB_ICONERROR);
			}
	
			HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hWndDisplay, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Waiting for client to connect...");

			// Create Buffer to write into and to send from
			hWndBuffer = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
				WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				50, 50, 400, 60, hWnd, (HMENU)IDC_DISPLAY, GetModuleHandle(NULL), NULL);

			if (!hWndBuffer)
			{
				MessageBox(hWnd, TEXT("Could not create outgoing edit box."), TEXT("Error"), MB_OK | MB_ICONERROR);
			}
	
			SendMessage(hWndBuffer, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndBuffer, WM_SETTEXT, NULL, (LPARAM)"Type message here...");
	
			//First row action buttons
			hWndInit = CreateWindow(TEXT("BUTTON"), TEXT("Init"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				50, 330, 95, 25, hWnd, (HMENU)IDC_INIT_BUTTON,
				GetModuleHandle(NULL), NULL);
	
			hWndSend = CreateWindow(TEXT("BUTTON"), TEXT("Send"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				150, 330, 95, 25, hWnd, (HMENU)IDC_SEND_BUTTON,
				GetModuleHandle(NULL), NULL);
	
			hWndGenLog = CreateWindow(TEXT("BUTTON"), TEXT("Generate Log"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				250, 330, 95, 25, hWnd, (HMENU)IDC_GEN_BUTTON,
				GetModuleHandle(NULL), NULL);
			
			hWndQuit = CreateWindow(TEXT("BUTTON"), TEXT("Quit"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				350, 330, 95, 25, hWnd, (HMENU)IDC_QUIT_BUTTON,
				GetModuleHandle(NULL), NULL);
	
			//Second Row movement buttons
			hWndMoveL = CreateWindow(TEXT("BUTTON"), TEXT("Move Left"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				50, 375, 95, 25, hWnd, (HMENU)IDC_MLEFT_BUTTON,
				GetModuleHandle(NULL), NULL);
	
			hWndMoveR = CreateWindow(TEXT("BUTTON"), TEXT("Move Right"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				150, 375, 95, 25, hWnd, (HMENU)IDC_MRIGHT_BUTTON,
				GetModuleHandle(NULL), NULL);
	
			hWndMoveU = CreateWindow(TEXT("BUTTON"), TEXT("Move Up"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				250, 375, 95, 25, hWnd, (HMENU)IDC_MUP_BUTTON,
				GetModuleHandle(NULL), NULL);
	
			hWndMoveD = CreateWindow(TEXT("BUTTON"), TEXT("Move Down"),
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				350, 375, 95, 25, hWnd, (HMENU)IDC_MDOWN_BUTTON,
				GetModuleHandle(NULL), NULL);
	
			EnableWindow(hWndSend, FALSE);
			//EnableWindow(hWndGenLog, FALSE);
			//EnableWindow(hWndQuit, FALSE);
			EnableWindow(hWndMoveL, FALSE);
			EnableWindow(hWndMoveR, FALSE);
			EnableWindow(hWndMoveU, FALSE);
			EnableWindow(hWndMoveD, FALSE);

			SendMessage(hWndSend, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndInit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndGenLog, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndQuit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndMoveL, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndMoveR, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndMoveU, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hWndMoveD, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	
			GSession->StartDataCom(&hWnd);
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			//Session dtor handles closing the data communication and the cleanup
			GSession->~Session();
			return 0;
		}
		break;

		case WM_SOCKET:
		{
			switch (WSAGETSELECTEVENT(lParam))
			{
				case FD_READ:
				{
					char szIncoming[1024];
					ZeroMemory(szIncoming, sizeof(szIncoming));

					int inDataLength = recv(GSession->GetSocket(), (char*)szIncoming, sizeof(szIncoming) / sizeof(szIncoming[0]), 0);

					strncat(GSession->GetHistory(), szIncoming, inDataLength);
					strcat(GSession->GetHistory(), "\r\n");
					++currentCharNumb;

					SendMessage(hWndDisplay, WM_SETTEXT, sizeof(szIncoming) - 1, (LPARAM)(GSession->GetHistory()));
				}
				break;

				case FD_CLOSE:
				{
					MessageBox(hWnd, TEXT("Client closed connection"), TEXT("Connection closed!"), MB_ICONINFORMATION | MB_OK);
					closesocket(GSession->GetSocket());
					SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				}
				break;

				case FD_ACCEPT:
				{
					int size = sizeof(sockaddr);
					GSession->SetSocket(accept(wParam, &(GSession->GetSockAddr()), &size));
					if (GSession->GetSocket() == INVALID_SOCKET)
					{
						int nret = WSAGetLastError();
						//Session dtor handles closing the data communication and the cleanup
						GSession->~Session();
					}
					SendMessage(hWndDisplay, WM_SETTEXT, NULL, (LPARAM)"Client connected!");
				}
				break;
			}
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	//Start session
	Session s;
	GSession = &s;

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

	HWND hWnd = CreateWindowEx(NULL, TEXT("Window Class"), TEXT("Ground Control"),
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
