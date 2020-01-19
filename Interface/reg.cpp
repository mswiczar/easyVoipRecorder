#include "reg.h"

#include "ping.h"
#include "helper.h"




void ReadRegistryPath(WCHAR* apath)
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"InstallDir", NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		salida=true;
		wcscat(apath,ou);
	};

	RegCloseKey( hKey );
    return ;
}


void ReadRegistryDBPath(WCHAR* apath)
{
		

	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"DBFolder", NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		salida=true;
		wcscat(apath,ou);
	};

	RegCloseKey( hKey );

	return ;
}




void ReadRegistryDestinationPath(WCHAR* apath)
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"DestinationFolder", NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		salida=true;
		wcscat(apath,ou);
	};

	RegCloseKey( hKey );
    return ;
}

void ReadRegistryUtilsPath(WCHAR* apath)
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"utilsFolder", NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		salida=true;
		wcscat(apath,ou);
	};

	RegCloseKey( hKey );
    return ;
}


void ReadRegistryExportPath(WCHAR* apath)
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"ExportFolder", NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		salida=true;
		wcscat(apath,ou);
	};

	RegCloseKey( hKey );
    return ;
}

bool ReadRegistryRunatStart(void)
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\Microsoft\\Windows\\CurrentVersion\\Run",0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, MRecorder, NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		salida=true;
	}
	else
	{
		salida=false;
	}
	RegCloseKey( hKey );
    return salida;
}


void WriteRegistryRunatStart(bool write, HINSTANCE hInst)
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);

	WCHAR ouFull[512];
	ZeroMemory(ouFull,1024);

	GetModuleFileNameW(hInst,ou,1024);
	wsprintfW(ouFull,L"\"%s\" /minimize",ou);


	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",0, KEY_ALL_ACCESS, &hKey );
	dwBufLen = 512;
	if (write)
	{
		RegSetValueExW(hKey,MRecorder,NULL,REG_EXPAND_SZ,(LPBYTE) ouFull, wcslen(ouFull)*2);
	}
	else
	{
		RegDeleteValueW(hKey,MRecorder);
	}
	RegCloseKey( hKey );
    return ;

}

bool ReadRegistryAutorecord(void)
{
	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"AutoRecord", NULL, NULL,(LPBYTE) &ou, &dwBufLen))
	{
	};
	RegCloseKey( hKey );
    if (ou ==0)
	{
		return false;
	}
	else
	{
		return true;
	}

}



void WriteRegistryAutorecord(bool write)
{
	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	if (write)
	{
		al = 1;
		RegSetValueExW(hKey,L"AutoRecord",NULL,REG_DWORD,(LPBYTE)&al , dwBufLen);
	}
	else
	{
		al = 0;
		RegSetValueExW(hKey,L"AutoRecord",NULL,REG_DWORD,(LPBYTE)&al, dwBufLen);
	}
	RegCloseKey( hKey );
    return ;

}

bool ReadRegistryGauge(void)
{
	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"gauge", NULL, NULL,(LPBYTE) &ou, &dwBufLen))
	{
	};
	RegCloseKey( hKey );
    if (ou ==0)
	{
		return false;
	}
	else
	{
		return true;
	}

}

void WriteRegistryGauge(bool write)
{
	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	if (write)
	{
		al = 1;
		RegSetValueExW(hKey,L"gauge",NULL,REG_DWORD,(LPBYTE)&al , dwBufLen);
	}
	else
	{
		al = 0;
		RegSetValueExW(hKey,L"gauge",NULL,REG_DWORD,(LPBYTE)&al, dwBufLen);
	}
	RegCloseKey( hKey );
    return ;

}


void WriteRegistryOpacity(int perc)
{
	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	al = perc;
	RegSetValueExW(hKey,L"opacity",NULL,REG_DWORD,(LPBYTE)&al , dwBufLen);
	RegCloseKey( hKey );
    return ;
}

