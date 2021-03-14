/***

tetris v1.0
Copyright (C) 2021 Yuri213212
Site:https://github.com/Yuri213212/tetris
Email: yuri213212@vip.qq.com
License: CC BY-NC-SA 4.0
https://creativecommons.org/licenses/by-nc-sa/4.0/

***/

#define UNICODE
#define _UNICODE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#ifdef LANG_EN
#include "lang_en/tetris_ui.h"
#include "lang_en/tetris_help.h"
#endif
#ifdef LANG_CH
#include "lang_ch/tetris_ui.h"
#include "lang_ch/tetris_help.h"
#endif
#ifdef LANG_JP
#include "lang_jp/tetris_ui.h"
#include "lang_jp/tetris_help.h"
#endif

#define tbuflen		1024
#define clwidth		288
#define clheight	370
#define SampleRate	44100
#define BufferLength	2940	//15fps

enum menuEnum{
	MENU_Reset=0x8001,
	MENU_Start,
	MENU_Pause,
	MENU_Help,
};

wchar_t wbuf[tbuflen];
int cxScreen,cyScreen,width,height,vol;
HDC hdcMem;
HBRUSH hBrushBg,hBrushRed,hBrushOrange,hBrushYellow,hBrushGreen,hBrushCyan,hBrushBlue,hBrushPurple;
RECT bgRect={0,0,clwidth,clheight},rect={199,0,279,0};
short outBuffer[2][BufferLength]={};
WAVEHDR WaveHdr[2]={
	{(LPSTR)outBuffer[0],BufferLength*2,0,0,0,0,0,0},
	{(LPSTR)outBuffer[1],BufferLength*2,0,0,0,0,0,0}
};
WAVEFORMATEX waveformat={WAVE_FORMAT_PCM,1,SampleRate,SampleRate*2,2,16,0};

#include "defproc.h"
#include "drawicon.h"
#include "sound.h"
#include "board.h"

void dotmain(HDC hdc,int x,int y){
	Rectangle(hdcMem,x*18+6,348-y*18,x*18+23,365-y*18);
	SelectObject(hdcMem,GetStockObject(GRAY_BRUSH));
	Rectangle(hdcMem,x*18+10,352-y*18,x*18+19,361-y*18);
}

void dotnext(HDC hdc,int x,int y){
	Rectangle(hdcMem,x*18+204,96-y*18,x*18+221,113-y*18);
	SelectObject(hdcMem,GetStockObject(GRAY_BRUSH));
	Rectangle(hdcMem,x*18+208,100-y*18,x*18+217,109-y*18);
}

