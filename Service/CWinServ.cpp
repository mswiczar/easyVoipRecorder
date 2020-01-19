#include "CWinServ.h"
#include <stdio.h>

/* -- Private -- */
int CWinServ::Install(SC_HANDLE hServCtrl, LPCWS_STRUCT lpServStr)
{
	SC_HANDLE hServ = CreateService(hServCtrl,
                                    lpServStr->lpServiceName,
								    lpServStr->lpDisplayName,
									lpServStr->dwDesiredAccess,
									lpServStr->dwServiceType,
									lpServStr->dwStartType,
									lpServStr->dwErrorControl,
									lpServStr->lpBinaryPathName,
									lpServStr->lpLoadOrderGroup,
									lpServStr->lpdwTagId,
									lpServStr->lpDependencies,
									lpServStr->lpServiceStartName,
									lpServStr->lpPassword);	
	if (hServ == NULL) {
		return GetLastError();
    }
	StartService(hServ,0,NULL);
	CloseServiceHandle(hServ);
	return 0;
}

int CWinServ::Remove(SC_HANDLE hServCtrl, LPCWS_STRUCT lpServStr)
{
	SERVICE_STATUS sta;
	SC_HANDLE hServ = OpenService(hServCtrl, lpServStr->lpServiceName, SC_MANAGER_ALL_ACCESS);
	if (hServ == NULL) {
		return GetLastError();
    }	
	ControlService(hServ,SERVICE_CONTROL_STOP,&sta);
	if (DeleteService(hServ)) {
	    return GetLastError();
	}	
	CloseServiceHandle(hServ);
    return 0;
}

void CWinServ::HandleErr(DWORD dwErrCode)
{
    switch (dwErrCode)
    {
    /* -- OpenSCManager -- */
        case ERROR_ACCESS_DENIED:
            printf("The requested access was denied.\n");
            break;
        case ERROR_DATABASE_DOES_NOT_EXIST:
            printf("The specified database does not exist.\n");
            break;
        case ERROR_INVALID_PARAMETER:
            printf("A specified parameter is invalid.\n");
            break;            
    /* -- OpenService -- */
        //case ERROR_ACCESS_DENIED:
            //printf("The handle does not have access to the service.\n");
            //break;
        case ERROR_INVALID_HANDLE:
            printf("The specified handle is invalid.\n");
            break;
        case ERROR_INVALID_NAME:
            printf("The specified service name is invalid.\n");
            break;
        case ERROR_SERVICE_DOES_NOT_EXIST:
         	printf("The specified service does not exist.\n");
         	break;
    /* -- CreateService -- */
        //case ERROR_ACCESS_DENIED:
            //printf("The handle to the SCM database does not have the SC_MANAGER_CREATE_SERVICE access right.\n");
            //break;
        case ERROR_CIRCULAR_DEPENDENCY:
            printf("A circular service dependency was specified.\n");
            break;
        case ERROR_DUPLICATE_SERVICE_NAME:
            printf("The display name already exists in the service control manager database either as a service name or as another display name.\n");
            break;
        //case ERROR_INVALID_HANDLE:
            //printf("The handle to the specified service control manager database is invalid.\n");
            //break;
        //case ERROR_INVALID_NAME:
            //printf("The specified service name is invalid.\n");
            //break;
        //case ERROR_INVALID_PARAMETER:
            //printf("A parameter that was specified is invalid.\n");
            //break;
        case ERROR_INVALID_SERVICE_ACCOUNT:
            printf("The user account name specified in the lpServiceStartName parameter does not exist.\n");
            break;
        case ERROR_SERVICE_EXISTS:
            printf("The specified service already exists in this database.\n");
            break;
    /* -- DeleteService -- */
        //case ERROR_ACCESS_DENIED:
            //printf("The handle does not have the DELETE access right.\n");
            //break;
        //case ERROR_INVALID_HANDLE:
            //printf("The specified handle is invalid.\n");
            //break;
        case ERROR_SERVICE_MARKED_FOR_DELETE:
            printf("The specified service has already been marked for deletion.\n");
            break;        
    /* -- StartServiceCtrlDispatcher -- */
        case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
            printf("Program is being run as a console application rather than as a service.\n");
            break;
        case ERROR_INVALID_DATA:
            printf("The specified dispatch table contains entries that are not in the proper format.\n");
            break;
        case ERROR_SERVICE_ALREADY_RUNNING:
            printf("The process has already called StartServiceCtrlDispatcher.  Each process can call StartServiceCtrlDispatcher only one time.\n");
            break;            
    /* -- StartService -- */
        //case ERROR_ACCESS_DENIED:
            //printf("The handle does not have the SERVICE_START access right.\n");
            //break;
        //case ERROR_INVALID_HANDLE:
            //printf("The handle is invalid.\n");
            //break;
        case ERROR_PATH_NOT_FOUND:
            printf("The service binary file could not be found.\n");
            break;
        //case ERROR_SERVICE_ALREADY_RUNNING:
            //printf("An instance of the service is already running.\n");
            //break;
        case ERROR_SERVICE_DATABASE_LOCKED:
            printf("The database is locked.\n");
            break;
        case ERROR_SERVICE_DEPENDENCY_DELETED:
            printf("The service depends on a service that does not exist or has been marked for deletion.\n");
            break;
        case ERROR_SERVICE_DEPENDENCY_FAIL:
            printf("The service depends on another service that has failed to start.\n");
            break;
        case ERROR_SERVICE_DISABLED:
            printf("The service has been disabled.\n");
            break;
        case ERROR_SERVICE_LOGON_FAILED:
            printf("The service did not start due to a logon failure. This error occurs if the service is configured to run under an account that does not have the \"Log on as a service\" right.\n");
            break;
        //case ERROR_SERVICE_MARKED_FOR_DELETE:
            //printf("The service has been marked for deletion.\n");
            //break;
        case ERROR_SERVICE_NO_THREAD:
            printf("A thread could not be created for the service.\n");
            break;
        case ERROR_SERVICE_REQUEST_TIMEOUT:
            printf("The process for the service was started, but it did not call StartServiceCtrlDispatcher, or the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function.\n");
            break;
            
        case 0:
            break;
            
        default:
            printf("Not SCM specific error code %d.\n", dwErrCode);
            break;
    }
}

/* -- Public -- */
int CWinServ::Config(int op, LPCWS_STRUCT lpServStr, int verbose)
{
    DWORD ret;
    SC_HANDLE hServCtrl = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
    if (hServCtrl == NULL) {
        ret = GetLastError();
        if (verbose) {
            HandleErr(ret);
        }
        return ret;
    }
    
    switch (op) {
        case CWS_INSTALL:
            ret = Install(hServCtrl, lpServStr);
            break;
        case CWS_REMOVE:
            ret = Remove(hServCtrl, lpServStr);
            break;
    }
  
    CloseServiceHandle(hServCtrl);
    
    if (verbose) {
        HandleErr(ret);
    }
    return ret;
}

int CWinServ::Run(LPSERVICE_TABLE_ENTRY lpServTabEntry, int verbose)
{
 	DWORD ret = StartServiceCtrlDispatcher(lpServTabEntry);
    if (verbose) {
        HandleErr(ret);
    }    
    return ret;
}