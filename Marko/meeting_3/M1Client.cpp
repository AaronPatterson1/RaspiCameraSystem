/*
 * ========================================================================
 * $File: M1Client.cpp $
 * $Date: January 30 2017 $
 * $Revision: v1.0 $
 * $Creator: Marko Radmanovic $
 * ========================================================================
 */

#include <windows.h>
#include <windows.networking.sockets.h>
#include <stdio.h>
//#include <iostream> in session
#include <string.h>
#include <string>
#include <stdlib.h>
#include <errno.h>
//#include "pktdef.h" included with session
#include "Session.h" 
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib, "commctrl.lib")
#pragma comment(lib, "ws2_32.lib")

//Pull these out into a resource filw
#define IDB_RADIO1 301
#define IDB_RADIO2 302

struct Globals
{
	HINSTANCE hInstance;
	HWND hwnd;
	HDC hdc;
	int width, height;
};

Globals g;

HWND hWndDisplay;
HWND hWndBuffer;
HWND hWndPktInfo;
static int i = 10;
char* text = NULL;
Session* gsession;
//static int pktInfoBufferOffset;

bool init = false;
bool messageToBeSent = false;

LRESULT CALLBACK
WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
        case WM_CREATE:
        {
            //maybe put the buttons and window creation here
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWndDisplay, &ps);
            EndPaint(hwnd, &ps);
        }
        break;
		
        case WM_COMMAND:
        {
            switch (LOWORD(wparam))
            {
                case 101://send command button
                {
                    HDC hdc = GetDC(hWndPktInfo);
                    char edittxt[1024];
                    auto filedit = GetDlgItem(hWndBuffer, 200);
                    int editlength = GetWindowTextLength(filedit);
                    GetWindowText(hWndBuffer, edittxt, 1024);

                    RECT rc;
                    GetClientRect(hWndPktInfo, &rc);
                    ExtTextOut(hdc, rc.left, rc.top, ETO_OPAQUE, &rc, 0, 0, 0);

                    text = edittxt;
                    rc.top += pktInfoBufferOffset;

                    gsession->GetBuffer()[currentCharNumb] = (char*)malloc(sizeof(text));
                    strcpy(gsession->GetBuffer()[currentCharNumb], text);
                    ++currentCharNumb;

                    DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);
                    ReleaseDC(hWndDisplay, hdc);

                    messageToBeSent = true;
					//TODO(marko) : decide which way is better direct send or delagted
                    //std::string a = edittxt;
                    //SendPacket(a);
                }
                break;

                case 102://Init button
                {
                    HDC hdc = GetDC(hWndPktInfo);
                    char edittxt[1024];

                    RECT rc;
                    GetClientRect(hWndPktInfo, &rc);
                    ExtTextOut(hdc, rc.left, rc.top, ETO_OPAQUE, &rc, 0, 0, 0);
                    if (init)
                    {
                        text = "Already Initialized";
                        return 0;
                    }
                    else
                    {
                        text = "Begin Initialization";
                        init = true;
                    }
                    rc.top += pktInfoBufferOffset;
					pktInfoBufferOffset += 20;
			
                    gsession->GetBuffer()[currentCharNumb] = (char*)malloc(sizeof(text));
                    strcpy(gsession->GetBuffer()[currentCharNumb], text);
                    ++currentCharNumb;

                    DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);

                    int dataComState = gsession->StartDataCom();
                    if (dataComState == -1)
                    {
                        text = "Data communication start failed";
                        rc.top += pktInfoBufferOffset;

                        gsession->GetBuffer()[currentCharNumb] = (char*)malloc(sizeof(text));
                        strcpy(gsession->GetBuffer()[currentCharNumb], text);
                        ++currentCharNumb;

                        DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);
                    }

                    ReleaseDC(hWndPktInfo, hdc);
					UpdateWindow(g.hwnd);
                }
                break;

				//TODO(marko) : add the other buttons and settings options
                case IDB_RADIO1:
                {
                    switch (HIWORD(wparam))
                    {
                        case BN_CLICKED:
                        {
                            if (SendDlgItemMessage(g.hwnd, IDB_RADIO1, BM_GETCHECK, 0, 0) == 0) 
                            {
                                SendDlgItemMessage(g.hwnd, IDB_RADIO1, BM_SETCHECK, 1, 0);
                                SendDlgItemMessage(g.hwnd, IDB_RADIO2, BM_SETCHECK, 0, 0);
                            }
                        }
                        break; 
                    }
                }
                break;

                case IDB_RADIO2:
                {
                    switch (HIWORD(wparam))
                    {
                        case BN_CLICKED:
                            if (SendDlgItemMessage(g.hwnd, IDB_RADIO2, BM_GETCHECK, 0, 0) == 0) 
                            {
                                SendDlgItemMessage(g.hwnd, IDB_RADIO2, BM_SETCHECK, 1, 0);
                                SendDlgItemMessage(g.hwnd, IDB_RADIO1, BM_SETCHECK, 0, 0);
                            }
                            break; 
                    }
                }
                break;
            }                
        }
        break;

        case WM_MOVE:
        {
        }
        break;

        case WM_CLOSE:
        {
            DestroyWindow(g.hwnd);
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;

        default:
        {
            return DefWindowProc(hwnd, message, wparam, lparam);
        }
        break;
	}
}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Start Session
	Session session;
	gsession = &session;

	//Build the window and elements from here on out
	g.hInstance = hInstance;

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Motion Camera Client";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	RECT rect;
	SetRect(&rect, 50, 50, 1000, 800);

	g.width = rect.right - rect.left;
	g.height = rect.bottom - rect.top;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	g.hwnd = CreateWindowEx(WS_EX_STATICEDGE, "Motion Camera Client", "Drone Communication - Server",
		                  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		                  rect.right - rect.left, rect.bottom - rect.top,
		                  NULL, NULL, hInstance, NULL);

	if (g.hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//TODO(marko) : drag out button and display creation to seperate function instead
	HWND hwndSend = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Send"),
		                         WS_VISIBLE | WS_CHILD | WS_BORDER | BS_FLAT,
		                         800, 10, 125, 40,
		                         g.hwnd, (HMENU)101, NULL, NULL);

	HWND hwndInit = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Initialize"),
		                         WS_VISIBLE | WS_CHILD | WS_BORDER | BS_FLAT,
		                         800, 685, 125, 40,
		                         g.hwnd, (HMENU)102, NULL, NULL);

	HWND hwndSettingSend = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Save Settings"),
		                                WS_VISIBLE | WS_CHILD | WS_BORDER | BS_FLAT,
		                                10, 685, 125, 40,
		                                g.hwnd, (HMENU)103, NULL, NULL);

	HWND hwndGroupBox = CreateWindowEx(0, TEXT("STATIC"),//TEXT("Button"), 
		                             TEXT("Settings"),
		                             WS_VISIBLE | WS_CHILD | WS_THICKFRAME,
		                             10, 200, 450, 470,
		                             g.hwnd, (HMENU)101, NULL, NULL);

    //Settings radio buttons
	HWND hWndVideoQuality = CreateWindowEx(0, "BUTTON", "Video Quality",
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
		25, 250, 420, 50,
		g.hwnd, (HMENU)NULL, g.hInstance,
		NULL);

    HWND hWndVQButton1 = CreateWindowEx(0, "BUTTON", "1080p", 
                                      WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                                      35, 265, 50, 25,//10, 10, 80, 20,
                                      g.hwnd, (HMENU)IDB_RADIO1, g.hInstance,
                                      NULL);

    HWND hWndVQButton2 = CreateWindowEx(0, "BUTTON", "720p",
                                      WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                                      95, 265, 50, 25,//10, 10, 80, 20,
                                      g.hwnd, (HMENU)IDB_RADIO2, g.hInstance,
                                      NULL);
	HWND hWndVQButton3 = CreateWindowEx(0, "BUTTON", "480p",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		155, 265, 50, 25,
		g.hwnd, (HMENU)303, g.hInstance,
		NULL);
	
	HWND hWndVQButton4 = CreateWindowEx(0, "BUTTON", "360p",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		215, 265, 50, 25,
		g.hwnd, (HMENU)303, g.hInstance,
		NULL);

	//Filter
	HWND hWndVideoFilter = CreateWindowEx(0, "BUTTON", "Video Filter",
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
		25, 300, 420, 50,
		g.hwnd, (HMENU)NULL, g.hInstance,
		NULL);

	HWND hWndVFButton1 = CreateWindowEx(0, "BUTTON", "Normal",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		35, 315, 75, 25,
		g.hwnd, (HMENU)304, g.hInstance,
		NULL);

	HWND hWndVFButton2 = CreateWindowEx(0, "BUTTON", "Black and whtie",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		125, 315, 125, 25,
		g.hwnd, (HMENU)305, g.hInstance,
		NULL);
	HWND hWndVFButton3 = CreateWindowEx(0, "BUTTON", "etc",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		255, 315, 50, 25,
		g.hwnd, (HMENU)306, g.hInstance,
		NULL);

	// Create the list-view window in report view with label editing enabled.
	hWndDisplay = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("ListBox"), TEXT("Display"),
								WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_HASSTRINGS | LBS_STANDARD,
								475, 200, 450, 475,
								g.hwnd, (HMENU)201, NULL, NULL);

	// Create seperate thread to retrieve vid files and update window
	// Test to ensure display is correct
	char* letter = "test-10:00-02-08-2017.mp4";
	SendMessage(hWndDisplay, LB_ADDSTRING, 0, (LPARAM)letter);
	letter = "test-11:25-01-06-2017.mp4";
	SendMessage(hWndDisplay, LB_ADDSTRING, 1, (LPARAM)letter);
	letter = "test-23:45-01-03-2017.mp4";
	SendMessage(hWndDisplay, LB_ADDSTRING, 2, (LPARAM)letter);
	

	hWndBuffer = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), NULL,
		                        WS_CHILD | WS_VISIBLE,
		                        10, 10, 775, 40,
		                        g.hwnd, (HMENU)200, NULL, NULL);

	hWndPktInfo = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Static"), NULL,
								WS_CHILD | WS_VISIBLE | WS_VSCROLL,
								10, 60, 775, 115,
								g.hwnd, (HMENU)202, NULL, NULL);

	ShowWindow(g.hwnd, nCmdShow);
	
	UpdateWindow(hWndDisplay);
	UpdateWindow(g.hwnd);

	g.hdc = GetDC(g.hwnd);

	MSG msg = {};

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (init && messageToBeSent)
		{
			gsession->SendPacket(text);
			messageToBeSent = false;
			
			gsession->RecievePacket(&hWndPktInfo, &g.hwnd);
			
			UpdateWindow(g.hwnd);
		}
	}

    return 0;
}

