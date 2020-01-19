#define UNICODE
#include "StdAfx.h"
#include ".\skypeapi.h"

#include "rpc.h"
#pragma comment ( lib, "Rpcrt4.lib") 


bool iTsOk=false;

bool firstime=true;

bool ConnectediTsOk(void)
{
	return iTsOk;
}


SkypeApi::SkypeApi(void)
{
	//Initializate();
}





// SkypeRecorder.cpp : Defines the entry point for the application.
//

static LRESULT APIENTRY SkypeAPITest_Windows_WindowProc(
	HWND hWindow, UINT uiMessage, WPARAM uiParam, LPARAM ulParam);

bool Initialize_CreateMainWindow(void);
void DeInitialize_DestroyMainWindow(void);
void DeInitialize_DestroyWindowClass(void);


#define tituloAplication L"tituloAplication"


HINSTANCE hInit_ProcessHandle;

UINT uiGlobal_MsgID_SkypeControlAPIAttach;
UINT uiGlobal_MsgID_SkypeControlAPIDiscover;

HWND hGlobal_SkypeAPIWindowHandle=NULL;
HWND hInit_MainWindowHandle;

HANDLE hGlobal_ThreadShutdownEvent;
bool Initialize_CreateWindowClass(void);

WCHAR acInit_WindowClassName[128];

#if defined(_DEBUG)
	bool volatile fGlobal_DumpWindowsMessages=true;
#else
	bool volatile fGlobal_DumpWindowsMessages=false;
#endif


enum {
	SKYPECONTROLAPI_ATTACH_SUCCESS=0,								// Client is successfully attached and API window handle can be found in wParam parameter
	SKYPECONTROLAPI_ATTACH_PENDING_AUTHORIZATION=1,	// Skype has acknowledged connection request and is waiting for confirmation from the user.
																									// The client is not yet attached and should wait for SKYPECONTROLAPI_ATTACH_SUCCESS message
	SKYPECONTROLAPI_ATTACH_REFUSED=2,								// User has explicitly denied access to client
	SKYPECONTROLAPI_ATTACH_NOT_AVAILABLE=3,					// API is not available at the moment. For example, this happens when no user is currently logged in.
																									// Client should wait for SKYPECONTROLAPI_ATTACH_API_AVAILABLE broadcast before making any further
																									// connection attempts.
	SKYPECONTROLAPI_ATTACH_API_AVAILABLE=0x8001
};




bool Initialize_CreateWindowClass(void)
	{
	WCHAR *paucUUIDString;
	RPC_STATUS lUUIDResult;
	bool fReturnStatus;
	UUID oUUID;

	fReturnStatus=false;
	lUUIDResult=UuidCreate(&oUUID);
	hInit_ProcessHandle=(HINSTANCE)OpenProcess( PROCESS_DUP_HANDLE, FALSE, GetCurrentProcessId());
	if( hInit_ProcessHandle!=NULL && (lUUIDResult==RPC_S_OK || lUUIDResult==RPC_S_UUID_LOCAL_ONLY) )
		{
		if( UuidToStringW( &oUUID, &paucUUIDString)==RPC_S_OK )
			{
			WNDCLASSW oWindowClass;
			ZeroMemory(acInit_WindowClassName,sizeof(acInit_WindowClassName));
			wcscat(acInit_WindowClassName, L"Skype-API-Test-");
			wcscat( acInit_WindowClassName, (WCHAR *)paucUUIDString);

			oWindowClass.style=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
			oWindowClass.lpfnWndProc=(WNDPROC)&SkypeAPITest_Windows_WindowProc;
			oWindowClass.cbClsExtra=0;
			oWindowClass.cbWndExtra=0;
			oWindowClass.hInstance=hInit_ProcessHandle;
			oWindowClass.hIcon=NULL;
			oWindowClass.hCursor=NULL;
			oWindowClass.hbrBackground=NULL;
			oWindowClass.lpszMenuName=NULL;
			oWindowClass.lpszClassName=acInit_WindowClassName;

			if( RegisterClassW(&oWindowClass)!=0 )
				fReturnStatus=true;

			RpcStringFreeW(&paucUUIDString);
			}
		}
	if( fReturnStatus==false )
	{
	}

		//CloseHandle(hInit_ProcessHandle),hInit_ProcessHandle=NULL;
	return(fReturnStatus);
	}


