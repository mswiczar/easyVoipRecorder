#include <stdio.h>
#include <stdlib.h>
#include "CCmdLineArgs.h"
#include "CWinServ.h"
#include "Monitor.h"

#define FULL_SERV_NAME "MRecorder"

SERVICE_STATUS serv_stat;
SERVICE_STATUS_HANDLE ss_handle;

VOID WINAPI serv_main(DWORD dwArgc, LPTSTR* lpszArgv);
VOID ctrl_handler(DWORD dwReq);

CMonitor * amonitor;


void main(int argc, char *argv[])
{      
	/*
	CMonitor * amonitor;
	amonitor = new CMonitor(NULL);
	for (int i=0;i <10000;i++)
	{
		amonitor->CheckApp();
		Sleep(1000);
	};
	delete amonitor;
	Sleep(10000);

	amonitor = new CMonitor(NULL);
	for (int i=0;i <10;i++)
	{
		amonitor->CheckApp();
		Sleep(1000);
	};
	delete amonitor;
	return;
*/

	int inst = 0, rem = 0, run = 0;
    /* -- Long option table. -- */
    lopt_entry lopts[] =
      { { "install",   CCmdLineArgs::CLA_NOARG, &inst, 'i' },
        { "uninstall", CCmdLineArgs::CLA_NOARG, &rem,  'u' },
        { "run",       CCmdLineArgs::CLA_NOARG, &run,  'r' },
        {  NULL,       0,                       NULL,  0  }  };
   
    char ret;
    while ((ret = CCmdLineArgs::Parse(argc, argv, "iur", lopts)) != CCmdLineArgs::CLA_DONE) {
        switch (ret) {
            case 'i':
                inst = 1;
                break;
            case 'u':
                rem = 1;
                break;
            case 'r':
                run = 1;
                break;                
            case 0:
                break;
            case CCmdLineArgs::CLA_MISSING:
                printf("argument missing for option %s.\n", CCmdLineArgs::optopt);
                exit(1);
            case CCmdLineArgs::CLA_INVALID:
                printf("option %s invalid.\n", CCmdLineArgs::optopt);
                exit(1);
            default:
                printf("unknown return value: %c\n", ret);
                exit(1);
        }
    }
    if (inst && rem) {
        printf("installation and removal of service requested, cannot perform both.\n");
        exit(1);
    }
    if (rem && run) {
        printf("execution and removal of service requested, cannot perform both.\n");
        exit(1);
    }
    
    CWS_STRUCT serv_str;
    serv_str.lpServiceName = FULL_SERV_NAME;
    serv_str.lpDisplayName = FULL_SERV_NAME;
    serv_str.dwDesiredAccess = GENERIC_READ | GENERIC_EXECUTE;
    serv_str.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serv_str.dwStartType = SERVICE_AUTO_START;
    serv_str.dwErrorControl = SERVICE_ERROR_IGNORE;
    serv_str.lpBinaryPathName = argv[0];
    serv_str.lpLoadOrderGroup = NULL;
    serv_str.lpdwTagId = NULL;
    serv_str.lpDependencies = NULL;
    serv_str.lpServiceStartName = NULL;
    serv_str.lpPassword = NULL;
    
    if (argc == 1) {
        run = 1;
    }
    if (rem) {
        /* -- Remove service. -- */
        printf("uninstall: requested uninstallation of service.\n");
        if (!CWinServ::Config(CWinServ::CWS_REMOVE, &serv_str, 1)) {
            printf("uninstall: uninstallation successful.\n");
        }
    }
    else {
        if (inst) {
            /* -- Install service. -- */
            printf("install: requested installation of service.\n");
            if (!CWinServ::Config(CWinServ::CWS_INSTALL, &serv_str, 1)) {
                printf("install: installation successful.\n");
            }
        }
        if (run) {
            /* -- Run service. -- */
            printf("run: requested execution of service.\n");
            SERVICE_TABLE_ENTRY ServTabEntry[] =
              { { FULL_SERV_NAME, serv_main }, 
                {           NULL,      NULL } };
            if (!CWinServ::Run(ServTabEntry,1)) {
                printf("run: successful.\n");
            }
            printf("run: execution terminated.\n");
            /* -- Return service exit code. -- */
            exit(serv_stat.dwWin32ExitCode);
        }
    }
    exit(0);
}

