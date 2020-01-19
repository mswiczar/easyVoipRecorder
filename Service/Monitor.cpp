#include ".\monitor.h"
#include "../Interface/reg.h"
#include "../Interface/helper.h"

#define DLLNAME L"easyVoipRecorder.dll"

#define FUNCTIONNAME "Begin"
#define FUNCTIONNAMEEnd "End"
#define FUNCTIONNAMEEvent "SetEventHandle"
#define EVENTNAME L"easyVoipRecorderEvent"


// Hacer un thread que verifique si esta corriendo aun o no el target.
// y que depure la lista


#define CHECK 9999
#define MSG_CHECK WM_USER+ CHECK

LIST_PROCESS alistprocessArray[512];

HMODULE MyhookModule;

typedef void   (__stdcall *BEGINCORE)(DWORD);
typedef void  (__stdcall *ENDCORE)(void);

BEGINCORE BeginCore;
ENDCORE   EndCore;


DWORD actual_pid =-1;
DWORD actual_id = -1;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool InjectarCool(DWORD  hprocessID, DWORD thread_id);


WCHAR aini[512];

void GetInipath(void)
{
 ZeroMemory(aini,sizeof(aini));
 ReadRegistryPath(aini);
 wcscat(aini,L"./" MRecorder L".ini");
}



BOOL CALLBACK functionEnum(      
    HWND hwnd,
    LPARAM lParam
)
{
	WCHAR ClassName[512];
	WCHAR TextName[512];
	DWORD atid;
	DWORD pPid;
	PLIST_PROCESS alistprocess = (PLIST_PROCESS) lParam;
	atid = GetWindowThreadProcessId(hwnd,&pPid);
		
	if (pPid== alistprocess->Pid)
	{
		GetClassNameW(hwnd,ClassName,512);
		GetWindowTextW(hwnd,TextName,512);
		if(0 ==lstrcmpiW(ClassName,alistprocess->Class))
		{
			alistprocess->aHwnd = hwnd;
			alistprocess->tid = atid;
			//aca vemos si es el HWND que corresponde
			// si es entonces cortar
			return false;
		}
		else
		{
			/*
			if(0 ==lstrcmpiW(ClassName,L"TZapMainForm.UnicodeClass"))
			{
				alistprocess->aHwnd = hwnd;
				alistprocess->tid = atid;
				return false;

			}
			*/
		}
	}

	return true;
}





DWORD WINAPI WindowThread(LPVOID lpParameter )
{


	CMonitor *amonitor;
	amonitor = (CMonitor *) lpParameter;
	if (amonitor->myhDeskUser != NULL)
	{
		SetThreadDesktop(amonitor->myhDeskUser);
	};

	amonitor->wcex.cbSize = sizeof(WNDCLASSEX); 
	amonitor->wcex.style			= CS_HREDRAW | CS_VREDRAW;
	amonitor->wcex.lpfnWndProc	= (WNDPROC)WndProc;
	amonitor->wcex.cbClsExtra		= 0;
	amonitor->wcex.cbWndExtra		= 0;
	amonitor->wcex.hInstance		= NULL;
	amonitor->wcex.hIcon			= NULL;
	amonitor->wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	amonitor->wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	amonitor->wcex.lpszMenuName	= NULL;
	amonitor->wcex.lpszClassName	= MRecorderService;
	amonitor->wcex.hIconSm		= NULL;
	
	RegisterClassExW(&amonitor->wcex);

  	amonitor->ahwndWindowSelf = CreateWindowW(MRecorderService, MRecorderService, WS_OVERLAPPEDWINDOW &~WS_MAXIMIZEBOX &~WS_THICKFRAME ,
	::GetSystemMetrics(SM_CXSCREEN)-300-120-50, GetSystemMetrics(SM_CYSCREEN)-300-100, 20, 20, NULL, NULL, NULL, NULL);

	//ShowWindow(amonitor->ahwndWindowSelf,SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, amonitor->ahwndWindowSelf, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
return 0;
}


void CMonitor::LoadHook(void)
{
	if (MyhookModule == NULL)
	{
		MyhookModule =   LoadLibraryW(DLLNAME);
		BeginCore    =  (BEGINCORE)GetProcAddress(MyhookModule,FUNCTIONNAME);
		EndCore      =	(ENDCORE)GetProcAddress(MyhookModule,FUNCTIONNAMEEnd);
	};
}