static LRESULT APIENTRY SkypeAPITest_Windows_WindowProc(
	HWND hWindow, UINT uiMessage, WPARAM uiParam, LPARAM ulParam)
	{
		WCHAR soutputstring[512];
		ZeroMemory(soutputstring,sizeof(soutputstring));
		bool fIssueDefProc=false;
		LRESULT lReturnCode=0;
		switch(uiMessage)
		{
			case WM_DESTROY:
				hInit_MainWindowHandle=NULL;
				//PostQuitMessage(0);
			break;
			case WM_COPYDATA:
			if( hGlobal_SkypeAPIWindowHandle==(HWND)uiParam )
				{
				char data[512];
				ZeroMemory(data,sizeof(data));
				PCOPYDATASTRUCT poCopyData=(PCOPYDATASTRUCT)ulParam;
				wsprintfA(data, "Message from Skype(%u): %.*s\n", poCopyData->dwData, poCopyData->cbData, poCopyData->lpData);
				OutputDebugStringA(data);

				lReturnCode=1;
				}
			break;
			
		default:
			if( uiMessage==uiGlobal_MsgID_SkypeControlAPIAttach )
				{
				switch(ulParam)
					{
					case SKYPECONTROLAPI_ATTACH_SUCCESS:
						wsprintfW(soutputstring,L"Connected");
						hGlobal_SkypeAPIWindowHandle=(HWND)uiParam;
						iTsOk=true;
				//MessageBoxW(0,soutputstring,tituloAplication,0);
						break;
					case SKYPECONTROLAPI_ATTACH_PENDING_AUTHORIZATION:
						//printf("!!! Pending authorization\n");
						wsprintfW(soutputstring,L"Refused");
						//MessageBoxW(0,soutputstring,tituloAplication,0);
						
						break;
					case SKYPECONTROLAPI_ATTACH_REFUSED:
						//printf("!!! Connection refused\n");
						wsprintfW(soutputstring,L"Refused");
						//MessageBoxW(0,soutputstring,tituloAplication,0);

						break;
					case SKYPECONTROLAPI_ATTACH_NOT_AVAILABLE:
						//printf("!!! Skype API not available\n");
						wsprintfW(soutputstring,L"Refused");
						//MessageBoxW(0,soutputstring,tituloAplication,0);
						firstime=true;
						break;
					case SKYPECONTROLAPI_ATTACH_API_AVAILABLE:
						//printf("!!! Try connect now (API available); issue #connect\n");
						wsprintfW(soutputstring,L"Refused");
						//MessageBoxW(0,soutputstring,tituloAplication,0);
						break;
					}
				lReturnCode=1;
				break;
				}
			fIssueDefProc=true;
			break;
		}
		if( fIssueDefProc )
			lReturnCode=DefWindowProc( hWindow, uiMessage, uiParam, ulParam);
		if( fGlobal_DumpWindowsMessages )
			{
				//printf( "WindowProc: hWindow=0x%08X, MainWindow=0x%08X, Message=%5u, WParam=0x%08X, LParam=0x%08X; Return=%ld%s\n",
				//hWindow, hInit_MainWindowHandle, uiMessage, uiParam, ulParam, lReturnCode, fIssueDefProc? " (default)":"");
			}
		return(lReturnCode);
};

bool Initialize_CreateMainWindow(void)
{
	hInit_MainWindowHandle=CreateWindowExW( WS_EX_APPWINDOW|WS_EX_WINDOWEDGE,
		acInit_WindowClassName, L"", WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 128, 128, NULL, 0, hInit_ProcessHandle, 0);
	return(hInit_MainWindowHandle!=NULL? true:false);
} 

void DeInitialize_DestroyMainWindow(void)
{
	if( hInit_MainWindowHandle!=NULL )
		DestroyWindow(hInit_MainWindowHandle),hInit_MainWindowHandle=NULL;
}
void DeInitialize_DestroyWindowClass(void)
{
	UnregisterClassW( acInit_WindowClassName, hInit_ProcessHandle);
	CloseHandle(hInit_ProcessHandle);
	hInit_ProcessHandle=NULL;
}



void SkypeApi::Initializate(void)
{
	if (firstime)
	{
		firstime=false;
		uiGlobal_MsgID_SkypeControlAPIAttach=RegisterWindowMessageW(L"SkypeControlAPIAttach");
		uiGlobal_MsgID_SkypeControlAPIDiscover=RegisterWindowMessageW(L"SkypeControlAPIDiscover");
		if( uiGlobal_MsgID_SkypeControlAPIAttach!=0 && uiGlobal_MsgID_SkypeControlAPIDiscover!=0 )
		{
			if( Initialize_CreateWindowClass() )
			{
				if( Initialize_CreateMainWindow() )
				{
					hGlobal_ThreadShutdownEvent=CreateEvent( NULL, TRUE, FALSE, NULL);
					if( hGlobal_ThreadShutdownEvent!=NULL )
					{
						SendMessage( HWND_BROADCAST, uiGlobal_MsgID_SkypeControlAPIDiscover, (WPARAM)hInit_MainWindowHandle, 0);
					}
				}
			}
		}
	}
}

SkypeApi::~SkypeApi(void)
{
		CloseHandle(hGlobal_ThreadShutdownEvent);
		DeInitialize_DestroyMainWindow();
		DeInitialize_DestroyWindowClass();
}