int ReadRegistryOpacity(void)
{
	DWORD ou;
	int salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(DWORD);
	DWORD test = RegQueryValueExW( hKey, L"opacity", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		salida=  ou;
	}
	else
	{
		salida = 100;
	}
	;
	RegCloseKey( hKey );
    return salida;
}



bool ReadRegistryAutoUpload(void)
{
	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"Autoupload", NULL, NULL,(LPBYTE) &ou, &dwBufLen))
	{
	};
	RegCloseKey( hKey );
    if (ou ==0)
	{
		return false;
	}
	else
	{
		return true;
	}

}



void WriteRegistryAutoUpload(bool write)
{
	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	if (write)
	{
		al = 1;
		RegSetValueExW(hKey,L"Autoupload",NULL,REG_DWORD,(LPBYTE)&al , dwBufLen);
	}
	else
	{
		al = 0;
		RegSetValueExW(hKey,L"Autoupload",NULL,REG_DWORD,(LPBYTE)&al, dwBufLen);
	}
	RegCloseKey( hKey );
    return ;

}

bool ReadRegistryShowPlaylist(void)
{
	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"playlist", NULL, NULL,(LPBYTE) &ou, &dwBufLen))
	{
	};
	RegCloseKey( hKey );
    if (ou ==0)
	{
		return false;
	}
	else
	{
		return true;
	}

}


void WriteRegistryShowPlaylist(bool write)
{
	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	if (write)
	{
		al = 1;
		RegSetValueExW(hKey,L"playlist",NULL,REG_DWORD,(LPBYTE)&al , dwBufLen);
	}
	else
	{
		al = 0;
		RegSetValueExW(hKey,L"playlist",NULL,REG_DWORD,(LPBYTE)&al, dwBufLen);
	}
	RegCloseKey( hKey );
    return ;

}







bool ReadRegistryRecordingOptions(PRECORDINGOPTIONS aoptions)
{
	//BITRATE
	//FORMAT
	//KHZ
	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\" MRecorder L"\\RecordingOptions",0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"BitRate", NULL, NULL,(LPBYTE) &aoptions->BitRate, &dwBufLen))
	{
		goto myerrorReadRegistryRecordingOptions;
	};
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"Format", NULL, NULL,(LPBYTE) &aoptions->Format, &dwBufLen))
	{
		goto myerrorReadRegistryRecordingOptions;
	};
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"KHZ", NULL, NULL,(LPBYTE) &aoptions->KHZ, &dwBufLen))
	{
		goto myerrorReadRegistryRecordingOptions;
	};
	RegCloseKey( hKey );
	return true;


myerrorReadRegistryRecordingOptions:
	return false;
}


bool WriteRegistryRecordingOptions(PRECORDINGOPTIONS aoptions)
{
	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	if (ERROR_SUCCESS!=RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\RecordingOptions",0, KEY_ALL_ACCESS , &hKey ))
	{
		goto myerrorWriteRegistryRecordingOptions;
	}
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"BitRate",NULL,REG_DWORD,(LPBYTE)&aoptions->BitRate, dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorWriteRegistryRecordingOptions;
	}
	
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"Format",NULL,REG_DWORD,(LPBYTE)&aoptions->Format , dwBufLen))
	{
			RegCloseKey( hKey );
			goto myerrorWriteRegistryRecordingOptions;
	}
	
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"KHZ",NULL,REG_DWORD,(LPBYTE)&aoptions->KHZ , dwBufLen))
	{
			RegCloseKey( hKey );
			goto myerrorWriteRegistryRecordingOptions;
	}

	RegCloseKey( hKey );
    return true;

myerrorWriteRegistryRecordingOptions:
	return false;

}

