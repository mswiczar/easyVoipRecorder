#define   MRecorder L"easyVoipRecorder"
#define   MRecorderDBMutex L"easyVoipRecorderDBMutex"
#define   MRecorderMapedFile L"easyVoipRecorderMapedFile"
#define   MRecorderService L"easyVoipRecorderService"
#define   MRecorderMonitor L"easyVoipRecorderService"





DWORD EjecutarProgram(WCHAR *execute, bool insyncro, int showmode );

bool iswinXporMore(void);

bool GetUploadData(WCHAR* afilename, WCHAR* uploadedfilename, int tipo);

void chek4update(HWND ahwnd);

bool ISFileExists(WCHAR *achar );

bool SendSerialSharing(HWND ahwnd,WCHAR* plan);
bool SendSerialTool(HWND ahwnd,WCHAR* plan, WCHAR* email);

