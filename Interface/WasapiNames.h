#pragma once
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <functiondiscoverykeys.h>

typedef struct  _RECORD_WASAPI_NAMES
{
	int pos;
	WCHAR devicename[512];
	int channels;
	int hz;
} RECORD_WASAPI_NAMES, * PRECORD_WASAPI_NAMES ;


bool getNames(PRECORD_WASAPI_NAMES thenames,bool is_mic);

void initialize_first (void);




template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}