bool ReadRegistryUploadOptions(PUPLOADOPTIONS aoptions)
{
	//DOMAINNAME
	//KEYCODE
	//METHOD
	//PASSWORD
	//USERNAME

	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\UploadOptions",0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"METHOD", NULL, NULL,(LPBYTE) &aoptions->METHOD, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistryUploadOptions;
	};
	dwBufLen=256;
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"DOMAINNAME", NULL, NULL,(LPBYTE) aoptions->DOMAIN_NAME, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistryUploadOptions;
	};
	dwBufLen=256;
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"KEYCODE", NULL, NULL,(LPBYTE) aoptions->KEYCODE, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistryUploadOptions;
	};

	dwBufLen=256;
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"PASSWORD", NULL, NULL,(LPBYTE) aoptions->PASSWORD, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistryUploadOptions;
	};

	dwBufLen=256;
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"USERNAME", NULL, NULL,(LPBYTE) aoptions->USERNAME, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistryUploadOptions;
	};

	RegCloseKey( hKey );
	return true;


myerrorReadRegistryUploadOptions:
	return false;

}

bool WriteRegistryUploadOptions(PUPLOADOPTIONS aoptions)
{
	//DOMAINNAME
	//KEYCODE
	//METHOD
	//PASSWORD
	//USERNAME

	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	if (ERROR_SUCCESS!=RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\UploadOptions",0, KEY_ALL_ACCESS , &hKey ))
	{
		goto myerrorWriteRegistryUploadOptions;
	}

	dwBufLen=wcslen(aoptions->DOMAIN_NAME)*2;
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"DOMAINNAME",NULL,REG_SZ,(LPBYTE)aoptions->DOMAIN_NAME, dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorWriteRegistryUploadOptions;
	}
	
	dwBufLen=wcslen(aoptions->KEYCODE)*2;
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"KEYCODE",NULL,REG_SZ,(LPBYTE)aoptions->KEYCODE , dwBufLen))
	{
			RegCloseKey( hKey );
			goto myerrorWriteRegistryUploadOptions;
	}
	
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"METHOD",NULL,REG_DWORD,(LPBYTE)&aoptions->METHOD , dwBufLen))
	{
			RegCloseKey( hKey );
			goto myerrorWriteRegistryUploadOptions;
	}

	dwBufLen=wcslen(aoptions->PASSWORD)*2;
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"PASSWORD",NULL,REG_SZ,(LPBYTE)&aoptions->PASSWORD, dwBufLen))
	{
			RegCloseKey( hKey );
			goto myerrorWriteRegistryUploadOptions;
	}

	dwBufLen=wcslen(aoptions->USERNAME)*2;
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"USERNAME",NULL,REG_SZ,(LPBYTE)aoptions->USERNAME, dwBufLen))
	{
			RegCloseKey( hKey );
			goto myerrorWriteRegistryUploadOptions;
	}


	RegCloseKey( hKey );
    return true;

myerrorWriteRegistryUploadOptions:
	return false;

}



bool ReadRegistration(PREGDATA aoptions)
{
	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\Registration",0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=sizeof(aoptions->EMAIL);
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"EMAIL", NULL, NULL,(LPBYTE) aoptions->EMAIL, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistration;
	};
	dwBufLen=64;
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"RegCode", NULL, NULL,(LPBYTE) aoptions->CODE, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistration;
	};
	
	ZeroMemory(aoptions->STRING,6);
	
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"Code", NULL, NULL,(LPBYTE) aoptions->STRING, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadRegistration;
	};
	DWORD ast;
	GetMACaddress(&ast);

	unsigned char * ser;

	unsigned char data[6];
	ZeroMemory(&data[0],6);
	memset(&data[0],0x90,5);
	ser = (unsigned char*)&ast;
	for (int i=0;i<5;i++)
	{
		aoptions->STRING[i] = aoptions->STRING[i] ^ ser[i];
	}
	aoptions->STRING[5]=0X90;


	RegCloseKey( hKey );
	return true;

myerrorReadRegistration:
	return false;

}

bool WriteRegistration(PREGDATA aoptions)
{

	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	if (ERROR_SUCCESS!=RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\Registration",0, KEY_ALL_ACCESS , &hKey ))
	{
		goto myerrorWriteRegistration;
	}

	dwBufLen=wcslen(aoptions->EMAIL)*2;
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"EMAIL",NULL,REG_SZ,(LPBYTE)aoptions->EMAIL, dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorWriteRegistration;
	}

	dwBufLen=wcslen(aoptions->CODE)*2;
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"RegCode",NULL,REG_SZ,(LPBYTE)aoptions->CODE, dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorWriteRegistration;
	}


	RegCloseKey( hKey );
	return true;