bool CMonitor::SearchIfInList(PLIST_PROCESS  alistprocess)
{
	for (int i=0;i< 512;i++)
	{
		if(0 ==lstrcmpiW(alistprocessArray[i].ProcessName,alistprocess->ProcessName))
		{
			if (alistprocessArray[i].Pid == alistprocess->Pid)
			{
				return true;
			}
			else
			{
				ZeroMemory(&alistprocessArray[i],sizeof(LIST_PROCESS));
				return false;							
			}
		}
	}
	return false;
}


bool CMonitor::InsertInList(PLIST_PROCESS  alistprocess)
{
	int salida;
	for (int i=0;i< 512;i++)
	{
		if (alistprocessArray[i].Pid ==0)
		{
				InjectarCool(alistprocess->Pid,alistprocess->tid);
				Sleep(2010);
				PostThreadMessage(alistprocess->tid,WM_COMMAND, MSG_BEGIN,(LPARAM) i);
				if (alistprocess->DS==1)
				{
					Sleep(200);
					PostThreadMessage(alistprocess->tid,WM_COMMAND, MSG_DS,1);
				}
				else
				{
					Sleep(200);
					PostThreadMessage(alistprocess->tid,WM_COMMAND, MSG_DS,0);
				}

			//	SendMessage(this->ahwndWindowSelf,MSG_FIX,alistprocess->tid,i);
				memcpy(&alistprocessArray[i],alistprocess,sizeof(LIST_PROCESS));
				return  true;

			//EndCore();
			break;
		}
	}
return  false;
}


void CMonitor::RemoveFromList(PLIST_PROCESS  alistprocess)
{
	for (int i=0;i< 512;i++)
	{
		if (alistprocessArray[i].Pid ==alistprocess->Pid)
		{
				PostThreadMessage(alistprocess->tid,WM_COMMAND, MSG_END,(LPARAM) alistprocess->aHwnd);
				Sleep(500);
				ZeroMemory(&alistprocessArray[i],sizeof(alistprocessArray[i]));
				break;
		}
	}
}

void CMonitor::RemoveAll(void)
{
	for (int i=0;i< 512;i++)
	{
		if (alistprocessArray[i].Pid !=0)
		{
			if (alistprocessArray[i].reconding)
			{
				PostThreadMessage(alistprocessArray[i].tid,WM_COMMAND, MSG_REC_STOP,0);
			}
			PostThreadMessage(alistprocessArray[i].tid,WM_COMMAND, MSG_END,(LPARAM) alistprocessArray[i].aHwnd);
			Sleep(500);
			ZeroMemory(&alistprocessArray[i],sizeof(alistprocessArray[i]));
		}
	}
	//EndCore();
}




void CMonitor::GetHwndfromPid(PLIST_PROCESS  alistprocess)
{
//	WCHAR windowClass[512];
//	WCHAR windowCaption[512];

	DWORD pPid;
//	ZeroMemory(windowClass,sizeof(windowClass));
//	ZeroMemory(windowCaption,sizeof(windowCaption));
	

	if (0 != GetPrivateProfileStringW(
							 alistprocess->ProcessName,
							 L"HWND_NAME",
							 NULL,
							 alistprocess->Class,
							 512,
							 aini
							 ))
	{
		alistprocess->DS =	GetPrivateProfileIntW(
							 alistprocess->ProcessName,
							 L"ds",
							 0,
							 aini
							 );

		if (0 == GetPrivateProfileStringW(
								alistprocess->ProcessName,
								L"caption",
								NULL,
								alistprocess->Caption,
								512,
								aini
								))
		{
			// esta en blanco
			alistprocess->aHwnd = FindWindowW(alistprocess->Class,NULL);
			if (alistprocess->aHwnd==NULL)
			{

				if(0 ==lstrcmpiW(alistprocess->Class,L"tSkMainForm.UnicodeClass"))
				{
					alistprocess->aHwnd = FindWindowW(L"TZapMainForm.UnicodeClass",NULL);
					if (alistprocess->aHwnd==NULL)
					{
							alistprocess->aHwnd = FindWindowW(L"tSkMainForm",NULL);
					}
				}
			}
			
			alistprocess->tid = GetWindowThreadProcessId(alistprocess->aHwnd,&pPid);
			if (pPid !=  alistprocess->Pid)
			{
				alistprocess->tid=0;
				alistprocess->aHwnd=0;
			}
		}
		else
		{
			alistprocess->tid=0;
			alistprocess->aHwnd=0;

			//tiene dato
			// aca cambiar el metodo
			// buscar todas las ventanas
			// fijarse cual es la de la clase que se necesita
			// fijarse si hay * en el nombre 
			EnumWindows(functionEnum,(LPARAM)alistprocess);
		}
	}
		
}




