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
#include <wchar.h>
#include <stdlib.h>
#include <errno.h>
//#include "pktdef.h" included with session
#include "Session.h" 
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib, "commctrl.lib")
#pragma comment(lib, "ws2_32.lib")

//Pull these out into a resource filw
//Video Quality
#define IDB_VQRADIO1 301
#define IDB_VQRADIO2 302
#define IDB_VQRADIO3 303
#define IDB_VQRADIO4 304

//Filter
#define IDB_FRADIO1 311
#define IDB_FRADIO2 312
#define IDB_FRADIO3 313

//Rotation
#define IDB_RRADIO1 321
#define IDB_RRADIO2 322
#define IDB_RRADIO3 323
#define IDB_RRADIO4 324

//Framerate
#define IDB_FRRADIO1 331
#define IDB_FRRADIO2 332
#define IDB_FRRADIO3 333
#define IDB_FRRADIO4 334

//Brightness
#define IDB_BRADIO1 341
#define IDB_BRADIO2 342
#define IDB_BRADIO3 343
#define IDB_BRADIO4 344
#define IDB_BRADIO5 345
//End of window menu defines

struct MainWind
{
	HINSTANCE hInstance;
	HWND hwnd;
	HDC hdc;
	int width, height;
};
MainWind g;

HWND hWndDisplay;
HWND hWndBuffer;
HWND hWndPktInfo;
char* text = NULL;
Session* gsession;

bool init = false;
bool messageToBeSent = false;

/*
inline static
void TextToHWND(RECT* rc, char* strdis)
{
	rc.top += pktInfoBufferOffset;
	pktInfoBufferOffset += 20;
	DrawTextA(hdc, strdis, strlen(strdis), &rc, DT_TOP | DT_LEFT);
}
*/

inline static
HWND CreateGroupBox(char* gname, int ypos)
{
	//TODO(marko) : add error checking
	return CreateWindowEx(0, "BUTTON", gname,
		                  WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
		                  25, ypos, 420, 50,
		                  g.hwnd, (HMENU)NULL, g.hInstance,
		                  NULL);
}

inline static
HWND CreateRadioButton(char* bname, int xpos, int ypos, int sxpos, int sypos, int id)
{
	//TODO(marko) : add error checking
	return CreateWindowEx(0, "BUTTON", bname,
		                  WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		                  xpos, ypos, sxpos, sypos,
		                  g.hwnd, (HMENU)id, g.hInstance,
		                  NULL);
}