myerrorWriteRegistration:
	return false;

}


bool ReadVersion(WCHAR* astr)
{
	DWORD ou;
	bool salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=512;
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"version", NULL, NULL,(LPBYTE) astr, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadVersion;
	};
	RegCloseKey( hKey );
	return true;

myerrorReadVersion:
	return false;

}


bool Pinged(void)
{
	DWORD ou;
	bool salida;
	WCHAR astr[8];
	ZeroMemory(astr,sizeof(astr));
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder,0, KEY_ALL_ACCESS , &hKey );
	DWORD dwBufLen=512;
	if (ERROR_SUCCESS!=RegQueryValueExW( hKey, L"pingued", NULL, NULL,(LPBYTE) astr, &dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorReadpinged;
	};
	if (lstrcmpiW(astr,L"")==0)
	{
		dwBufLen=8;
		RegSetValueExW(hKey,L"pingued",NULL,REG_SZ,(LPBYTE)L"YES", dwBufLen);
		RegCloseKey( hKey );
		return true;
	}
	else
	{
		 RegCloseKey( hKey );
		 return false;
	}


myerrorReadpinged:
	return true;

}
bool WriteRegistrationCode(unsigned char* adata)
{

	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	if (ERROR_SUCCESS!=RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\Registration",0, KEY_ALL_ACCESS , &hKey ))
	{
		goto myerrorWriteRegistrationCode;
	}


	dwBufLen=5;
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"Code",NULL,REG_BINARY,(LPBYTE)&adata[0], dwBufLen))
	{
		RegCloseKey( hKey );
		goto myerrorWriteRegistrationCode;
	}

	RegCloseKey( hKey );
	return true;

myerrorWriteRegistrationCode:
	return false;

}


bool isRegistered()
{
	WCHAR ou[512];
	ZeroMemory(ou,1024);
	bool salida;
	HKEY hKey;
	DWORD dwBufLen;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\Registration",0, KEY_ALL_ACCESS , &hKey );
	dwBufLen = 512;
	if (ERROR_SUCCESS==RegQueryValueExW( hKey, L"Code", NULL, NULL,(LPBYTE) ou, &dwBufLen))
	{
		if (lstrcmpiW(ou,L"")==0)
		{
			salida=false;
		}
		else
		{
			salida=true;
		}
	}
	else
	{
		salida=false;
	};

	RegCloseKey( hKey );
    return salida;

}
void GetSourceDestinationSample(DWORD hzsource,DWORD*hzdestino,int* abit)
{
	RECORDINGOPTIONS arec;
	ReadRegistryRecordingOptions(&arec);
	switch (hzsource)
	{
		case 48000:
			if (arec.KHZ==0)
			{
				*hzdestino=48000;
			};
			if (arec.KHZ==1)
			{
				*hzdestino=24000;
			};
			if (arec.KHZ==2)
			{
				*hzdestino=12000;
			};

			if (arec.KHZ==3)
			{
				*hzdestino=8000;
			};

			break;

		case 8000:
			*hzdestino=8000;
			break;

		case 44100:
			if (arec.KHZ==0)
			{
				*hzdestino=44100;
			};
			if (arec.KHZ==1)
			{
				*hzdestino=22050;
			};
			if (arec.KHZ==2)
			{
				*hzdestino=11025;
			};

			if (arec.KHZ==3)
			{
				*hzdestino=11025;
			};
		
			break;
		
		case 22050:
			if (arec.KHZ==0)
			{
				*hzdestino=22050;
			};
			if (arec.KHZ==1)
			{
				*hzdestino=11025;
			};
			if (arec.KHZ==2)
			{
				*hzdestino=11025;
			};

			if (arec.KHZ==3)
			{
				*hzdestino=11025;
			};
		
			break;
		case 11025:
			*hzdestino=11025;
			break;
	}
	switch (arec.BitRate)
	{
		case 0:
			*abit=100;		
			break;
		case 1:
			*abit=50;
			break;
		case 2:
			*abit=20;
			break;
		case 3:
			*abit=10;
			break;
	}


//	*abit= 
	//arec.KHZ
/*
//high = 0;
//medium=1;
//low=2;
verylow=3;


*/

}

