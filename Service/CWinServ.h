#ifndef CWinServ_h_defined
#define CWinServ_h_defined

#include <windows.h>

typedef struct CWS_STRUCT_
{
    LPCTSTR lpServiceName;
    LPCTSTR lpDisplayName;
    DWORD dwDesiredAccess;
    DWORD dwServiceType;
    DWORD dwStartType;
    DWORD dwErrorControl;
    LPCTSTR lpBinaryPathName;
    LPCTSTR lpLoadOrderGroup;
    LPDWORD lpdwTagId;
    LPCTSTR lpDependencies;
    LPCTSTR lpServiceStartName;
    LPCTSTR lpPassword;
} CWS_STRUCT, *LPCWS_STRUCT;

/* -- Static class -- */
class CWinServ
{       
    private:
        CWinServ(){}
        static void HandleErr(DWORD dwErrCode);
        static int Install(SC_HANDLE hServCtrl, LPCWS_STRUCT lpStr);
        static int Remove(SC_HANDLE hServCtrl, LPCWS_STRUCT lpStr);
       
    public:
        enum {
            CWS_INSTALL, CWS_REMOVE
        };
        static int Config(int op, LPCWS_STRUCT lpStr, int verbose=0);
        static int Run(LPSERVICE_TABLE_ENTRY lpServTabEntry, int verbose=0);
};

#endif