LRESULT CALLBACK
WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
        case WM_CREATE:
        {	
            
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
                    //HDC hdc = GetDC(hWndPktInfo);
                    char edittxt[1024];
                    int editlength = GetWindowTextLength(GetDlgItem(hWndBuffer, 200));
                    GetWindowText(hWndBuffer, edittxt, 1024);

                    //RECT rc;
                    //GetClientRect(hWndPktInfo, &rc);
                    //ExtTextOut(hdc, rc.left, rc.top, ETO_OPAQUE, &rc, 0, 0, 0);

                    text = edittxt;
                    //rc.top += pktInfoBufferOffset;

                    //DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);
                    //ReleaseDC(hWndDisplay, hdc);
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
                    //rc.top += pktInfoBufferOffset;
					//pktInfoBufferOffset += 20;
                    DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);

                    int dataComState = gsession->StartDataCom();
                    if (dataComState == -1)
                    {
						//When static scroll is added switch to this
						//TextToHWND(rc, "Data communication start failed");

                        text = "Data communication start failed";
                        //rc.top += pktInfoBufferOffset;
						DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);
                    }
					else
					{
						text = "Waiting for client connection";
						//rc.top += pktInfoBufferOffset;
						DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);

						text = "\nCommands\n   0 - sends packet\n   1 - sends sleep packet (terminates connection)\n   2 - sends ack packet";
						//rc.top += pktInfoBufferOffset;
						DrawTextA(hdc, text, strlen(text), &rc, DT_TOP | DT_LEFT);
					}

                    ReleaseDC(hWndPktInfo, hdc);
					UpdateWindow(g.hwnd);
                }
                break;

				//Make a function handle a group of radio buttons instead of expanding in the switch statement
				//TODO(marko) : add the other buttons and settings options
                case IDB_VQRADIO1:
                {
                    switch (HIWORD(wparam))
                    {
                        case BN_CLICKED:
                        {
                            if (SendDlgItemMessage(g.hwnd, IDB_VQRADIO1, BM_GETCHECK, 0, 0) == 0)
                            {
                                SendDlgItemMessage(g.hwnd, IDB_VQRADIO1, BM_SETCHECK, 1, 0);
                                SendDlgItemMessage(g.hwnd, IDB_VQRADIO2, BM_SETCHECK, 0, 0);
                            }
                        }
                        break; 
                    }
                }
                break;

                case IDB_VQRADIO2:
                {
                    switch (HIWORD(wparam))
                    {
                        case BN_CLICKED:
                            if (SendDlgItemMessage(g.hwnd, IDB_VQRADIO2, BM_GETCHECK, 0, 0) == 0)
                            {
                                SendDlgItemMessage(g.hwnd, IDB_VQRADIO2, BM_SETCHECK, 1, 0);
                                SendDlgItemMessage(g.hwnd, IDB_VQRADIO1, BM_SETCHECK, 0, 0);
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
	Session session;
	gsession = &session;
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

	g.hwnd = CreateWindowEx(WS_EX_STATICEDGE, "Motion Camera Client", "Drone Communication - Client",
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

	HWND hwndGroupBox = CreateWindowEx(0, TEXT("STATIC"), TEXT("Settings"),
		                               WS_VISIBLE | WS_CHILD | WS_THICKFRAME,
		                               10, 200, 450, 470,
		                               g.hwnd, (HMENU)104, NULL, NULL);


	//Settings radio buttons Video Quality
	HWND hWndVideoQuality = CreateGroupBox("Video Quality", 235);
	HWND hWndVQButton1 = CreateRadioButton("1080p", 35, 250, 50, 25, IDB_VQRADIO1);
	HWND hWndVQButton2 = CreateRadioButton("720p", 95, 250, 50, 25, IDB_VQRADIO2);
	HWND hWndVQButton3 = CreateRadioButton("480p", 155, 250, 50, 25, IDB_VQRADIO3);
	HWND hWndVQButton4 = CreateRadioButton("480p", 215, 250, 50, 25, IDB_VQRADIO4);

	//Filter
	HWND hWndVideoFilter = CreateGroupBox("Video Filter", 300);
	HWND hWndVFButton1 = CreateRadioButton("Normal", 35, 315, 75, 25, IDB_FRADIO1);
	HWND hWndVFButton2 = CreateRadioButton("Black and white", 125, 315, 125, 25, IDB_FRADIO2);
	HWND hWndVFButton3 = CreateRadioButton("Negative", 255, 315, 75, 25, IDB_FRADIO3);
	
	//Rotation
	HWND hWndVideoRotation = CreateGroupBox("Video Rotation", 365);
	HWND hWndRButton1 = CreateRadioButton("0", 35, 380, 50, 25, IDB_RRADIO1);
	HWND hWndRButton2 = CreateRadioButton("90", 95, 380, 50, 25, IDB_RRADIO2);
	HWND hWndRButton3 = CreateRadioButton("180", 155, 380, 50, 25, IDB_RRADIO3);
	HWND hWndRButton4 = CreateRadioButton("270", 215, 380, 50, 25, IDB_RRADIO4);

	//Framerates
	HWND hWndFrameRate = CreateGroupBox("Frame rate (fps)", 430);
	HWND hWndFRButton1 = CreateRadioButton("15", 35, 445, 50, 25, IDB_FRRADIO1);
	HWND hWndFRButton2 = CreateRadioButton("24", 95, 445, 50, 25, IDB_FRRADIO2);
	HWND hWndFRButton3 = CreateRadioButton("28", 155, 445, 50, 25, IDB_FRRADIO3);
	HWND hWndFRButton4 = CreateRadioButton("30", 215, 445, 50, 25, IDB_FRRADIO4);

	//Brigthness
	HWND hWndBrightness = CreateGroupBox("Brightness", 495);
    HWND hWndBButton1 = CreateRadioButton("0", 35, 510, 50, 25, IDB_BRADIO1);
	HWND hWndBButton2 = CreateRadioButton("25", 95, 510, 50, 25, IDB_BRADIO2);
	HWND hWndBButton3 = CreateRadioButton("50", 155, 510, 50, 25, IDB_BRADIO3);
	HWND hWndBButton4 = CreateRadioButton("75", 215, 510, 50, 25, IDB_BRADIO4);
	HWND hWndBButton5 = CreateRadioButton("100", 275, 510, 50, 25, IDB_BRADIO5);

	// Create the list-view window in report view with label editing enabled.
	hWndDisplay = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("ListBox"), TEXT("Display"),
								 WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_HASSTRINGS | LBS_STANDARD,
								 475, 200, 450, 475,
								 g.hwnd, (HMENU)105, NULL, NULL);

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
		                        g.hwnd, (HMENU)106, NULL, NULL);

	//Add scroll support instead of overwriting the info
	hWndPktInfo = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Static"), NULL,
								 WS_CHILD | WS_VISIBLE | WS_VSCROLL,
								 10, 60, 775, 115,
								 g.hwnd, (HMENU)107, NULL, NULL);

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
			
			gsession->RecievePacket(&hWndPktInfo, &g.hwnd, &hWndBuffer);
			
			UpdateWindow(g.hwnd);
		}
	}

	ReleaseDC(g.hwnd, g.hdc);
    return 0;
}