void redraw(){
	int i,j,temp;

	FillRect(hdcMem,&bgRect,hBrushBg);
	swprintf(wbuf,L"%d",score);
	rect.top=186;
	rect.bottom=202;
	DrawTextW(hdcMem,wbuf,-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_RIGHT|DT_VCENTER);
	rect.top=240;
	rect.bottom=256;
	DrawTextW(hdcMem,szUI_Modes[mode],-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	swprintf(wbuf,L"%d",level);
	rect.top=294;
	rect.bottom=310;
	DrawTextW(hdcMem,wbuf,-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_RIGHT|DT_VCENTER);
	if (pause){
		rect.top=348;
		rect.bottom=364;
		DrawTextW(hdcMem,szMenu_Pause,-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	for (i=2;i<22;++i){
		for (j=4;j<14;++j){
			if ((fullline&1)&&(fullline&(1<<i))) continue;
			if (board[i]&(1<<j)){
				SelectObject(hdcMem,hBrushRed);
				dotmain(hdcMem,j-4,i-2);
			}
		}
	}
	if (gameover>0){
		if (gameover<20){
			temp=gameover+2;
		}else{
			temp=42-gameover;
		}
		for (i=2;i<temp;++i){
			for (j=4;j<14;++j){
				SelectObject(hdcMem,hBrushRed);
				dotmain(hdcMem,j-4,i-2);
			}
		}
	}
	if (next>=0){
		for (i=0;i<4;++i){
			for (j=0;j<4;++j){
				if (piece[next*4+i]&(1<<j)){
					switch (next>>2){
					case 15:
					case 14:
					case 13:
						SelectObject(hdcMem,hBrushPurple);
						break;
					case 12:
						SelectObject(hdcMem,hBrushGreen);
						break;
					default:
						SelectObject(hdcMem,hBrushBlue);
						break;
					}
					dotnext(hdcMem,j,i);
				}
			}
		}
	}
	if (bulletRowD>=1&&bulletRowD<21){
		SelectObject(hdcMem,hBrushCyan);
		dotmain(hdcMem,bulletShiftD-3,bulletRowD-1);
	}
	if (bulletRowA>=1&&bulletRowA<21){
		SelectObject(hdcMem,GetStockObject(BLACK_BRUSH));
		dotmain(hdcMem,bulletShiftA-3,bulletRowA-1);
	}
	if (current>=0){
		for (i=0;i<4;++i){
			if (i+row>=22) continue;
			if (i+row<2) continue;
			for (j=0;j<4;++j){
				if (piece[current*4+i]&(1<<j)){
					switch (current>>2){
					case 16:
						SelectObject(hdcMem,hBrushOrange);
						break;
					case 15:
					case 14:
					case 13:
						SelectObject(hdcMem,hBrushPurple);
						break;
					case 12:
						if (board_hittest(current,row,shift)){
							SelectObject(hdcMem,hBrushYellow);
						}else{
							SelectObject(hdcMem,hBrushGreen);
						}
						break;
					default:
						SelectObject(hdcMem,hBrushBlue);
						break;
					}
					dotmain(hdcMem,j+shift-4,i+row-2);
				}
			}
		}
	}
}

LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
	static int endflag=0;
	static HWAVEOUT hwo=NULL;
	static SCROLLINFO vsi={sizeof(SCROLLINFO),SIF_DISABLENOSCROLL|SIF_PAGE|SIF_POS|SIF_RANGE,0,271,16,192,0};
	static HBITMAP hBitmap;

	HDC hdc;

	switch (message){
	case WM_CREATE:
		vol=64;
		hBrushRed=CreateSolidBrush(RGB(255,0,0));
		hBrushOrange=CreateSolidBrush(RGB(255,128,0));
		hBrushYellow=CreateSolidBrush(RGB(255,255,0));
		hBrushGreen=CreateSolidBrush(RGB(0,255,0));
		hBrushCyan=CreateSolidBrush(RGB(0,255,255));
		hBrushBlue=CreateSolidBrush(RGB(0,0,255));
		hBrushPurple=CreateSolidBrush(RGB(128,0,255));
		hdc=GetDC(NULL);
		hdcMem=CreateCompatibleDC(hdc);
		hBitmap=CreateCompatibleBitmap(hdc,clwidth,clheight);
		ReleaseDC(NULL,hdc);
		SelectObject(hdcMem,hBitmap);
		FillRect(hdcMem,&bgRect,GetStockObject(WHITE_BRUSH));
		SelectObject(hdcMem,GetStockObject(NULL_PEN));
		SelectObject(hdcMem,GetStockObject(BLACK_BRUSH));
		Rectangle(hdcMem,0,0,191,371);
		Rectangle(hdcMem,198,36,281,119);
		SelectObject(hdcMem,GetStockObject(WHITE_BRUSH));
		Rectangle(hdcMem,4,4,187,367);
		Rectangle(hdcMem,202,40,277,115);
		SelectObject(hdcMem,GetStockObject(SYSTEM_FIXED_FONT));
		SetBkMode(hdcMem,TRANSPARENT);
		SetTextColor(hdcMem,RGB(0,0,0));
		rect.top=6;
		rect.bottom=22;
		DrawTextW(hdcMem,szUI_Next,-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		rect.top=168;
		rect.bottom=184;
		DrawTextW(hdcMem,szUI_Score,-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_VCENTER);
		rect.top=222;
		rect.bottom=238;
		DrawTextW(hdcMem,szUI_Mode,-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_VCENTER);
		rect.top=276;
		rect.bottom=292;
		DrawTextW(hdcMem,szUI_Level,-1,&rect,DT_NOPREFIX|DT_SINGLELINE|DT_LEFT|DT_VCENTER);
		hBrushBg=CreatePatternBrush(hBitmap);
		SetScrollInfo(hwnd,SB_VERT,&vsi,TRUE);
		SendMessage(hwnd,WM_COMMAND,MENU_Reset,0);
		if (waveOutOpen(&hwo,WAVE_MAPPER,&waveformat,(DWORD_PTR)hwnd,0,CALLBACK_WINDOW)!=MMSYSERR_NOERROR){
			MessageBoxW(NULL,szErr_AudioDevice,szTitle,MB_ICONERROR);
			DestroyWindow(hwnd);
			return 0;
		}
		waveOutPrepareHeader(hwo,&WaveHdr[0],sizeof(WAVEHDR));
		waveOutPrepareHeader(hwo,&WaveHdr[1],sizeof(WAVEHDR));
		SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
		return 0;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
		redraw();
		hdc=GetDC(hwnd);
		BitBlt(hdc,0,0,clwidth,clheight,hdcMem,0,0,SRCCOPY);
		ReleaseDC(hwnd,hdc);
		ValidateRect(hwnd,NULL);
		return 0;
	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam)<0){
			vol-=16;
		}else{
			vol+=16;
		}
		if (vol>256){
			vol=256;
		}
		if (vol<0){
			vol=0;
		}
		SetScrollPos(hwnd,SB_VERT,256-vol,TRUE);
		return 0;
	case WM_VSCROLL:
		switch(LOWORD(wParam)){
		case SB_TOP:
			vol=256;
			break;
		case SB_BOTTOM:
			vol=0;
			break;
		case SB_LINEUP:
			vol+=1;
			break;
		case SB_PAGEUP:
			vol+=16;
			break;
		case SB_LINEDOWN:
			vol-=1;
			break;
		case SB_PAGEDOWN:
			vol-=16;
			break;
		case SB_THUMBTRACK:
			vol=256-HIWORD(wParam);
			break;
		default:
			return 0;
		}
		if (vol>256){
			vol=256;
		}
		if (vol<0){
			vol=0;
		}
		SetScrollPos(hwnd,SB_VERT,256-vol,TRUE);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case MENU_Reset:
			sound_init();
			board_init();
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		case MENU_Start:
			board_newgame();
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		case MENU_Pause:
			pause^=1;
			sound_event(ST_pause);
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		case MENU_Help:
			pause|=2;
			InvalidateRect(hwnd,NULL,FALSE);
			MessageBoxW(hwnd,szHelp,szTitle,MB_ICONINFORMATION);
			pause&=~2;
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam){
		case VK_LEFT:
			board_left();
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		case VK_RIGHT:
			board_right();
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		case VK_UP:
			board_rotate();
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		case VK_DOWN:
			board_accelerate();
			InvalidateRect(hwnd,NULL,FALSE);
			return 0;
		case VK_PRIOR://Page Up
			SendMessage(hwnd,WM_VSCROLL,SB_PAGEUP,0);
			return 0;
		case VK_NEXT://Page Down
			SendMessage(hwnd,WM_VSCROLL,SB_PAGEDOWN,0);
			return 0;
		case VK_HOME:
			SendMessage(hwnd,WM_VSCROLL,SB_TOP,0);
			return 0;
		case VK_END:
			SendMessage(hwnd,WM_VSCROLL,SB_BOTTOM,0);
			return 0;
		case VK_RETURN://Enter
			SendMessage(hwnd,WM_COMMAND,MENU_Start,0);
			return 0;
		case VK_ESCAPE://Esc
			SendMessage(hwnd,WM_COMMAND,MENU_Reset,0);
			return 0;
		case ' ':
			SendMessage(hwnd,WM_COMMAND,MENU_Pause,0);
			return 0;
		default:
			return 0;
		}
		return 0;
	case MM_WOM_OPEN:
		waveOutWrite(hwo,&WaveHdr[0],sizeof(WAVEHDR));
		waveOutWrite(hwo,&WaveHdr[1],sizeof(WAVEHDR));
		return 0;
	case MM_WOM_DONE:
		if (endflag){
			waveOutClose(hwo);
		}else{
			FillBuffer((short *)((PWAVEHDR)lParam)->lpData);
			waveOutWrite(hwo,(PWAVEHDR)lParam,sizeof(WAVEHDR));
			board_next();
			InvalidateRect(hwnd,NULL,FALSE);
		}
		return 0;
	case MM_WOM_CLOSE:
		waveOutUnprepareHeader(hwo,&WaveHdr[0],sizeof(WAVEHDR));
		waveOutUnprepareHeader(hwo,&WaveHdr[1],sizeof(WAVEHDR));
		DestroyWindow(hwnd);
		return 0;
	case WM_CLOSE:
		endflag=1;
		waveOutReset(hwo);
		return 0;
	case WM_DESTROY:
		SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
		KillTimer(hwnd,1);
		DeleteObject(hBrushBg);
		DeleteObject(hBrushRed);
		DeleteObject(hBrushOrange);
		DeleteObject(hBrushYellow);
		DeleteObject(hBrushGreen);
		DeleteObject(hBrushCyan);
		DeleteObject(hBrushBlue);
		DeleteObject(hBrushPurple);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return myDefWindowProc(hwnd,message,wParam,lParam);
}

int main(){
	int argc;
	wchar_t **argv;
	HINSTANCE hInstance;
	int iCmdShow;
	STARTUPINFOW si;

	HICON hIcon;
	WNDCLASSW wndclass;
	HMENU hMenu;
	HWND hwnd;
	MSG msg;
	LARGE_INTEGER l;

	argv=CommandLineToArgvW(GetCommandLineW(),&argc);
	hInstance=GetModuleHandleW(NULL);
	GetStartupInfoW(&si);
	iCmdShow=si.dwFlags&STARTF_USESHOWWINDOW?si.wShowWindow:SW_SHOWNORMAL;

	hIcon=drawIcon();
	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc=WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance=hInstance;
	wndclass.hIcon=hIcon;
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName=szAppName;
	wndclass.lpszClassName=szAppName;
	if (!RegisterClassW(&wndclass)){
		MessageBoxW(NULL,szErr_RegisterClass,argv[0],MB_ICONERROR);
		DestroyIcon(hIcon);
		return 0;
	}

	QueryPerformanceCounter(&l);
	srand(l.LowPart%RAND_MAX+1);
	cxScreen=GetSystemMetrics(SM_CXSCREEN);
	cyScreen=GetSystemMetrics(SM_CYSCREEN);
	width=clwidth+GetSystemMetrics(SM_CXFIXEDFRAME)*2+GetSystemMetrics(SM_CXVSCROLL);
	height=clheight+GetSystemMetrics(SM_CYFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU);
	hMenu=CreateMenu();
	AppendMenuW(hMenu,MF_STRING,MENU_Reset,szMenu_Reset);
	AppendMenuW(hMenu,MF_STRING,MENU_Start,szMenu_Start);
	AppendMenuW(hMenu,MF_STRING,MENU_Pause,szMenu_Pause);
	AppendMenuW(hMenu,MF_STRING,MENU_Help,szMenu_Help);
	hwnd=CreateWindowW(szAppName,szTitle,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VSCROLL,(cxScreen-width)/2,(cyScreen-height)/2,width,height,NULL,hMenu,hInstance,NULL);
	ShowWindow(hwnd,iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyIcon(hIcon);
	LocalFree(argv);
	return msg.wParam;
}
