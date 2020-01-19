// TestVistaWasapi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <AudioClient.h>
#include <MMDeviceAPI.h>
#include <AudioPolicy.h>


IAudioClient *_AudioClient;
IAudioRenderClient *_RenderClient;
IAudioCaptureClient *_CaptureClient;

DWORD theRealFunction_WASAPIGetBufferMic;


DWORD theRealFunction_WASAPIGetBufferSpeaker;


DWORD theRealFunction_WASAPIReleaseBufferSpeaker;





void setPatchMic( DWORD*  arrayDWORDFunctionBase);
void setPatchSpeaker( DWORD*  arrayDWORDFunctionBase);



HRESULT STDMETHODCALLTYPE WASAPIGetBufferMic( 
			DWORD       pDSB,
            BYTE **ppData,
            UINT32 *pNumFramesToRead,
            DWORD *pdwFlags,
            UINT64 *pu64DevicePosition,
            UINT64 *pu64QPCPosition)
{
		char salidas[512];
		ZeroMemory(salidas,sizeof(salidas));
		sprintf(salidas,"%s\n","WASAPIGetBufferMic");
		OutputDebugStringA(salidas);

		HRESULT salida;
		LPVOID  patcheado =(LPVOID) theRealFunction_WASAPIGetBufferMic;
		__asm                  
		{
			push pu64QPCPosition
			push pu64DevicePosition
			push pdwFlags
			push pNumFramesToRead
			push ppData
			mov 	ecx   , pDSB
			push        pDSB  
			call patcheado
			mov salida ,eax
		}



	return salida;
}


HRESULT STDMETHODCALLTYPE WASAPIGetBufferOut(
				DWORD       pDSB,
				UINT32 NumFramesRequested,
				BYTE **ppData)
{
		char salidas[512];
		ZeroMemory(salidas,sizeof(salidas));
		sprintf(salidas,"%s\n","WASAPIGetBufferOut");
		OutputDebugStringA(salidas);

	
		HRESULT salida;
		LPVOID  patcheado = (LPVOID)theRealFunction_WASAPIGetBufferSpeaker;
		__asm                  
		{
			push ppData
			push NumFramesRequested
			mov 	edx   , pDSB
			mov 	ecx   , pDSB
			push        edx  
			call patcheado
			mov salida ,eax
		}
	return salida;
}
      

HRESULT STDMETHODCALLTYPE WASAPIReleaseBufferOut( 
				DWORD       pDSB,
				UINT32 NumFramesWritten,
				DWORD dwFlags)
{

		char salidas[512];
		ZeroMemory(salidas,sizeof(salidas));
		sprintf(salidas,"%s\n","WASAPIReleaseBufferOut");
		OutputDebugStringA(salidas);

		HRESULT salida;
		LPVOID  patcheado = (LPVOID)theRealFunction_WASAPIReleaseBufferSpeaker;
		__asm                  
		{
			push dwFlags
			push NumFramesWritten
			push        pDSB  
			call patcheado
			mov salida ,eax
		}
	return salida;

}





