#include "stdafx.h"

#pragma once

bool Pinged(void);

typedef enum _tipo_destino
{
	TIPO_WAV =0,
	TIPO_OGG,
	TIPO_SPEEX,
	TIPO_MP3,
	TIPO_WMA,
	TIPO_MPG,
	TIPO_YOUTUBE
}TIPO_DESTINO;

typedef enum _METHOD_UPLOAD
{
	HTTP=0,
	FTP,
	SAYANDPOST,
	YOUTUBE
}METHOD_UPLOAD,*PMETHOD_UPLOAD;


typedef struct _RECORDINGOPTIONS
{
	DWORD BitRate;
	DWORD Format;
	DWORD KHZ;
}
RECORDINGOPTIONS, *PRECORDINGOPTIONS;

typedef struct _UPLOADOPTIONS
{
	DWORD METHOD;
	WCHAR DOMAIN_NAME[128];
	WCHAR USERNAME[128];
	WCHAR PASSWORD[128];
	WCHAR KEYCODE[128];
}
UPLOADOPTIONS, *PUPLOADOPTIONS;

typedef struct _REGDATA
{
	WCHAR EMAIL[128];
	WCHAR CODE[128];
	unsigned char STRING[6];
}
REGDATA, *PREGDATA;




void ReadRegistryPath(WCHAR* apath);
void ReadRegistryDestinationPath(WCHAR* apath);
void ReadRegistryUtilsPath(WCHAR* apath);


void WriteRegistryRunatStart(bool write, HINSTANCE hInst);
bool ReadRegistryRunatStart(void);

void WriteRegistryAutorecord(bool write);
bool ReadRegistryAutorecord(void);

void WriteRegistryGauge(bool write);
bool ReadRegistryGauge(void);

void WriteRegistryHistory(bool write);
bool ReadRegistryHistory(void);

void WriteRegistryPlaylist(bool write);
bool ReadRegistryPlaylist(void);

void WriteRegistryOpacity(int perc);
int ReadRegistryOpacity(void);

void WriteRegistryAutoUpload(bool write);
bool ReadRegistryAutoUpload(void);

bool ReadRegistryShowPlaylist(void);
void WriteRegistryShowPlaylist(bool write);






// RecordingOptions
bool ReadRegistryRecordingOptions(PRECORDINGOPTIONS aoptions);
bool WriteRegistryRecordingOptions(PRECORDINGOPTIONS aoptions);

// UploadOptions
bool ReadRegistryUploadOptions(PUPLOADOPTIONS aoptions);
bool WriteRegistryUploadOptions(PUPLOADOPTIONS aoptions);


// Registrations
bool ReadRegistration(PREGDATA aoptions);
bool WriteRegistration(PREGDATA aoptions);

bool ReadVersion(WCHAR* astr);
void ReadRegistryExportPath(WCHAR* apath);
void ReadRegistryDBPath(WCHAR* apath);

bool WriteRegistrationCode(unsigned char* adata);
bool isRegistered();


void GetSourceDestinationSample(DWORD hzsource,DWORD*hzdestino,int* abitrate);

bool ReadWASAPIValues(int* in_HZ , int* in_channel , int* in_position , int* out_HZ , int* out_channel , int* out_position,int *bus_hz);
bool WriteWASAPIValues(int in_HZ , int in_channel , int in_position , int out_HZ , int out_channel , int out_position,int bus_hz);
