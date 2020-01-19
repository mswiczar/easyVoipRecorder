#pragma once
#include "windows.h"
#include "psapi.h"

#define size_wchar_heap 1024*1024

#pragma comment ( lib, "psapi.lib") 

#define EasyVoipRecorder "easyVoipRecorder"
typedef struct _LIST_PROCESS
{
	DWORD  Pid;
	DWORD  tid;
	WCHAR  ProcessName[512];
	HWND   aHwnd;
	WCHAR  Caption[512];
	WCHAR  Class[512];
	bool   reconding;
	int DS;
	WCHAR filenameWorking[512];

}LIST_PROCESS, *PLIST_PROCESS;

#define MSG_REC_START    WM_USER + 9002
#define MSG_REC_STOP     WM_USER + 9003
#define MSG_REC_PAUSE    WM_USER + 9004
#define MSG_REC_CONTINUE WM_USER + 9005

#define MSG_BEGIN WM_USER + 9010
#define MSG_END   WM_USER + 9011


#define MSG_MICBEGIN   WM_USER + 9012
#define MSG_MICEND   WM_USER + 9013

#define MSG_NEWAPP   WM_USER + 9014
#define MSG_DELAPP   WM_USER + 9015

#define MSG_GETSOURCE   WM_USER + 9020

#define MSG_SPEAKERDATA   WM_USER + 9030
#define MSG_MICDATA   WM_USER + 9040

//#define MSG_FIX   WM_USER + 9050

#define MSG_DS WM_USER + 9100 		

#define MSG_PATCHED WM_USER + 9110



class CMonitor
{
public:
	WCHAR lpprocess[size_wchar_heap];
	HWND ahwndWIndowClient;
	CMonitor(HDESK aparam);
	virtual ~CMonitor(void);
	void CheckApp(void);
	WNDCLASSEXW wcex;
	HWND ahwndWindowSelf;
	HANDLE MytrheadWin;
	HDESK myhDeskUser;

private:
	void InsertIntoList(PLIST_PROCESS adata);
	bool PrintProcessNameAndID( PLIST_PROCESS  alistprocess);

	void LoadHook(void);
	void UnLoadHook(void);


	bool searchinProcessIni(WCHAR* aproc);
	bool SearchIfInList(PLIST_PROCESS  alistprocess);
	bool InsertInList(PLIST_PROCESS  alistprocess);
	void RemoveFromList(PLIST_PROCESS  alistprocess);

	void RemoveAll(void);

	void GetHwndfromPid(PLIST_PROCESS  alistprocess);

	

};