bool getPosWSAPIout(void)
{
	char salida[1024];
	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","getPosWSAPIOut");
	OutputDebugStringA(salida);

    UINT32      _BufferSize;
	WAVEFORMATEX *_MixFormat;
    //  A GUI application should use COINIT_APARTMENTTHREADED instead of COINIT_MULTITHREADED.
    //      HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
   if (FAILED(hr))
    {
        printf("Unable to initialize COM: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to initialize COM: %x\n");
		OutputDebugStringA(salida);

		return false;
    }

    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDeviceEnumerator *   _DeviceEnumerator;

	IMMDeviceCollection *deviceCollection = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	OutputDebugStringA(salida);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
	        printf("Unable to instantiate device enumerator: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to instantiate device enumerator\n");
		OutputDebugStringA(salida);

		return  false;
    }

    IMMDevice *device = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	OutputDebugStringA(salida);

	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
        ERole deviceRole = eConsole;    // Assume we're using the console role.


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","GetDefaultAudioEndpoint");
	OutputDebugStringA(salida);

		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, deviceRole, &device);
        if (FAILED(hr))
        {
            printf("Unable to get default device for role %d: %x\n", deviceRole, hr);
			ZeroMemory(salida,sizeof(salida));
			sprintf(salida,"%s","Unable to get default device for role");
			OutputDebugStringA(salida);

			return false;
		}



	IMMDevice *Endpoint = device;

	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","Activate");
	OutputDebugStringA(salida);

    hr = Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        printf("Unable to activate audio client: %x.\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to activate audio client");
		OutputDebugStringA(salida);

		return false;
    }

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
        return false;
    }


	  hr = _AudioClient->GetMixFormat(&_MixFormat);


     hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 
        AUDCLNT_STREAMFLAGS_NOPERSIST, 
        30*10000, 
        0, 
        _MixFormat, 
        NULL);

    if (FAILED(hr))
    {
        printf("Unable to initialize audio client: %x.\n", hr);
        return false;
    }

    //
    //  Retrieve the buffer size for the audio client.
    //
    hr = _AudioClient->GetBufferSize(&_BufferSize);
    if(FAILED(hr))
    {
        printf("Unable to get audio client buffer: %x. \n", hr);
        return false;
    }

    hr = _AudioClient->GetService(IID_PPV_ARGS(&_RenderClient));
    if (FAILED(hr))
    {
        printf("Unable to get new render client: %x.\n", hr);
        return false;
    }
	 setPatchSpeaker( (DWORD*)  _RenderClient);

	//_RenderClient->GetBuffer();
	//render client. a patchear.
	/*
	    virtual HRESULT STDMETHODCALLTYPE GetBuffer( 
            __in  UINT32 NumFramesRequested,
            __out  BYTE **ppData) = 0;
        GetBuffer
		ReleaseBuffer
        virtual HRESULT STDMETHODCALLTYPE ReleaseBuffer( 
            __in  UINT32 NumFramesWritten,
            __in  DWORD dwFlags) = 0;
     */

}



bool getPosWSAPIin(void)
{
	char salida[1024];
	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","getPosWSAPIIn");
	OutputDebugStringA(salida);

    UINT32      _BufferSize;
	WAVEFORMATEX *_MixFormat;
    //  A GUI application should use COINIT_APARTMENTTHREADED instead of COINIT_MULTITHREADED.
    //      HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
   if (FAILED(hr))
    {
        printf("Unable to initialize COM: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to initialize COM: %x\n");
		OutputDebugStringA(salida);

		return false;
    }

    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDeviceEnumerator *   _DeviceEnumerator;

	IMMDeviceCollection *deviceCollection = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	OutputDebugStringA(salida);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
	        printf("Unable to instantiate device enumerator: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to instantiate device enumerator\n");
		OutputDebugStringA(salida);

		return  false;
    }

    IMMDevice *device = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	OutputDebugStringA(salida);


	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
        ERole deviceRole = eConsole;    // Assume we're using the console role.


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","GetDefaultAudioEndpoint");
	OutputDebugStringA(salida);

		hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, deviceRole, &device);
        if (FAILED(hr))
        {
            printf("Unable to get default device for role %d: %x\n", deviceRole, hr);
			ZeroMemory(salida,sizeof(salida));
			sprintf(salida,"%s","Unable to get default device for role");
			OutputDebugStringA(salida);

			return false;
		}



	IMMDevice *Endpoint = device;

	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","Activate");
	OutputDebugStringA(salida);

    hr = Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        printf("Unable to activate audio client: %x.\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to activate audio client");
		OutputDebugStringA(salida);

		return false;
    }

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
        return false;
    }


	  hr = _AudioClient->GetMixFormat(&_MixFormat);


     hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 
        AUDCLNT_STREAMFLAGS_NOPERSIST, 
        30*10000, 
        0, 
        _MixFormat, 
        NULL);

    if (FAILED(hr))
    {
        printf("Unable to initialize audio client: %x.\n", hr);
        return false;
    }

    //
    //  Retrieve the buffer size for the audio client.
    //
    hr = _AudioClient->GetBufferSize(&_BufferSize);
    if(FAILED(hr))
    {
        printf("Unable to get audio client buffer: %x. \n", hr);
        return false;
    }

