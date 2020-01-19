
//#define UNICODE
#include "stdlib.h"
#include "stdafx.h"
#include "windows.h"
#include "wav.h"

void GenerateWaveFile(WCHAR*  FileName,WCHAR* FileNameout, DWORD HZ , WORD CHANNELS )
{
	LPVOID mybuffer = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	DWORD filesizeHightBytes;
	DWORD filesizeLowBytes;
	DWORD readedBytes;
	DWORD writenBytes;
	HANDLE myfileRead  = CreateFileW(FileName , GENERIC_READ , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	HANDLE myfileWrite = CreateFileW(FileNameout , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	filesizeLowBytes   = GetFileSize(myfileRead,&filesizeHightBytes);

	
	TWAVERECORD WAVERECORD;

	

	WAVERECORD.riff[0] = 'R';
	WAVERECORD.riff[1] = 'I';
	WAVERECORD.riff[2] = 'F';
	WAVERECORD.riff[3] = 'F';


	WAVERECORD.tamano = 44 + filesizeLowBytes - 8;
	WAVERECORD.WAVEfmt[0] = 'W';
	WAVERECORD.WAVEfmt[1] = 'A';
	WAVERECORD.WAVEfmt[2] = 'V';
	WAVERECORD.WAVEfmt[3] = 'E';
	WAVERECORD.WAVEfmt[4] = 'f';
	WAVERECORD.WAVEfmt[5] = 'm';
	WAVERECORD.WAVEfmt[6] = 't';
	WAVERECORD.WAVEfmt[7] = ' ';
	WAVERECORD.formato = sizeof(WAVEFORMATEXAZ);
	ZeroMemory(&WAVERECORD.myw,sizeof(WAVEFORMATEXAZ));

	WAVERECORD.myw.nChannels       = CHANNELS;
	WAVERECORD.myw.nSamplesPerSec  = HZ;
	WAVERECORD.myw.wBitsPerSample  = 16;
	WAVERECORD.myw.wFormatTag      = 1;
	WAVERECORD.myw.nBlockAlign     = WAVERECORD.myw.wBitsPerSample  *  WAVERECORD.myw.nChannels /8;
	WAVERECORD.myw.nAvgBytesPerSec = WAVERECORD.myw.nChannels * (WAVERECORD.myw.wBitsPerSample * WAVERECORD.myw.nSamplesPerSec) /8;


	WAVERECORD.datos[0]='d';
	WAVERECORD.datos[1]='a';
	WAVERECORD.datos[2]='t';
	WAVERECORD.datos[3]='a';
	WAVERECORD.bytes_muestra = filesizeLowBytes ;

		
	WriteFile(myfileWrite,(LPVOID) &WAVERECORD,sizeof(WAVERECORD),&writenBytes,NULL);
	while ((ReadFile(myfileRead,mybuffer,16384,&readedBytes,NULL) != 0) &&  (readedBytes !=0))
	{
		WriteFile(myfileWrite,mybuffer,readedBytes,&writenBytes,NULL);
	}
	CloseHandle(myfileRead);
	CloseHandle(myfileWrite);
	VirtualFree(mybuffer,MEM_DECOMMIT,16384);
	VirtualFree(mybuffer,MEM_RELEASE,0);

};
