#define UNICODE
#include "windows.h"
#include "reg.h"
#include "Urlmon.h"
#include "ping.h"
#include "helper.h"


bool ISFileExists(WCHAR *achar )
{
	bool salida;
	HANDLE File = CreateFile(achar, GENERIC_READ | GENERIC_WRITE , FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL);
	if (INVALID_HANDLE_VALUE !=File)
	{
		salida = true;
		CloseHandle(File);
	}
	else
	{
		salida = false;

	}
	return salida;

	
}


DWORD EjecutarProgram(WCHAR *execute, bool insyncro, int showmode )
{
    WCHAR malditostack[512];
	ZeroMemory(malditostack,1024);
	wcscat(malditostack,execute);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow =showmode;
	
	ZeroMemory( &pi, sizeof(pi) );


    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        malditostack,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
//        printf( "CreateProcess failed (%d)\n", GetLastError() );
        return 0;
    }

    // Wait until child process exits.
	DWORD salida=0;
	if (insyncro) 
	{
		WaitForSingleObject( pi.hProcess, INFINITE );
		GetExitCodeProcess(pi.hProcess,&salida);
    // Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	};
        return salida;

}

bool iswinXporMore(void)
{
    OSVERSIONINFO osvi;
    BOOL bIsWindowsXPorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    bIsWindowsXPorLater = 
       ( (osvi.dwMajorVersion > 5) ||
       ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) ));

	return bIsWindowsXPorLater;
}

void GetJustNameNoExtensiontoUpload(WCHAR* infile ,WCHAR* out)
{
	int tamano;
	int fin =0;
	tamano = wcslen(infile);
	int inicio =0;
	for (int i= tamano-1;i>0;i--)
	{
		if (infile[i]==L'\\'&& (inicio==0))
		{
			inicio =i+1;
			break;
		}
		if (infile[i]==L'.'&& (fin==0))
		{
			fin =i;
		}

	}
	wcscat(out,&infile[inicio]);
	out[fin-inicio]=0x00;
}



bool GetUploadData(WCHAR* afilename, WCHAR* uploadedfilename, int tipo)
{
	WCHAR filename[512];
	WCHAR serialcode[512];

	UPLOADOPTIONS aup;
	ZeroMemory(&aup,sizeof(aup));
	ReadRegistryUploadOptions(&aup);
	ZeroMemory(serialcode,sizeof(serialcode));
	ZeroMemory(filename,sizeof(filename));
	GetJustNameNoExtensiontoUpload(afilename,filename);
	wcscat(serialcode,aup.DOMAIN_NAME);
	if (lstrcmpi(serialcode,L"")==0)
	{
		return false;
	}

	if (tipo==0)
	{
		//Just a link
		wsprintf(uploadedfilename,L"http://www.easyvoiprecorder.com/upload/%s-%s.mp3", serialcode,filename); 	
	}
	else
	{
		wsprintf(uploadedfilename,L"<object type='application/x-shockwave-flash' width='320' height='17' \r\n data = 'http://www.easyvoiprecorder.com/player/xspf_player_slim.swf?song_url=%s%s-%s.mp3'> \r\n <param name='movie' value ='http://www.easyvoiprecorder.com/player/xspf_player_slim.swf?song_url=%s%s-%s.mp3'/>\r\n</object>",L"http://www.easyvoiprecorder.com/upload/",serialcode,filename,L"http://www.easyvoiprecorder.com/upload/",serialcode,filename); 	
		// a button	
	}

	return true;
}


void chek4update(HWND ahwnd)
{
	char mybuffer1[16384];
	WCHAR mybuffer1Unicode[8192];

	WCHAR redirect[512];
	ZeroMemory(redirect,sizeof(redirect));
	WCHAR aversion[10];
	WCHAR aversionhttp[10];

	ZeroMemory(aversion,sizeof(aversion));
	ZeroMemory(aversionhttp,sizeof(aversionhttp));
	ZeroMemory(mybuffer1Unicode,sizeof(mybuffer1Unicode));

	
	ReadVersion(aversion);

	WCHAR Destfilename[512];
	ZeroMemory(Destfilename,sizeof(Destfilename));
	URLDownloadToCacheFile(NULL,L"http://www.easyvoiprecorder.com/check4update.php",Destfilename,sizeof(Destfilename),0,NULL);
	HANDLE myfileRead1 = CreateFile(Destfilename, GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	ZeroMemory(mybuffer1,sizeof(mybuffer1));
	DWORD readedBytes1;
	ReadFile(myfileRead1,(LPVOID)mybuffer1,16384,&readedBytes1,NULL);
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,mybuffer1,16384,mybuffer1Unicode,8192);
	CloseHandle(myfileRead1);
	WCHAR * posicion= wcsstr(mybuffer1Unicode,L",");
	posicion[0]=0x00;
	wcscat(redirect,posicion+1);
	wcscat(aversionhttp,mybuffer1Unicode);

	if (lstrcmpi(aversion,aversionhttp)==0)
	{
		MessageBox(ahwnd,L"You have the lastest version available.\nNo update needed.",MRecorder,64);
	}
	else
	{
		WCHAR message[512];
		ZeroMemory(message,sizeof(message));
		wsprintf(message,L"There is a new Version available for download.\n %s Ver:%s\nDo you want to read more about it?",MRecorder,aversionhttp);
		if (6==MessageBox(ahwnd,message,MRecorder,36))
		{
			ShellExecute(ahwnd,L"open",redirect,NULL,NULL,SW_SHOW);
		};
	}
};



