// TheHook.cpp : Defines the entry point for the DLL application.
//#include "AggressiveOptimize.h"
#include <windows.h>
#include "PlayWave.h"
#include "encoder_example.h"


#define MSG_REC_START    WM_USER + 9002
#define MSG_REC_STOP     WM_USER + 9003
#define MSG_REC_PAUSE    WM_USER + 9004
#define MSG_REC_CONTINUE WM_USER + 9005

#define MSG_BEGIN WM_USER + 9010
#define MSG_END   WM_USER + 9011
#define MSG_DS WM_USER + 9100 		


#define MSG_PATCHED WM_USER + 9110



HANDLE eventTerminar;
#define MODULO_VISUAL	  "easyVoipRecorder" 

bool inrec=false;
bool patched=false;


DWORD main_posicion_in_array=-1;

// --------------------------------------------------------------------

HINSTANCE	g_hModule	= 0;
HHOOK hook;


// MAIN ---------------------------------------------------------------


void GetOnlyName(WCHAR* afilenameDest,WCHAR*afilenameSource)
{
	int tamano;
	tamano = wcslen(afilenameSource);
	int inicio =0;
	int fin =0;

	for (int i= tamano-1;i>0;i--)
	{
		if ((afilenameSource[i]==L'.') && (fin==0))
		{
			fin = i;
			afilenameSource[i]=NULL;
		}

		if (afilenameSource[i]==L'\\'&& (inicio==0))
		{
			inicio =i+1;
			break;
		}
	}
	wcscat(afilenameDest,&afilenameSource[inicio]);
}


BOOL WINAPI DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hModule = hModule;
			Initialize();
		break;
		case DLL_PROCESS_DETACH:
			break;
		default:
			break;
	}
	return TRUE;
}



LRESULT CALLBACK GetMsgProc(      
    int code,
    WPARAM wParam,
    LPARAM lParam
)
{
	if (code == HC_ACTION)
	{
		PMSG mes = (PMSG) lParam;

		char salida[512];
		ZeroMemory(salida,sizeof(salida));
	

		if (mes->message==WM_COMMAND)
			{
				switch (mes->wParam)
				{

				case MSG_DS :
					{
						sprintf(salida,"%s","MSG_DS");
						//OutputDebugStringA(salida);
						PatchMemory(main_posicion_in_array);
						patched=true;
					}
					break;


			case MSG_BEGIN :
				{
					sprintf(salida,"%s","BEGIN");
					//OutputDebugStringA(salida);
					inrec=false;

					ZeroMemory(afilename,sizeof(afilename));
					GetModuleFileName(NULL,afilename,1024);

					WCHAR afilename2[512];
					ZeroMemory(afilename2,sizeof(afilename2));
					GetOnlyName(afilename2,afilename);
					SetIsSkype(false);
					SetIsMSN(false);
					SetOlympic(false);
					
					if (lstrcmpi(afilename2,L"SKYPE")==0)
					{
						SetIsSkype(true);
					}
					else
					{
						if (lstrcmpi(afilename2,L"msnmsgr")==0)
						{
							SetIsMSN(true);
						}
						else
						{
							if (lstrcmpi(afilename2,L"olympic")==0)
							{
								SetOlympic(true);
							}
							else
							{
								SetisOther(true);
							}
						}
					}

					main_posicion_in_array = mes->lParam;

				}
				break;


			case MSG_REC_START :
					try
					{	
						if (inrec==false)					
						{
							inrec=true;					
							WCHAR atemp[512];
							ZeroMemory(atemp,1024);
							WCHAR toexecute[512];
							ZeroMemory(toexecute,1024);
							
							GetTempPath(512,atemp);
							SYSTEMTIME atime;
							GetLocalTime(&atime);

							ZeroMemory(afilename,sizeof(afilename));
							GetModuleFileName(NULL,afilename,1024);

							WCHAR afilename2[512];
							ZeroMemory(afilename2,sizeof(afilename2));
							GetOnlyName(afilename2,afilename);
							

							wsprintf(toexecute,L"%s%s-%u-%u-%u--%u_%u_%u",atemp,afilename2, atime.wYear,atime.wMonth,atime.wDay,atime.wHour,atime.wMinute,atime.wSecond);
							SetHWND((HWND)mes->lParam);
							BeginAll(toexecute);
						}
						else
						{
						};
					}
					catch (int)
					{
					}
				break;
			
			case MSG_REC_STOP :
				try
				{
					inrec=false;
					StopAll();						

				}
				catch (int)
				{
				}
				break;

			case MSG_END:
				try
				{
					UnpatchMemory();
					SetEvent(eventTerminar);	
				}
				catch (int)
				{
				}
				
				break;

			case MSG_REC_PAUSE :
				try
				{
					PauseAll();
				}
				catch (int)
				{
				}
				break;

			case MSG_REC_CONTINUE :
				try
				{
					ContinueAll();
				}
				catch (int)
				{
				}
				break;

			case MSG_PATCHED :
				try
				{
					if(	patched)
					{
						return 770;
					}
					return 0;
				}
				catch (int)
				{
				}
				break;


			default :

				break;
			}
		}
	}
	return  CallNextHookEx(hook,code,wParam,lParam);
}


 void __stdcall Begin(DWORD hThreadIdSkype)
{
	try
	{
		hook = SetWindowsHookEx(WH_GETMESSAGE, &GetMsgProc, g_hModule, hThreadIdSkype);
	}
	catch (int)
	{
	}

}

 void __stdcall SetEventHandle(HANDLE aevent)
{
	eventTerminar = aevent;
}


 void __stdcall End(void)
{
	try
	{
		UnhookWindowsHookEx(hook);
	}
	catch (int)
	{
	}
}