bool WriteWASAPIValues(int in_HZ , int in_channel , int in_position , int out_HZ , int out_channel , int out_position,int bus_hz)
{
	HKEY hKey;
	DWORD dwBufLen=sizeof(DWORD);
	DWORD al;
	if (ERROR_SUCCESS!=RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\WASAPI",0, KEY_ALL_ACCESS , &hKey ))
	{
		return false;
	}

	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"IN_CHANNELS",NULL,REG_DWORD,(LPBYTE)&in_channel, dwBufLen))
	{
		RegCloseKey( hKey );
		return false;
	}
	
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"IN_HZ",NULL,REG_DWORD,(LPBYTE)&in_HZ, dwBufLen))
	{
		RegCloseKey( hKey );
		return false;
	}

	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"IN_POS",NULL,REG_DWORD,(LPBYTE)&in_position, dwBufLen))
	{
		RegCloseKey( hKey );
		return false;
	}


	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"OUT_CHANNELS",NULL,REG_DWORD,(LPBYTE)&out_channel, dwBufLen))
	{
		RegCloseKey( hKey );
		return false;
	}
	
	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"OUT_HZ",NULL,REG_DWORD,(LPBYTE)&out_HZ, dwBufLen))
	{
		RegCloseKey( hKey );
		return false;
	}

	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"OUT_POS",NULL,REG_DWORD,(LPBYTE)&out_position, dwBufLen))
	{
		RegCloseKey( hKey );
		return false;
	}

	dwBufLen=sizeof(DWORD);
	if (ERROR_SUCCESS!=RegSetValueExW(hKey,L"BUS_HZ",NULL,REG_DWORD,(LPBYTE)&bus_hz, dwBufLen))
	{
		RegCloseKey( hKey );
		return false;
	}




	RegCloseKey( hKey );
    return true;



}

bool ReadWASAPIValues(int* in_HZ , int* in_channel , int* in_position , int* out_HZ , int* out_channel , int* out_position,int* bus_hz)
{
	DWORD ou;
	int salida;
	HKEY hKey;
	RegOpenKeyExW( HKEY_CURRENT_USER,L"SoftWare\\"MRecorder L"\\WASAPI",0, KEY_ALL_ACCESS , &hKey );

	DWORD dwBufLen=sizeof(DWORD);
	DWORD test = RegQueryValueExW( hKey, L"IN_CHANNELS", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		*in_channel=  ou;
	}


	dwBufLen=sizeof(DWORD);
	test = RegQueryValueExW( hKey, L"IN_HZ", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		*in_HZ=  ou;
	}

	dwBufLen=sizeof(DWORD);
	test = RegQueryValueExW( hKey, L"IN_POS", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		*in_position=  ou;
	}


	dwBufLen=sizeof(DWORD);
	test = RegQueryValueExW( hKey, L"OUT_CHANNELS", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		*out_channel=  ou;
	}


	dwBufLen=sizeof(DWORD);
	test = RegQueryValueExW( hKey, L"OUT_HZ", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		*out_HZ=  ou;
	}

	dwBufLen=sizeof(DWORD);
	test = RegQueryValueExW( hKey, L"OUT_POS", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		*out_position=  ou;
	}

	dwBufLen=sizeof(DWORD);
	test = RegQueryValueExW( hKey, L"BUS_HZ", NULL, NULL,(LPBYTE) &ou, &dwBufLen);
	if (ERROR_SUCCESS==test)
	{
		*bus_hz=  ou;
	}


	RegCloseKey( hKey );

	return true;
}