bool SendSerialSharing(HWND ahwnd,WCHAR* plan)
{
	char mybuffer1[16384];
	WCHAR mybuffer1Unicode[8192];

	WCHAR redirect[512];
	ZeroMemory(redirect,sizeof(redirect));
	WCHAR aversion[10];
	WCHAR aversionhttp[10];

	ZeroMemory(aversion,sizeof(aversion));
	ZeroMemory(aversionhttp,sizeof(aversionhttp));
	ZeroMemory(mybuffer1Unicode,sizeof(mybuffer1Unicode));

	
	ReadVersion(aversion);

	WCHAR Destfilename[512];
	ZeroMemory(Destfilename,sizeof(Destfilename));
	WCHAR DestfilenameURL[512];
	ZeroMemory(DestfilenameURL,sizeof(DestfilenameURL));
	DWORD serail;
	GetMACaddress(&serail);
	wsprintf(DestfilenameURL,L"http://www.easyvoiprecorder.com/registration_plan.php?sn=%lx&sharingplan=%s",serail,plan);
	//MessageBox(0,DestfilenameURL,L"",0);
		
	URLDownloadToCacheFile(NULL,DestfilenameURL,Destfilename,sizeof(Destfilename),0,NULL);
	HANDLE myfileRead1 = CreateFile(Destfilename, GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	ZeroMemory(mybuffer1,sizeof(mybuffer1));
	DWORD readedBytes1;
	ReadFile(myfileRead1,(LPVOID)mybuffer1,16384,&readedBytes1,NULL);
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,mybuffer1,16384,mybuffer1Unicode,8192);
	CloseHandle(myfileRead1);
	
	WCHAR * posicion= wcsstr(mybuffer1Unicode,L",");
	posicion[0]=0x00;
	wcscat(aversionhttp,mybuffer1Unicode);

	if (lstrcmpi(L"0",aversionhttp)==0)
	{
		MessageBox(ahwnd,L"Registration success.",MRecorder,64);
		return true;
	}
	else
	{
		WCHAR message[512];
		ZeroMemory(message,sizeof(message));
		wsprintf(message,L"There is an error with the registration process\nPlease contact to our customer service",aversionhttp);
		MessageBox(ahwnd,message,MRecorder,48);
		return false;
	}
};


bool SendSerialTool(HWND ahwnd,WCHAR* plan, WCHAR* email)
{
	char mybuffer1[16384];
	WCHAR mybuffer1Unicode[8192];

	WCHAR redirect[512];
	ZeroMemory(redirect,sizeof(redirect));
	WCHAR aversion[10];
	WCHAR aversionhttp[10];

	ZeroMemory(aversion,sizeof(aversion));
	ZeroMemory(aversionhttp,sizeof(aversionhttp));
	ZeroMemory(mybuffer1Unicode,sizeof(mybuffer1Unicode));

	
	ReadVersion(aversion);

	WCHAR Destfilename[512];
	ZeroMemory(Destfilename,sizeof(Destfilename));
	WCHAR DestfilenameURL[512];
	ZeroMemory(DestfilenameURL,sizeof(DestfilenameURL));
	DWORD serail=0;
	GetMACaddress(&serail);
	wsprintf(DestfilenameURL,L"http://www.easyvoiprecorder.com/registration_tool.php?sn=%lx&sharingplan=%s&email=%s",serail,plan,email);
	//MessageBox(0,DestfilenameURL,L"",0);
		
	URLDownloadToCacheFile(NULL,DestfilenameURL,Destfilename,sizeof(Destfilename),0,NULL);
	HANDLE myfileRead1 = CreateFile(Destfilename, GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	ZeroMemory(mybuffer1,sizeof(mybuffer1));
	DWORD readedBytes1;
	ReadFile(myfileRead1,(LPVOID)mybuffer1,16384,&readedBytes1,NULL);
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,mybuffer1,16384,mybuffer1Unicode,8192);
	CloseHandle(myfileRead1);
	
	WCHAR * posicion= wcsstr(mybuffer1Unicode,L",");
	posicion[0]=0x00;
	wcscat(aversionhttp,mybuffer1Unicode);
	posicion[0]=0x00;

	WCHAR acode[64];
	ZeroMemory(acode,sizeof(acode));
	wcscat(acode,posicion+1);
	unsigned char * ser;

	if (lstrcmpi(L"0",aversionhttp)==0)
	{
		unsigned char data[6];
		ZeroMemory(&data[0],6);
		memset(&data[0],0x90,5);
		ser = (unsigned char*)&serail;
		for (int i=0;i<5;i++)
		{
			data[i] = data[i] ^ ser[i];
		}
		
		WriteRegistrationCode(data);


		MessageBox(ahwnd,L"Registration success.",MRecorder,64);
		//escribir el codigo de registracion.
		return true;
	}
	else
	{
		WCHAR message[512];
		ZeroMemory(message,sizeof(message));
		wsprintf(message,L"There is an error with the registration process\nPlease contact to our customer service",aversionhttp);
		MessageBox(ahwnd,message,MRecorder,48);
		return false;
	}
};