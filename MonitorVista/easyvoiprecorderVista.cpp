// easyvoiprecorderVista.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "../service/Monitor.h"
#include "easyvoiprecorderVista.h"
#include "../interface/helper.h"



CMonitor * elmonitor;

void CALLBACK TimerProc(HWND, UINT, UINT, DWORD);



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	// Perform application initialization:
	HANDLE Mutex;
	Mutex =CreateMutexW(NULL,true,MRecorderMonitor); 
	if (((Mutex != 0) && (GetLastError() == 0))==false)
	{
		TerminateProcess(GetCurrentProcess(),0);
	}		

	elmonitor = new CMonitor(NULL);
	::Sleep(1000);
	//elmonitor->CheckApp();

	UINT id = SetTimer(NULL, 0, 5000, (TIMERPROC) TimerProc);
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
	}
	delete elmonitor;
	KillTimer(NULL, id);


	return (int) msg.wParam;
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime) 
{
	elmonitor->CheckApp();

}