VOID WINAPI serv_main(DWORD dwArgc, LPTSTR* lpszArgv)
{
	serv_stat.dwServiceType = SERVICE_WIN32;
	serv_stat.dwCurrentState = SERVICE_START_PENDING;
	serv_stat.dwControlsAccepted = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serv_stat.dwWin32ExitCode = NO_ERROR;
	serv_stat.dwServiceSpecificExitCode = 0;
	serv_stat.dwCheckPoint = 0;
	serv_stat.dwWaitHint = 0;
	
    SetServiceStatus(ss_handle, &serv_stat);
	ss_handle = RegisterServiceCtrlHandler(FULL_SERV_NAME, (LPHANDLER_FUNCTION)(ctrl_handler));
	
	/* -- Preset service exit code to 1. -- */
	serv_stat.dwServiceSpecificExitCode = 1;
	HDESK hDeskUser;
	HWINSTA hWinStaUser;
	if (ss_handle) {	
        /* -- Open user's window station "WinSta0". -- */
		hWinStaUser = OpenWindowStation("WinSta0", FALSE, GENERIC_ALL);       
        /* -- Set process window station to "WinSta0", this enables the process to 
              access objects in the window station such as desktops. -- */
        if (SetProcessWindowStation(hWinStaUser)) 
		{
            /* -- Open user's desktop "Default". -- */
            hDeskUser = OpenDesktop("Default", 0, FALSE, GENERIC_ALL);
            /* -- Set thread desktop to "Default". -- */            
            if (SetThreadDesktop(hDeskUser)) 
			{
                /* -- Now install hook procedure to monitor events associated 
                      with threads in the user's desktop. -- */
                    serv_stat.dwCurrentState = SERVICE_RUNNING;
    		        SetServiceStatus(ss_handle, &serv_stat);
					amonitor = new CMonitor(hDeskUser);
					while (serv_stat.dwCurrentState != SERVICE_STOPPED) 
					{
		                // -- Run -- 
		                if (serv_stat.dwCurrentState == SERVICE_RUNNING) 
						{
							amonitor->CheckApp();
							//MessageBeep(0);
							Sleep(5000);
                        }
                        else 
						{
							__asm nop
						}
		            }
		            // -- Service should terminate, uninstall hook procedure. 
    		        // -- No error. --

					serv_stat.dwWin32ExitCode = 0;
            }
            if (hDeskUser) {
                CloseDesktop(hDeskUser); 
            }
        }
        if (hWinStaUser) {
            CloseWindowStation(hWinStaUser);
        }
	}
	
	serv_stat.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(ss_handle, &serv_stat);
}

VOID ctrl_handler(DWORD dwReq)
{
	switch (dwReq) {
	    case SERVICE_CONTROL_PAUSE:
	        /* -- Set service status. -- */
			serv_stat.dwCurrentState = SERVICE_PAUSED;
			serv_stat.dwWin32ExitCode = NO_ERROR;
			SetServiceStatus(ss_handle, &serv_stat);			
	        break;
        case SERVICE_CONTROL_CONTINUE:
            /* -- Set service status. -- */
			serv_stat.dwCurrentState = SERVICE_RUNNING;
			serv_stat.dwWin32ExitCode = NO_ERROR;
			SetServiceStatus(ss_handle, &serv_stat);
            break;	        
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:				
		    /* -- Set service status. -- */
			//__asm int 3
			delete amonitor;

			serv_stat.dwCurrentState = SERVICE_STOPPED;
			serv_stat.dwWin32ExitCode = NO_ERROR;
			SetServiceStatus(ss_handle, &serv_stat);				
			break;
	}
}