bool CMonitor::searchinProcessIni(WCHAR* aproc)
{
	bool salida = false;
	WCHAR* comparar;
	size_t nexpos = 0;
	comparar = this->lpprocess;	
	while (true)
	{
		comparar = &comparar[nexpos];	
		if (comparar[0]==0x00) 
		{
			break;
		};
		if(0 ==lstrcmpiW(aproc,comparar))
		{
			salida = true;
			break;
		}
		nexpos= wcslen(&comparar[0])+1;
	};
	return salida;
}





void CMonitor::UnLoadHook(void)
{
	BeginCore   = NULL;
	EndCore = NULL;
	FreeLibrary(MyhookModule);
}

/*

	return TRUE;
*/







bool CMonitor::PrintProcessNameAndID(PLIST_PROCESS  alistprocess)
{
	
	WCHAR salida[1024];
	ZeroMemory(salida,sizeof(salida));
	bool asalida = false; 
    
	// Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, alistprocess->Pid );

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseNameW( hProcess, hMod, alistprocess->ProcessName, 
                               sizeof(alistprocess->ProcessName)/2 );
			asalida=true;
		}

    }

	ZeroMemory(salida,sizeof(salida));
	//wsprintf(salida,L"Process Name =%s  PID: %u", szProcessName, processID );
    CloseHandle( hProcess );
	return asalida ;
}




CMonitor::CMonitor(HDESK aparam)
{
	
	myhDeskUser = aparam;
	ZeroMemory(&alistprocessArray[0],sizeof(alistprocessArray));
	GetInipath();
	GetPrivateProfileSectionW(
	L"process",
	lpprocess,
	size_wchar_heap,
	//L".\\EasyVoiprecorder.ini"
	aini);
	LoadHook();

	MytrheadWin = CreateThread(NULL,0,WindowThread,(LPVOID)this,CREATE_SUSPENDED,NULL);
	ResumeThread(MytrheadWin);

}

CMonitor::~CMonitor(void)
{
	RemoveAll();
	UnLoadHook();
	ZeroMemory(&alistprocessArray[0],sizeof(alistprocessArray));
	DestroyWindow(ahwndWindowSelf);
	TerminateThread(MytrheadWin,0);

}

