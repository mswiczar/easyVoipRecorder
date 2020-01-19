#include "ping.h"

#include "reg.h"
#define UNICODE
#pragma comment ( lib, "urlmon.lib") 


void GetMACaddress(DWORD* serial_number )
{

	WCHAR namebuffer[50];
	WCHAR filenamebuffer[50];
	DWORD maximun;
	DWORD fileflags;
	WCHAR filesss[100];
	GetWindowsDirectoryW(&filesss[0],100);
	filesss[3] = 0;
	GetVolumeInformationW(&filesss[0],&namebuffer[0],50,serial_number,&maximun,&fileflags,&filenamebuffer[0],50);
};


void DoPing(void)
{
	DWORD mac;
	WCHAR version[64];
	WCHAR salida[128];
	ZeroMemory(salida,sizeof(salida));
	ZeroMemory(version,sizeof(version));

	if (Pinged())
	{
		GetMACaddress(&mac);  
		ReadVersion(version);
		WCHAR filename[512];
		ZeroMemory(filename,sizeof(filename));
		wsprintfW(salida,L"http://www.easyvoiprecorder.com/eaysvoip_install.php?sn=%lx&version=%s",mac,version);
		URLDownloadToCacheFileW(NULL,salida,filename,sizeof(filename),0,NULL);
	};
		//ShellExecute(0,"open",salida,NULL,NULL,SW_SHOW);
	return;
}

