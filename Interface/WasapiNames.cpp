#include "WasapiNames.h"
#include "stdafx.h"
#include "reg.h"


LPWSTR GetDeviceName2(IMMDeviceCollection *DeviceCollection, UINT DeviceIndex)
{

    IMMDevice *device;
    LPWSTR deviceId;
    HRESULT hr;

    hr = DeviceCollection->Item(DeviceIndex, &device);
    if (FAILED(hr))
    {
        //printf("Unable to get device %d: %x\n", DeviceIndex, hr);
        return NULL;
    }
    hr = device->GetId(&deviceId);
    if (FAILED(hr))
    {
        //printf("Unable to get device %d id: %x\n", DeviceIndex, hr);
        return NULL;
    }

    IPropertyStore *propertyStore;
    hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
    SafeRelease(&device);
    if (FAILED(hr))
    {
        //printf("Unable to open device %d property store: %x\n", DeviceIndex, hr);
        return NULL;
    }

    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
    SafeRelease(&propertyStore);

    if (FAILED(hr))
    {
        //printf("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
        return NULL;
    }

    wchar_t deviceName[128];
 //   hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
    hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal);
    if (FAILED(hr))
    {
        //printf("Unable to format friendly name for device %d : %x\n", DeviceIndex, hr);
        return NULL;
    }

    PropVariantClear(&friendlyName);
    CoTaskMemFree(deviceId);

    wchar_t *returnValue = _wcsdup(deviceName);
    if (returnValue == NULL)
    {
        //printf("Unable to allocate buffer for return\n");
        return NULL;
    }
    return returnValue;
}


bool getNames(PRECORD_WASAPI_NAMES thenames,bool is_mic)
{
	IAudioClient *myAudioClient;

	char salida[1024];
	ZeroMemory(salida,sizeof(salida));

   HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
   if (FAILED(hr))
    {
        //printf("Unable to initialize COM: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to initialize COM: %x\n");
		//OutputDebugStringA(salida);
		return false;
    }

    IMMDeviceEnumerator *deviceEnumerator = NULL;
	IMMDeviceCollection *deviceCollection = NULL;

	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
	    //printf("Unable to instantiate device enumerator: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to instantiate device enumerator\n");
		//OutputDebugStringA(salida);
		return  false;
    }

    IMMDevice *device = NULL;

	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);
	if (is_mic)
	{
		hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
	}
	else
	{
		hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
	}


    UINT deviceCount;
    hr = deviceCollection->GetCount(&deviceCount);
    if (FAILED(hr))
    {
            printf("Unable to get device collection length: %x\n", hr);
    }
    for (UINT i = 0 ; i < deviceCount ; i += 1)
    {
            LPWSTR deviceName;
            deviceName = GetDeviceName2(deviceCollection, i);
            if (deviceName == NULL)
            {
            }
         //    printf("    %d:  %S\n", i , deviceName);
			 wsprintf(thenames[i].devicename,L"%s",deviceName); 
             free(deviceName);
			 hr = deviceCollection->Item(i, &device);
			 hr =device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&myAudioClient));
			 if (FAILED(hr))
			 {
		//		printf("Unable to activate audio client: %x.\n", hr);
				ZeroMemory(salida,sizeof(salida));
				sprintf(salida,"%s","Unable to activate audio client");
				//OutputDebugStringA(salida);
			}
			WAVEFORMATEX *MixFormat;
			hr = myAudioClient->GetMixFormat(&MixFormat);
			ZeroMemory(salida,sizeof(salida));
			sprintf(salida,"Hz = %d channels = %d",MixFormat->nSamplesPerSec, MixFormat->nChannels );
			//OutputDebugStringA(salida);
			thenames[i].channels= MixFormat->nChannels;
			thenames[i].hz=MixFormat->nSamplesPerSec;
			thenames[i].pos=i;
		//	printf("Hz = %d channels = %d",MixFormat->nSamplesPerSec, MixFormat->nChannels );
    }

}


void get_channels_and_hz( PRECORD_WASAPI_NAMES thenames,bool is_mic)
{
   IAudioClient *myAudioClient;
   char salida[1024];
   ZeroMemory(salida,sizeof(salida));
   HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
   if (FAILED(hr))
    {
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to initialize COM: %x\n");
		//OutputDebugStringA(salida);
    }

    IMMDeviceEnumerator *deviceEnumerator = NULL;
	IMMDeviceCollection *deviceCollection = NULL;

	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to instantiate device enumerator\n");
		//OutputDebugStringA(salida);
    }

    IMMDevice *device = NULL;
	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);
	ERole deviceRole = eCommunications;
	if (is_mic)
	{
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, deviceRole, &device);
	}
	else
	{
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, deviceRole, &device);
	}

	hr =device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&myAudioClient));
	if (FAILED(hr))
	{
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to activate audio client");
		//OutputDebugStringA(salida);
	}
	WAVEFORMATEX *MixFormat;
	hr = myAudioClient->GetMixFormat(&MixFormat);
	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"Hz = %d channels = %d",MixFormat->nSamplesPerSec, MixFormat->nChannels );
	//OutputDebugStringA(salida);
	thenames->channels= MixFormat->nChannels;
	thenames->hz=MixFormat->nSamplesPerSec;
	thenames->pos=0;
}





void initialize_first (void)
{
	// verificar si esta 0 hz posicion y canales
	int in_HZ =0;
	int in_channel =0;
	int in_position =0;
	int out_HZ =0;
	int out_channel=0 ;
	int out_position=0;	
	int bus_hz;
	RECORD_WASAPI_NAMES therecordMic;
	RECORD_WASAPI_NAMES therecordSpeaker;

	ZeroMemory(&therecordMic,sizeof(RECORD_WASAPI_NAMES));
	ZeroMemory(&therecordSpeaker,sizeof(RECORD_WASAPI_NAMES));

	
	ReadWASAPIValues(&in_HZ , &in_channel , &in_position , &out_HZ , &out_channel , &out_position,&bus_hz);
	if ((in_channel == 0) || (out_channel == 0))
	{
		get_channels_and_hz( &therecordMic,true);
		get_channels_and_hz( &therecordSpeaker,false);
		WriteWASAPIValues(therecordMic.hz , therecordMic.channels , therecordMic.pos , therecordSpeaker.hz , therecordSpeaker.channels , therecordSpeaker.pos,therecordSpeaker.hz);
	}
}