void CMonitor::CheckApp(void)
{

	LIST_PROCESS alistprocess;
	ZeroMemory(&alistprocess,sizeof(alistprocess));
	DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
			return;

	cProcesses = cbNeeded / sizeof(DWORD);
	for ( i = 0; i < cProcesses; i++ )
	{

		if( aProcesses[i] != 0 )
		{
			alistprocess.Pid = aProcesses[i];
			if (PrintProcessNameAndID(&alistprocess))
			{
				if (searchinProcessIni(alistprocess.ProcessName))
				{
					// fijarse si existe en la lista interna de patcheados.
					if (SearchIfInList(&alistprocess))
					{
						

					}
					else
					{
						GetHwndfromPid(&alistprocess);
						if (alistprocess.tid!=0)
						{
							InsertInList(&alistprocess);
							HWND awin = FindWindow(EasyVoipRecorder,EasyVoipRecorder);
							SendMessage(awin,MSG_NEWAPP,alistprocess.Pid,0);
						};
					}
				}
			}
		}
	}

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HWND awin;
	PAINTSTRUCT ps;
	HDC hdc;
	POINT apoint;
	PCOPYDATASTRUCT icopdat;
	switch (message) 
	{
		case MSG_REC_START:
			awin = FindWindow(EasyVoipRecorder,EasyVoipRecorder);
			if (actual_pid !=-1)
			{
				if (alistprocessArray[wParam].reconding==false)
				{
					PostThreadMessage(alistprocessArray[wParam].tid,WM_COMMAND, MSG_REC_START,(LPARAM)awin);
					alistprocessArray[wParam].reconding=true;
				};
			}
			else
			{
				return 1;
			}

			break;

		case MSG_REC_STOP:
			if (alistprocessArray[wParam].reconding)
			{
				PostThreadMessage(alistprocessArray[wParam].tid,WM_COMMAND, MSG_REC_STOP,0);
				alistprocessArray[wParam].reconding=false;
			};
			break;

		case MSG_REC_PAUSE:
				PostThreadMessage(alistprocessArray[wParam].tid,WM_COMMAND, MSG_REC_PAUSE,0);
			break;

		case MSG_REC_CONTINUE:
				PostThreadMessage(alistprocessArray[wParam].tid,WM_COMMAND, MSG_REC_CONTINUE,0);
			break;

		case MSG_MICBEGIN:
			actual_pid = alistprocessArray[wParam].Pid;
			actual_id = wParam;
			awin = FindWindow(EasyVoipRecorder,EasyVoipRecorder);
			if (awin == NULL)
			{
				if (ReadRegistryAutorecord())
				{
					PostThreadMessage(alistprocessArray[wParam].tid,WM_COMMAND, MSG_REC_START,(LPARAM)awin);
					alistprocessArray[wParam].reconding=true;
				}
			}
			else
			{
				SendMessage(awin,MSG_MICBEGIN,alistprocessArray[wParam].Pid,wParam);
			}
			break;

		case MSG_MICEND:
			awin = FindWindow(EasyVoipRecorder,EasyVoipRecorder);
			if (awin == NULL)
			{
				if (ReadRegistryAutorecord())
				{
					PostThreadMessage(alistprocessArray[wParam].tid,WM_COMMAND, MSG_REC_STOP,0);
					alistprocessArray[wParam].reconding=false;
				}
			}
			else
			{
				actual_pid = -1;
				actual_id = -1;
				SendMessage(awin,MSG_MICEND,alistprocessArray[wParam].Pid,0);
			}
			break;

		case MSG_GETSOURCE:
			if (actual_pid != -1)
			{
				awin = FindWindow(EasyVoipRecorder,EasyVoipRecorder);
				SendMessage(awin,MSG_MICBEGIN,actual_pid,actual_id);
			}
			break;
/*
		case MSG_FIX:
				BeginCore(wParam);
				Sleep(200);
				PostThreadMessage(wParam,WM_COMMAND, MSG_BEGIN,(LPARAM) lParam);
				Sleep(200);
			break;

		case WM_COPYDATA:
				icopdat = (PCOPYDATASTRUCT)lParam;			
				MessageBoxW(0,(LPCWSTR)icopdat->lpData,(LPCWSTR)icopdat->lpData,0);
				break;
*/

		default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


/*
void ReadRegistryPath(WCHAR* apath)
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_LOCAL_MACHINE,L"SoftWare\\EasyVoipRecorder",0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"InstallDir", NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		salida=true;
		wcscat(apath,ou);
	};

	RegCloseKey( hKey );
    return ;
}
*/


typedef struct _data
{
	WCHAR dllname[512];
	WCHAR EventName[512];
	char FunctionName[512];
	char FunctionNameEnd[512];
	char FunctionNameEvent[512];
	LPVOID _LoadLibrary;
	LPVOID _FreeLibrary;
	LPVOID _GetProcAddress;
	LPVOID _sleep;
	LPVOID _CreateEvent;
	LPVOID _WaitForSingleObject;
	LPVOID _CloseHandle;
	DWORD _thread_id;
	unsigned char codigo[1024];

}
DATA,*PDATA;



DWORD WINAPI ThreadInjectar(
  LPVOID lpParameter
)
{
	PDATA parametro = (PDATA)lpParameter;
	LPVOID _LoadLibrary;
	LPVOID _FreeLibrary;
	LPVOID _GetProcAddress;
	LPVOID _WaitForSingleObject;
	LPVOID _CreateEvent;

	WCHAR *_filename;
	WCHAR *_eventName;
	char * _functionName;
	char * _functionNameEnd;
	char * _functionNameEvent;

	DWORD salida;
	HMODULE moduleeasy;
	LPVOID  _EasyVoipRecorderStart;
	LPVOID  _EasyVoipRecorderEnd;
	LPVOID  _EasyVoipRecorderSetEvent;

	LPVOID _sleep;
	LPVOID _CloseHandle;
	DWORD _threadid;
	HANDLE eventhandle;

	_LoadLibrary = parametro->_LoadLibrary;
	_GetProcAddress = parametro->_GetProcAddress;
	_filename =  parametro->dllname;
	_eventName = parametro->EventName;
	_functionName = parametro->FunctionName;
	_functionNameEnd = parametro->FunctionNameEnd;
	_functionNameEvent = parametro->FunctionNameEvent;
	_threadid = parametro->_thread_id;
	_sleep = parametro->_sleep;
	_CreateEvent = parametro->_CreateEvent;
	_WaitForSingleObject = parametro->_WaitForSingleObject;
	_FreeLibrary = parametro->_FreeLibrary;
	_CloseHandle = parametro->_CloseHandle;
	__asm
	{
		//int 3
		//load library "easy"
		push _filename
		call _LoadLibrary
		mov moduleeasy, eax 

		// getprocadddress funciton begin
		push _functionName	
		push moduleeasy;
		call _GetProcAddress
		mov _EasyVoipRecorderStart,eax

		// getprocadddress function end
		push _functionNameEnd	
		push moduleeasy;
		call _GetProcAddress
		mov _EasyVoipRecorderEnd,eax

		// getprocadddress function SetEventHandle
		push _functionNameEvent	
		push moduleeasy;
		call _GetProcAddress
		mov _EasyVoipRecorderSetEvent,eax

		
		// llamo a la funcion start
		push _threadid
		call _EasyVoipRecorderStart
		mov salida,eax
		
		// busco el evento
		//push _eventName
		push NULL
		push 0
		push 0
		push 0
		call _CreateEvent 		
		mov eventhandle , eax

		// llamo a set event
		push eventhandle
		call _EasyVoipRecorderSetEvent

		// espero el evento
		push INFINITE
		push eventhandle
		call _WaitForSingleObject

		//close handle
		push eventhandle
		call _CloseHandle
		
		//espero 2 segs
		push 500
		call _sleep
		
		// llamo al end
		call _EasyVoipRecorderEnd
	
		// freelibrary
		push moduleeasy	
		call _FreeLibrary
	}
	return salida;
}

void longitud(void)
{
	MessageBeep(0);
}



bool InjectarCool(DWORD  hprocessID, DWORD thread_id)
{
DWORD lpNum;
  DWORD hThreadID;
  LPVOID pLibRemote;
  HANDLE hThread;
  DATA mydata;    
  PDATA cast;
  unsigned long tamanos = (unsigned long)longitud - (unsigned long)ThreadInjectar;


  HANDLE hProcess   = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
                            false, hprocessID);
  ZeroMemory(&mydata,sizeof(DATA));
  //strings

  memcpy(&mydata.dllname[0], DLLNAME,wcslen(DLLNAME)*2);
  memcpy(&mydata.FunctionName[0], FUNCTIONNAME,sizeof(FUNCTIONNAME));
  memcpy(&mydata.FunctionNameEnd[0], FUNCTIONNAMEEnd,sizeof(FUNCTIONNAMEEnd));
  memcpy(&mydata.FunctionNameEvent[0], FUNCTIONNAMEEvent,sizeof(FUNCTIONNAMEEvent));

  memcpy(&mydata.EventName[0], EVENTNAME,wcslen(EVENTNAME)*2);

  

  //PROCEDURES
  HMODULE mymodule;
  mymodule= LoadLibraryW(L"Kernel32.dll");
  mydata._LoadLibrary = (LPVOID)GetProcAddress(mymodule,"LoadLibraryW");
  mydata._GetProcAddress = (LPVOID)GetProcAddress(mymodule,"GetProcAddress");
  mydata._sleep = (LPVOID)GetProcAddress(mymodule,"Sleep");
  mydata._CreateEvent= (LPVOID)GetProcAddress(mymodule,"CreateEventW");
  mydata._WaitForSingleObject = (LPVOID)GetProcAddress(mymodule,"WaitForSingleObject");
  mydata._FreeLibrary = (LPVOID)GetProcAddress(mymodule,"FreeLibrary");
  mydata._CloseHandle = (LPVOID)GetProcAddress(mymodule,"CloseHandle");
  mydata._thread_id= thread_id;
  // ASM CODE
  memcpy(&mydata.codigo[0],ThreadInjectar,tamanos);

  //codigo
  pLibRemote   = VirtualAllocEx(hProcess, NULL, sizeof(DATA), MEM_COMMIT, PAGE_EXECUTE_READWRITE );
  cast = (PDATA) pLibRemote;
	
  if (WriteProcessMemory(hProcess, pLibRemote, &mydata , (SIZE_T)(sizeof(DATA)), (SIZE_T*)&lpNum))
	{
		try
		{
			hThread = CreateRemoteThread(hProcess,NULL, 0,
                                   (LPTHREAD_START_ROUTINE)&cast->codigo[0],
                                   pLibRemote, 0, &hThreadID );
			//WaitForSingleObject(hThread,INFINITE);
			return  true;
		}
		catch (int erro)
		{
			return  false;
		}
	};
  	return  false;
};