/*   
	hr = _AudioClient->GetService(IID_PPV_ARGS(&_RenderClient));
    if (FAILED(hr))
    {
        printf("Unable to get new render client: %x.\n", hr);
        return false;
    }
	*/

    hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
    if (FAILED(hr))
    {
        printf("Unable to get new capture client: %x.\n", hr);
        return false;
    }

	setPatchMic((DWORD*)_CaptureClient);
	//_RenderClient->GetBuffer();
	//render client. a patchear.
	// posicion array 3
	/*
0x73876754  73886829 73876ee1 73878d76 73876880 73876950 73876f79 73879969 73878daa 738862a9 73876ee1 738766dc 738790b4  )hˆsán.sv..s€h.sPi.syo.si™.sª..s©bˆsán.sÜf.s´..s
0x73876784  73876a20 7388623b 73876f79 738798a8 73878c64 90909090 90909090 8b55ff8b 084d8bec 7c004db8 56c83b88 8e0ff18b   j.s;bˆsyo.s¨˜.sdŒ.s.........ÿU.ì.M.¸M.|ˆ;ÈV.ñ.Ž
0x738767B4  000075a8 004ff981 840f887c 00001c44 0050f981 840f887c 00001c2e 0051f981 840f887c 00001c18 0003f981 840f887d  ¨u...ùO.|ˆ..D....ùP.|ˆ.......ùQ.|ˆ.......ù..}ˆ..
0x738767E4  00001c02 0009f981 840f887d 00001bf6 850fc985 00001bcd 5d5ec68b 900004c2 90909090 90909090 90909090 8b55ff8b  .....ù..}ˆ..ö....É..Í....Æ^]Â................ÿU.
0x73876814  575653ec 8d087d8b 0000889f 15ff5300 738710a8 c0183d83 0f007388 0078c485 78778b00 8c0ff685 000078cc 000c7d83  ìSVW.}..Ÿˆ...Sÿ.¨..sƒ=.Àˆs...Äx...wx.ö.ŒÌx..ƒ}..
0x73876844 


	73876880
			73886331  mov         edi,edi 
			73886333  push        ebp  
			73886334  mov         ebp,esp 
			73886336  sub         esp,3Ch 
			73886339  and         dword ptr [ebp-4],0 
			7388633D  and         dword ptr [ebp-0Ch],0 
			73886341  push        esi  
			73886342  mov         esi,dword ptr [ebp+8] 
			73886345  mov         ecx,esi 
			73886347  mov         dword ptr [ebp-8],414350

        virtual HRESULT STDMETHODCALLTYPE GetBuffer( 
            __out  BYTE **ppData,
            __out  UINT32 *pNumFramesToRead,
            __out  DWORD *pdwFlags,
            __out_opt  UINT64 *pu64DevicePosition,
            __out_opt  UINT64 *pu64QPCPosition) = 0;
			*/
}




	// obtencion de la posicion de memoria a patchear WAVE OUT
void setPatchMic( DWORD*  arrayDWORDFunctionBase)
{
//		__asm int 3

	DWORD* FIRST_VIRTUAL_TABLE;
	DWORD* MY_PATCH_FUNCTION ;
	DWORD intable=3;
	DWORD dwProtect_antes;

	FIRST_VIRTUAL_TABLE  = arrayDWORDFunctionBase;

	DWORD* aux 	= (DWORD*)((DWORD*) ((*FIRST_VIRTUAL_TABLE)+ (intable * 4)));
	theRealFunction_WASAPIGetBufferMic = * aux;

	VirtualProtect((LPVOID)aux, 4, PAGE_EXECUTE_READWRITE, &dwProtect_antes);
	* aux = (DWORD) &WASAPIGetBufferMic;


}	


void setPatchSpeaker( DWORD*  arrayDWORDFunctionBase)
{
	// __asm int 3
	DWORD* FIRST_VIRTUAL_TABLE;
	DWORD* MY_PATCH_FUNCTION ;
	DWORD intable;
	DWORD* aux;
	DWORD dwProtect_antes;
	
	intable=3;
	FIRST_VIRTUAL_TABLE  = arrayDWORDFunctionBase;
	aux 	= (DWORD*)((DWORD*) ((*FIRST_VIRTUAL_TABLE)+ (intable * 4)));
	theRealFunction_WASAPIGetBufferSpeaker = * aux;
	VirtualProtect((LPVOID)aux, 4, PAGE_EXECUTE_READWRITE, &dwProtect_antes);
	* aux = (DWORD) &WASAPIGetBufferOut;



	intable=4;
	FIRST_VIRTUAL_TABLE  = arrayDWORDFunctionBase;
	aux 	= (DWORD*)((DWORD*) ((*FIRST_VIRTUAL_TABLE)+ (intable * 4)));
	theRealFunction_WASAPIReleaseBufferSpeaker = * aux;
	VirtualProtect((LPVOID)aux, 4, PAGE_EXECUTE_READWRITE, &dwProtect_antes);
	* aux = (DWORD) &WASAPIReleaseBufferOut;
}

