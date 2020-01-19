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
unsigned char unbufferRollback_WASAPIGetBufferMic[30];
unsigned char unbufferPatch_WASAPIGetBufferMic[7];
bool PatchMemoryWASAPIGetBufferMic();


DWORD theRealFunction_WASAPIGetBufferSpeaker;
unsigned char unbufferRollback_WASAPIGetBufferSpeaker[30];
unsigned char unbufferPatch_WASAPIGetBufferSpeaker[7];
bool PatchMemoryWASAPIGetBufferSpeaker();


DWORD theRealFunction_WASAPIReleaseBufferSpeaker;
unsigned char unbufferRollback_WASAPIReleaseBufferSpeaker[30];
unsigned char unbufferPatch_WASAPIReleaseBufferSpeaker[7];
bool PatchMemoryWASAPIReleaseBufferSpeaker();





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
		sprintf(salidas,"%s","WASAPIGetBufferMic");
		OutputDebugStringA(salidas);

		HRESULT salida;
		LPVOID  patcheado = &unbufferRollback_WASAPIGetBufferMic;
		__asm                  
		{
			push pu64QPCPosition
			push pu64DevicePosition
			push pdwFlags
			push pNumFramesToRead
			push ppData
			mov 	edx   , pDSB
			mov 	ecx   , pDSB
			push        edx  
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
		sprintf(salidas,"%s","WASAPIGetBufferOut");
		OutputDebugStringA(salidas);

	
		HRESULT salida;
		LPVOID  patcheado = &unbufferRollback_WASAPIGetBufferSpeaker;
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
		sprintf(salidas,"%s","WASAPIReleaseBufferOut");
		OutputDebugStringA(salidas);

		HRESULT salida;
		LPVOID  patcheado = &unbufferRollback_WASAPIReleaseBufferSpeaker;
		__asm                  
		{
			push dwFlags
			push NumFramesWritten
			mov 	edx   , pDSB
			mov 	ecx   , pDSB
			push        edx  
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

	setPatchMic((DWORD*)_RenderClient);
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
	DWORD* FIRST_VIRTUAL_TABLE;
	DWORD* MY_PATCH_FUNCTION ;
	DWORD intable=3;
	FIRST_VIRTUAL_TABLE  = arrayDWORDFunctionBase;
	DWORD* aux 	= (DWORD*)((DWORD*) ((*FIRST_VIRTUAL_TABLE)+ (intable * 4)));
	
	theRealFunction_WASAPIGetBufferMic = * aux;
	PatchMemoryWASAPIGetBufferMic();
}	


void setPatchSpeaker( DWORD*  arrayDWORDFunctionBase)
{
	DWORD* FIRST_VIRTUAL_TABLE;
	DWORD* MY_PATCH_FUNCTION ;

	DWORD intable=3;
	FIRST_VIRTUAL_TABLE  = arrayDWORDFunctionBase;
	DWORD* aux 	= (DWORD*)((DWORD*) ((*FIRST_VIRTUAL_TABLE)+ (intable * 4)));
	theRealFunction_WASAPIGetBufferSpeaker = * aux;
	PatchMemoryWASAPIGetBufferSpeaker();


	intable=4;
	FIRST_VIRTUAL_TABLE  = arrayDWORDFunctionBase;
	aux 	= (DWORD*)((DWORD*) ((*FIRST_VIRTUAL_TABLE)+ (intable * 4)));
	theRealFunction_WASAPIReleaseBufferSpeaker = * aux;
	PatchMemoryWASAPIReleaseBufferSpeaker();
}







int _tmain(int argc, _TCHAR* argv[])
{
	getPosWSAPIout();
	getPosWSAPIin();

	return 0;
}



// DWORD theRealFunction_WASAPIGetBufferMic;
// unsigned char unbufferRollback_WASAPIGetBufferMic[30];
// unsigned char unbufferPatch_WASAPIGetBufferMic[7];



int GetPositionCOM3(LPVOID buffer, int elementos)
{
	return 24;
}




bool PatchMemoryWASAPIGetBufferMic()
{
	LPVOID posicion;
    DWORD dwProtect_antes;
    HRESULT hr;
	// posicion de memoria a sobreescribir
	// fijarse como averiguar esta posicion de memoria !!!!
	// http://www.codeproject.com/com/cominterfacehookingpart.asp
	//MyProcess_waveIn = (DWORD)&g_pDSBCapture;
	//MyProcess_waveIn =  0x510A952A;
	if (0 != VirtualProtect((LPVOID)theRealFunction_WASAPIGetBufferMic, 30, PAGE_EXECUTE_READWRITE, &dwProtect_antes))
	{
	    // ponemos en 0 los datos del codigo de patch y del codigo de rollback
		ZeroMemory(unbufferPatch_WASAPIGetBufferMic,7);
		ZeroMemory(unbufferRollback_WASAPIGetBufferMic,30);

		// posicion para cortar, dessasembler COM;
		unsigned int posi = GetPositionCOM3((LPVOID)theRealFunction_WASAPIGetBufferMic,30);

		printf("%u",posi);
		
		MoveMemory(&unbufferRollback_WASAPIGetBufferMic[0], (LPVOID)theRealFunction_WASAPIGetBufferMic, posi);
		
		// patch code de rollback
		//move eax ,
		unbufferRollback_WASAPIGetBufferMic[posi] = 0xB8;
        //pointerfunc :=@fantasia;
		posicion = ((char*) (WASAPIGetBufferMic)+posi) ;

		MoveMemory(&unbufferRollback_WASAPIGetBufferMic[posi+1], &posicion, 4);

		// jmp eax
		unbufferRollback_WASAPIGetBufferMic[posi+5] = 0xFF;
		unbufferRollback_WASAPIGetBufferMic[posi+6] = 0xE0;
		// nop
		unbufferRollback_WASAPIGetBufferMic[posi+7] = 0x90;
		
		// hago ejecutable el codigo del rollback
		VirtualProtect(&unbufferRollback_WASAPIGetBufferMic, 30, PAGE_EXECUTE_READWRITE, &dwProtect_antes);
		// fin codigo del rollback

		// patch code comun
        //move eax ,
	

		unbufferPatch_WASAPIGetBufferMic[0]=0xB8;
        // valor para eax (posicion de pointerfunc) (la posicion de la funcion magica)
        //pointerfunc :=@fantasia;
		//	aca hay que ver si quiero skype o quiero otra cosa
		posicion = &theRealFunction_WASAPIGetBufferMic;
		MoveMemory(&unbufferPatch_WASAPIGetBufferMic[1], &posicion, 4);
		// jmp eax
        unbufferPatch_WASAPIGetBufferMic[5] = 0xFF;
        unbufferPatch_WASAPIGetBufferMic[6] = 0xE0;
		//Patchear la memoria con el nuevo codigo;
		MoveMemory((LPVOID)theRealFunction_WASAPIGetBufferMic,&unbufferPatch_WASAPIGetBufferMic,7);
		return 1;
	}
	return 0;
};



	 
bool PatchMemoryWASAPIGetBufferSpeaker()
{
	LPVOID posicion;
    DWORD dwProtect_antes;
    HRESULT hr;
	// posicion de memoria a sobreescribir
	// fijarse como averiguar esta posicion de memoria !!!!
	// http://www.codeproject.com/com/cominterfacehookingpart.asp
	//MyProcess_waveIn = (DWORD)&g_pDSBCapture;
	//MyProcess_waveIn =  0x510A952A;
	if (0 != VirtualProtect((LPVOID)theRealFunction_WASAPIGetBufferMic, 30, PAGE_EXECUTE_READWRITE, &dwProtect_antes))
	{
	    // ponemos en 0 los datos del codigo de patch y del codigo de rollback
		ZeroMemory(unbufferPatch_WASAPIGetBufferMic,7);
		ZeroMemory(unbufferRollback_WASAPIGetBufferMic,30);

		// posicion para cortar, dessasembler COM;
		unsigned int posi = GetPositionCOM3((LPVOID)theRealFunction_WASAPIGetBufferMic,30);

		printf("%u",posi);
		
		MoveMemory(&unbufferRollback_WASAPIGetBufferMic[0], (LPVOID)theRealFunction_WASAPIGetBufferMic, posi);
		
		// patch code de rollback
		//move eax ,
		unbufferRollback_WASAPIGetBufferMic[posi] = 0xB8;
        //pointerfunc :=@fantasia;
		posicion = ((char*) (WASAPIGetBufferMic)+posi) ;

		MoveMemory(&unbufferRollback_WASAPIGetBufferMic[posi+1], &posicion, 4);

		// jmp eax
		unbufferRollback_WASAPIGetBufferMic[posi+5] = 0xFF;
		unbufferRollback_WASAPIGetBufferMic[posi+6] = 0xE0;
		// nop
		unbufferRollback_WASAPIGetBufferMic[posi+7] = 0x90;
		
		// hago ejecutable el codigo del rollback
		VirtualProtect(&unbufferRollback_WASAPIGetBufferMic, 30, PAGE_EXECUTE_READWRITE, &dwProtect_antes);
		// fin codigo del rollback

		// patch code comun
        //move eax ,
	

		unbufferPatch_WASAPIGetBufferMic[0]=0xB8;
        // valor para eax (posicion de pointerfunc) (la posicion de la funcion magica)
        //pointerfunc :=@fantasia;
		//	aca hay que ver si quiero skype o quiero otra cosa
		posicion = &theRealFunction_WASAPIGetBufferMic;
		MoveMemory(&unbufferPatch_WASAPIGetBufferMic[1], &posicion, 4);
		// jmp eax
        unbufferPatch_WASAPIGetBufferMic[5] = 0xFF;
        unbufferPatch_WASAPIGetBufferMic[6] = 0xE0;
		//Patchear la memoria con el nuevo codigo;
		MoveMemory((LPVOID)theRealFunction_WASAPIGetBufferMic,&unbufferPatch_WASAPIGetBufferMic,7);
		return 1;
	}
	return 0;
};



/*
DWORD theRealFunction_WASAPIReleaseBufferSpeaker;
unsigned char unbufferRollback_WASAPIReleaseBufferSpeaker[30];
unsigned char unbufferPatch_WASAPIReleaseBufferSpeaker[7];
bool PatchMemoryWASAPIReleaseBufferSpeaker();}
WASAPIReleaseBufferOut
*/

bool PatchMemoryWASAPIReleaseBufferSpeaker()
{
	LPVOID posicion;
    DWORD dwProtect_antes;
    HRESULT hr;
	// posicion de memoria a sobreescribir
	// fijarse como averiguar esta posicion de memoria !!!!
	// http://www.codeproject.com/com/cominterfacehookingpart.asp
	//MyProcess_waveIn = (DWORD)&g_pDSBCapture;
	//MyProcess_waveIn =  0x510A952A;
	if (0 != VirtualProtect((LPVOID)theRealFunction_WASAPIReleaseBufferSpeaker, 30, PAGE_EXECUTE_READWRITE, &dwProtect_antes))
	{
	    // ponemos en 0 los datos del codigo de patch y del codigo de rollback
		ZeroMemory(unbufferPatch_WASAPIReleaseBufferSpeaker,7);
		ZeroMemory(unbufferRollback_WASAPIReleaseBufferSpeaker,30);

		// posicion para cortar, dessasembler COM;
		unsigned int posi = GetPositionCOM3((LPVOID)theRealFunction_WASAPIReleaseBufferSpeaker,30);
		printf("%u",posi);
		
		MoveMemory(&unbufferRollback_WASAPIReleaseBufferSpeake[0], (LPVOID)theRealFunction_WASAPIReleaseBufferSpeaker, posi);
		// patch code de rollback
		//move eax ,
		unbufferRollback_WASAPIReleaseBufferSpeake[posi] = 0xB8;
        //pointerfunc :=@fantasia;
		posicion = ((char*) (WASAPIReleaseBufferOut)+posi) ;

		MoveMemory(&unbufferRollback_WASAPIReleaseBufferSpeake[posi+1], &posicion, 4);

		// jmp eax
		unbufferRollback_WASAPIReleaseBufferSpeake[posi+5] = 0xFF;
		unbufferRollback_WASAPIReleaseBufferSpeake[posi+6] = 0xE0;
		// nop
		unbufferRollback_WASAPIReleaseBufferSpeake[posi+7] = 0x90;
		
		// hago ejecutable el codigo del rollback
		VirtualProtect(&unbufferRollback_WASAPIReleaseBufferSpeake, 30, PAGE_EXECUTE_READWRITE, &dwProtect_antes);
		// fin codigo del rollback



		// patch code comun
        //move eax ,
		unbufferPatch_WASAPIReleaseBufferSpeaker[0]=0xB8;
        // valor para eax (posicion de pointerfunc) (la posicion de la funcion magica)
        //pointerfunc :=@fantasia;
		//	aca hay que ver si quiero skype o quiero otra cosa
		posicion = &WASAPIReleaseBufferOut;
		MoveMemory(&unbufferPatch_WASAPIReleaseBufferSpeaker[1], &posicion, 4);
		// jmp eax
        unbufferPatch_WASAPIReleaseBufferSpeaker[5] = 0xFF;
        unbufferPatch_WASAPIReleaseBufferSpeaker[6] = 0xE0;
		//Patchear la memoria con el nuevo codigo;
		MoveMemory((LPVOID)theRealFunction_WASAPIReleaseBufferSpeaker,&unbufferPatch_WASAPIReleaseBufferSpeaker,7);
		return 1;
	}
	return 0;
};
