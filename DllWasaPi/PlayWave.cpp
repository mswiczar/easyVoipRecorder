#include "stdlib.h"
#include "stdafx.h"
#include "windows.h"
#include "mmsystem.h"
#include "Mmreg.h"
#include "WAV.h"
#include "config.h"
#include "Clist.h"
#include "encoder_example.h"

#include "..\\interface\\playlist.h"
#include "..\\interface\\reg.h"
#include "..\\interface\\helper.h"
#include <math.h>

#include <AudioClient.h>
#include <MMDeviceAPI.h>
#include <AudioPolicy.h>
#include "Vistapatch.h"

LONG volatile good1=0;

LONG volatile count000 = 0;
//DWORD good1=0;

LONGLONG volatile last_timeworked_in=0;


#define MSG_MICBEGIN               WM_USER + 9012
#define MSG_MICEND                 WM_USER + 9013

#define MSG_SPEAKERDATA            WM_USER + 9030
#define MSG_MICDATA                WM_USER + 9040
#define MSG_SIZEKB                 WM_USER + 9050
#define MSG_REC_STOP               WM_USER + 9003

#define MSG_REFRESHPLAYLIST        WM_USER + 8000
#define MSG_SHOWDASHBOARD          WM_USER + 8001

int posicion_arreglo               = -1;


LPVOID void_pointer_from_out       = NULL;
LPVOID void_pointer_from_in        = NULL;
RECORDINGOPTIONS aoptionsRecording;

bool first     = true;
bool is_skype  = false;
bool is_MSN    = false;
bool is_oly    = false;
bool is_other  = false;

RECORD_PLAYLIST arecord;



bool is_mono=true;
bool firs_is_mono=true;

bool is_rec_stereo = false;

long volatile Is_direct_sound=0;
CPlaylist* aplaylist;
bool is_mono_out_new=false;


bool is_skype61ornew =false;



void checkVersion() 
{ 


	TCHAR exepath[MAX_PATH+1];

	TCHAR dest[MAX_PATH+1];

	GetModuleFileName(0, exepath, MAX_PATH+1);


	DWORD dwDummy; 
	DWORD dwFVISize = GetFileVersionInfoSize( exepath , &dwDummy ); 
	LPBYTE lpVersionInfo = new BYTE[dwFVISize]; 
	GetFileVersionInfo( exepath , 0 , dwFVISize , lpVersionInfo ); 
	UINT uLen; 
	VS_FIXEDFILEINFO *lpFfi; 
	VerQueryValue( lpVersionInfo , _T("\\") , (LPVOID *)&lpFfi , &uLen ); 
	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS; 
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS; 
	delete [] lpVersionInfo; 
	printf( "Higher: %x\n" , dwFileVersionMS ); 
	printf( "Lower: %x\n" , dwFileVersionLS ); 
	DWORD dwLeftMost = HIWORD(dwFileVersionMS); 
	DWORD dwSecondLeft = LOWORD(dwFileVersionMS); 
	DWORD dwSecondRight = HIWORD(dwFileVersionLS); 
	DWORD dwRightMost = LOWORD(dwFileVersionLS); 


	is_skype61ornew=true;

	wsprintf( dest ,  L"Version: %d.%d.%d.%d\n" , dwLeftMost, dwSecondLeft, 
	dwSecondRight, dwRightMost ); 

	//::MessageBox(0,dest,dest,16);


	is_skype61ornew =false;

	if (dwLeftMost >= 6)
	{
	  if (dwSecondLeft >= 1)
	  {
		 is_skype61ornew =true;
	  }
    }
} 










#include <AudioClient.h>
#include <MMDeviceAPI.h>
#include <AudioPolicy.h>

IAudioClient *_AudioClient;
IAudioRenderClient *_RenderClient;
IAudioCaptureClient *_CaptureClient;

DWORD theRealFunction_WASAPIGetBufferMic;
DWORD theRealFunction_WASAPIGetBufferSpeaker;
DWORD theRealFunction_WASAPIReleaseBufferSpeaker;

DWORD theRealFunction_WASAPIGetMixFormat;




void setPatchMic( DWORD*  arrayDWORDFunctionBase);
void setPatchSpeaker( DWORD*  arrayDWORDFunctionBase);
void setPatchSpeaker( DWORD*  arrayDWORDFunctionBase);
void setPatchMix ( DWORD*  arrayDWORDFunctionBase);


 


struct fifo_node 
{
  struct fifo_node *next;
  BYTE   *value;
};

	fifo_node *front;
	fifo_node *back;


  fifo_node *dequeue(void)
  {



	fifo_node *tmp = front;
    if ( front != NULL )
      front = front->next;
    else
      back = NULL;

		//char salida[512];
		//ZeroMemory(salida,sizeof(salida));
		//sprintf(salida,"removed %d",tmp->value);
 		//OutputDebugStringA(salida);

	
	return tmp;
  }


  void queue(BYTE ** value)
  {
    
//		char salida[512];
//		ZeroMemory(salida,sizeof(salida));
//		sprintf(salida,"inserted %d",*value);
//		OutputDebugStringA(salida);

	  
	fifo_node *tempNode = (fifo_node*)VirtualAlloc(NULL,sizeof(fifo_node), MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
    tempNode->value = *value;
    if ( front == NULL )
    {
      front = tempNode;
      back = tempNode;
    }
    else
    {
      back->next = tempNode;
      back = tempNode;
    }
  }





void SetIsSkype(bool ais_skype)
{
	is_skype=ais_skype;

}

void SetIsMSN(bool ais_MSN)
{
	is_MSN=ais_MSN;
}

void SetOlympic(bool ais_ol)
{
	is_oly=ais_ol;
}

void SetisOther(bool ais_ol)
{
	is_other=ais_ol;
};



int GetPosicionInArray(void)
{
	return posicion_arreglo;
}





WAVEHDR G_mywavehdr;
WAVEFORMATEX G_WAVEFORMATEX;
LONG volatile inconversation = 0;
HWAVEIN default_wavein;
LPVOID     Original_Function_Pointer;
LPWAVEHDR anterior;
LPWAVEHDR anterior_in;
HWND MessageHWND;
bool FirstTime;CListA* MyListaPlay;
LONG volatile  Patched;
LONG volatile  RecordingMic;
LONG volatile  RecordingNow;
HANDLE ThreadHandleWaveIn;
CListA* MyListaRec=NULL;
CMergedList* MyMergedLists; 
bool isevent = false;
bool isthread= false;
HANDLE findwindowthread;


int ___in_channel=0;
int ___in_hz=0;
int ___in_pos=0;


int ___out_channel=0;
int ___out_hz=0;
int ___out_pos=0;

int ___bus_hz=0;

void SetHWND(HWND ahwnd)
{
	MessageHWND = ahwnd;
}


DWORD WINAPI SearchWindow(LPVOID arg)
{
	while (1)
	{
		HWND awindow = FindWindow(MRecorder, MRecorder);
		if (awindow != NULL)
		{
			MessageHWND =awindow;
			if (MyMergedLists != NULL)
			{
				MyMergedLists->MessageHWND =MessageHWND; 
			}

		};
		Sleep(5000);
	};
};





HMODULE mymodule;




void Initialize(void)
{
	ZeroMemory(&G_mywavehdr,sizeof(WAVEHDR));
	Patched   = 0;
	inconversation = 0;
	InterlockedExchange(&RecordingMic,0);
	InterlockedExchange(&RecordingNow,0);
	MyListaPlay = NULL;
	MyListaRec = NULL;
}




void ContinueAll(void)
{
	InterlockedExchange(&RecordingMic,1);
	InterlockedExchange(&RecordingNow,1);
}


void PauseAll(void)
{
	InterlockedExchange(&RecordingMic,0);
	InterlockedExchange(&RecordingNow,0);
}


void GetFileName(WCHAR* afilenameDest,WCHAR*afilenameSource)
{
	int tamano;
	tamano = wcslen(afilenameSource);
	int inicio =0;

	for (int i= tamano-1;i>0;i--)
	{
		if (afilenameSource[i]==L'\\'&& (inicio==0))
		{
			inicio =i+1;
			break;
		}
	}
	wcscat(afilenameDest,&afilenameSource[inicio]);
}


void StopAll(void)
{
	if (MyListaPlay !=NULL)
	{
		InterlockedExchange(&RecordingNow,0);
		InterlockedExchange(&RecordingMic,0);

		MyMergedLists->Stop();

		MyMergedLists->ProcessFiles();
	
		HANDLE myfileRead1 = CreateFile(MyMergedLists->FileNameFullPath, GENERIC_READ , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
		if (myfileRead1 != INVALID_HANDLE_VALUE)
		{
			DWORD filesizeHightBytes;
			DWORD  filesizeLowBytes1 = GetFileSize(myfileRead1,&filesizeHightBytes);
			CloseHandle(myfileRead1);

			if (filesizeLowBytes1 > 8192)
			{
				::MoveFile(MyMergedLists->FileNameFullPath,MyMergedLists->RealDestination);

				ZeroMemory(&arecord,sizeof(arecord));
				wcscat(arecord.FileName,MyMergedLists->RealDestination);
				FILETIME myFiletime;
				SYSTEMTIME atime;
				GetLocalTime(&atime);

				SystemTimeToFileTime(&atime,&myFiletime);
				arecord.datetime.dwHighDateTime = myFiletime.dwHighDateTime;
				arecord.datetime.dwLowDateTime = myFiletime.dwLowDateTime;
				WCHAR fil[512];
				ZeroMemory(fil,1024);
				GetModuleFileName(NULL,fil,1024);
				GetFileName(arecord.Source,fil);
				aplaylist->Insert_Record(&arecord);
				HWND awindow;

				if (ReadRegistryShowPlaylist())
				{
					awindow = FindWindow(MRecorder, NULL);
					SendMessage(awindow,MSG_SHOWDASHBOARD ,0,0);
				};
				awindow = FindWindow(NULL, MRecorder L" DashBoard");
				SendMessage(awindow,MSG_REFRESHPLAYLIST ,0,0);

			}
			else
			{
				DeleteFile(MyMergedLists->FileNameFullPath);
			}
		}
		delete MyListaPlay;
		MyListaPlay = NULL;
		
		delete MyListaRec;
		MyListaRec = NULL;
		TerminateThread(findwindowthread,0);

		InterlockedExchange(&good1,0);


	};
}

DWORD __stdcall  Terminaratiempo(LPVOID alp)
{
	if (isRegistered()==false)
	{
		Sleep(60000);
		StopAll();
	};
	return 0;
}



void BeginAll(WCHAR *filenam)
{
	// obtener los datos
	InterlockedExchange(&good1,0);

	ReadWASAPIValues(&___in_hz, &___in_channel, &___in_pos , &___out_hz , &___out_channel, &___out_pos ,&___bus_hz);

	DWORD id;
	findwindowthread = CreateThread(NULL,NULL,SearchWindow,NULL,false,&id);
	FirstTime = true;
	anterior = NULL;
	WCHAR temporal[512];
	CreateThread(NULL,NULL,Terminaratiempo,NULL,false,&id);
	TIPO_DESTINO tipo;
	ZeroMemory(&aoptionsRecording,sizeof(aoptionsRecording));
	ReadRegistryRecordingOptions(&aoptionsRecording);
	//tipo= TIPO_WAV ;
	tipo= (TIPO_DESTINO)aoptionsRecording.Format;
	MyMergedLists = new CMergedList(filenam,tipo);

	ZeroMemory(temporal,sizeof(temporal));
	wcscat(temporal,filenam);
	wcscat(temporal,L".speaker");

	
	DWORD destino=0;
	int bitrate;

	/*
	if (is_MSN==false)
	{
		if (is_oly)
		{
			G_WAVEFORMATEX.cbSize = sizeof(G_WAVEFORMATEX);
			G_WAVEFORMATEX.nChannels=1;
			G_WAVEFORMATEX.nSamplesPerSec=8000;
			G_WAVEFORMATEX.wBitsPerSample=16;
			GetSourceDestinationSample(G_WAVEFORMATEX.nSamplesPerSec,&destino,&bitrate);
		}
		else
		{

			GetSourceDestinationSample(G_WAVEFORMATEX.nSamplesPerSec,&destino,&bitrate);
		}
	}
	else
	{
		// esto es msn.
		G_WAVEFORMATEX.cbSize = sizeof(G_WAVEFORMATEX);
		G_WAVEFORMATEX.nChannels=1;
		G_WAVEFORMATEX.nSamplesPerSec=16000;
		G_WAVEFORMATEX.wBitsPerSample=16;
		GetSourceDestinationSample(G_WAVEFORMATEX.nSamplesPerSec,&destino,&bitrate);
	}
	*/
	// obtener el mayor entre los 2.
	// dado el mayor

	GetSourceDestinationSample(G_WAVEFORMATEX.nSamplesPerSec,&destino,&bitrate);
	MyMergedLists->bitrate = bitrate;

	MyListaPlay = new CListA(temporal,___out_hz);
	MyMergedLists->Wave = MyListaPlay;
	ZeroMemory(temporal,sizeof(temporal));
	wcscat(temporal,filenam);
	wcscat(temporal,L".Mic");

	MyListaRec  = new CListA(temporal,___in_hz);

	MyMergedLists->Mic = MyListaRec; 
	ZeroMemory(&arecord,sizeof(arecord));
	
	MyMergedLists->HZ=___bus_hz;
	MyMergedLists->GenerateNames();
	MyMergedLists->Start();
	InterlockedExchange(&RecordingMic,1);
	InterlockedExchange(&RecordingNow,1);
}














void teststatus()
{
	LONGLONG atime   = GetTickCount64();
	LONGLONG saved   = last_timeworked_in;
	LONGLONG diferen = atime - saved;
	
	if (diferen>2000) 
	{
		//detectar el stop
		if(inconversation==1)
		{
			InterlockedExchange(&inconversation,0);
			HWND awindow = FindWindow(MRecorderService, NULL);
			SendMessage(awindow,MSG_MICEND,0,0);
			awindow = FindWindow(MRecorder, MRecorder);
			if (awindow ==NULL)
			{
				if (RecordingMic==1)
				{
					if (ReadRegistryAutorecord())
					{
						awindow = FindWindow(MRecorderService, NULL);
						SendMessage(awindow,MSG_REC_STOP,posicion_arreglo,0);
					}
				}
			}
		}
	}
	else
	{
		if(inconversation==0)
		{
			InterlockedExchange(&inconversation,1);
			HWND awindow = FindWindow(MRecorderService, NULL);
			SendMessage(awindow,MSG_MICBEGIN,posicion_arreglo,0);
		}
	}
		
	
}





DWORD WINAPI checkstartstop(LPVOID theobj)
{
	int salida;
	while(1)
	{
		teststatus();			
		Sleep(400);
	}
	return 0;
}



void PatchMemory(int idinarray)
{
	if (Patched ==0)	
	{

		Patched =1;
		checkVersion();

		char salida[512];
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Patched");

		posicion_arreglo = 	idinarray;

		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Patched is skype");
		//OutputDebugStringA(salida);
	
		getPosWSAPIout();
		getPosWSAPIin();
		DWORD theid=0;
		::CreateThread(NULL,NULL,checkstartstop,NULL,0,&theid);
		Patched =1;
		aplaylist = new CPlaylist();
	}
};


void UnpatchMemory()
{
	char salida[512];
	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","UNPatched");
	//OutputDebugStringA(salida);
	// todo aca hay que fijarse como despatchear.

};






HRESULT STDMETHODCALLTYPE WASAPIGetBufferMic( 
			DWORD       pDSB,
            BYTE **ppData,
            UINT32 *pNumFramesToRead,
            DWORD *pdwFlags,
            UINT64 *pu64DevicePosition,
            UINT64 *pu64QPCPosition)
{
			LONGLONG atime = ::GetTickCount64();
			::InterlockedExchange64(&last_timeworked_in,atime);
			char salidas[512];
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
			
			

			if (is_skype)
			{
				ZeroMemory(salidas,sizeof(salidas));
				sprintf(salidas,"SKYPE WASAPIGetBufferMic  ppData %x,  pNumFramesToRead %d ___in_channel= %d\n" ,*ppData,*pNumFramesToRead,___in_channel);
				//OutputDebugStringA(salidas);
				ITEMLIST* PItemList; 
				if (RecordingNow ==1)	
				{
					if (*pNumFramesToRead!=0)
					{
						if (MyListaRec != NULL) 
						{
							
							PItemList = (ITEMLIST*)  VirtualAlloc(NULL,sizeof(ITEMLIST), MEM_COMMIT,PAGE_READWRITE);	
							PItemList->UnDato.Buffer  = VirtualAlloc(NULL,(*pNumFramesToRead)*2, MEM_COMMIT,PAGE_READWRITE);
							PItemList->UnDato.BytesUsed = (*pNumFramesToRead)*2;
							for (int i = 0 ; i<(*pNumFramesToRead); i++)
							{
								((unsigned short*)PItemList->UnDato.Buffer)[i] = (((unsigned short*)*ppData)[i*___in_channel]) ; 			

							}	
							MyListaRec->AddItem(PItemList);
						}
					}
				}
			}
			else
			{
				//ZeroMemory(salidas,sizeof(salidas));
				//sprintf(salidas,"NO WASAPIGetBufferMic  ppData %x,  pNumFramesToRead %d \n" ,*ppData,*pNumFramesToRead);
				////OutputDebugStringA(salidas);
				ITEMLIST* PItemList; 
				if (RecordingNow ==1)	
				{
					if (*pNumFramesToRead!=0)
					{
						if (MyListaRec != NULL) 
						{

							PItemList = (ITEMLIST*)  VirtualAlloc(NULL,sizeof(ITEMLIST), MEM_COMMIT,PAGE_READWRITE);	
							PItemList->UnDato.Buffer  = VirtualAlloc(NULL,(*pNumFramesToRead)*2, MEM_COMMIT,PAGE_READWRITE);
							PItemList->UnDato.BytesUsed = (*pNumFramesToRead)*2;
//							::ZeroMemory(PItemList->UnDato.Buffer,PItemList->UnDato.BytesUsed);
							for (int i = 0 ; i<(*pNumFramesToRead); i++)
							{
								((short*)PItemList->UnDato.Buffer)[i] = (short )((((float*)*ppData)[i*___in_channel]) * 32767) ; 			
								//((short*)PItemList->UnDato.Buffer)[i] = (short )((((float*)*ppData)[i]) * 32767) ; 			
							}	
							MyListaRec->AddItem(PItemList);
						}
					}
				}
			}
	return salida;
}







BYTE * removePuntero()
{
	fifo_node  *anode = dequeue();
	BYTE * salida =  anode->value;

	VirtualFree(anode,sizeof(fifo_node),MEM_DECOMMIT);
	return salida;
}

//DWORD good2=0;



void setgood(DWORD pDSB)
{
	 
	InterlockedExchange(&good1,pDSB);

}

bool isgood(DWORD pDSB)
{
	if (good1==0)
	{
		return true;
	}

	if (pDSB==good1)
	{
		return true;
	}
	else
	{
		return false;
	}

}



HRESULT STDMETHODCALLTYPE WASAPIGetBufferOut(
				DWORD       pDSB,
				UINT32 NumFramesRequested,
				BYTE **ppData)
{
		char salidas[512];

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
		//BYTE * puntero;
		//puntero = *ppData;
	
		if (is_skype)
		{
		
			if (isgood( pDSB))
			{
				queue(ppData);
				ZeroMemory(salidas,sizeof(salidas));
				sprintf(salidas,"GOOD WASAPIGetBufferOut  pDSB %d ,  ppData %x ,  pNumFramesToRead %d salida=%d \n" ,pDSB,  *ppData,NumFramesRequested,salida);
				OutputDebugStringA(salidas);
			}
			else
			{
				ZeroMemory(salidas,sizeof(salidas));
				sprintf(salidas,"NO GOOD WASAPIGetBufferOut  pDSB %d ,  ppData %x ,  pNumFramesToRead %d salida=%d \n" ,pDSB,  *ppData,NumFramesRequested,salida);
				OutputDebugStringA(salidas);
			}
		}
		else
		{
				queue(ppData);
				ZeroMemory(salidas,sizeof(salidas));
				sprintf(salidas,"NO SKYPE GOOD WASAPIGetBufferOut  pDSB %d ,  ppData %x ,  pNumFramesToRead %d salida=%d \n" ,pDSB,  *ppData,NumFramesRequested,salida);
				OutputDebugStringA(salidas);

		}




	return salida;
}
     


HRESULT STDMETHODCALLTYPE WASAPIReleaseBufferOut( 
				DWORD       pDSB,
				UINT32 NumFramesWritten,
				DWORD dwFlags)
{
		char salidas[512];
		BYTE atemp[2048];
		ZeroMemory(atemp,sizeof(atemp));
		
		BYTE * puntero;
		
		if (isgood(pDSB))
		{
			puntero=removePuntero();
		}

		//ZeroMemory(salidas,sizeof(salidas));
		//sprintf(salidas,"SKYPE WASAPIReleaseBufferOut pDSB %d NumFramesWritten %d ___out_channel =%d dwFlags =%d \n" ,pDSB , NumFramesWritten,___out_channel,dwFlags);
		//OutputDebugStringA(salidas);


		if (is_skype)
		{
			if ((RecordingMic==1))
			{

				if (MyListaPlay != NULL) 
				{
					if(NumFramesWritten !=0 )
					{
						   if (puntero==NULL)
						   {
								OutputDebugStringA("SKYPE WASAPIReleaseBufferOut puntero==NULL");
						   }
						   else
						   {
							   
							   if (dwFlags==0)
							   {
								   if (isgood(pDSB))
								   {
									    unsigned short test=0;
										ITEMLIST* PItemList;
										PItemList = (ITEMLIST*)  VirtualAlloc(NULL,sizeof(ITEMLIST), MEM_COMMIT,PAGE_READWRITE);	
										PItemList->UnDato.Buffer  = VirtualAlloc(NULL,(NumFramesWritten)*2, MEM_COMMIT,PAGE_READWRITE);
										PItemList->UnDato.BytesUsed = (NumFramesWritten)*2;
										
										for (int i = 0 ; i<(NumFramesWritten); i++)
										{
											test = test + (((unsigned short*)puntero)[i*___out_channel]);  		
											((unsigned short*)PItemList->UnDato.Buffer)[i] = (((unsigned short*)puntero)[i*___out_channel]) ; 			
										}	
										MyListaPlay->AddItem(PItemList);
								
										ZeroMemory(salidas,sizeof(salidas));
										sprintf(salidas,"SKYPE WASAPIReleaseBufferOut pDSB %d NumFramesWritten %d ___out_channel =%d dwFlags =%d test=%d\n" ,pDSB , NumFramesWritten,___out_channel,dwFlags,test);
										OutputDebugStringA(salidas);

										if (test>0)
										{
											setgood(pDSB);
										}
										else
										{
											count000++;
											if (count000>5)
											{
												setgood(0);
											}
										}

									}
							   }
						   
						   }
					}
				};
			};
		}
		else
		{
//			ZeroMemory(salidas,sizeof(salidas));
//			sprintf(salidas,"NO WASAPIReleaseBufferOut  NumFramesWritten %d\n" ,NumFramesWritten);
//			//OutputDebugStringA(salidas);
			if ((RecordingMic==1))
			{
				if (MyListaPlay != NULL) 
				{
					if(NumFramesWritten !=0 )
					{
						ITEMLIST* PItemList;

						PItemList = (ITEMLIST*)  VirtualAlloc(NULL,sizeof(ITEMLIST), MEM_COMMIT,PAGE_READWRITE);	
						PItemList->UnDato.Buffer  = VirtualAlloc(NULL,(NumFramesWritten)*2, MEM_COMMIT,PAGE_READWRITE);
						PItemList->UnDato.BytesUsed = (NumFramesWritten)*2;
						for (int i = 0 ; i<(NumFramesWritten); i++)
						{
							((short*)PItemList->UnDato.Buffer)[i] = (short )((((float*)puntero)[i*___out_channel]) * 32767) ; 			
						}	
						MyListaPlay->AddItem(PItemList);
					}
				};
			};
		}
	

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




HRESULT STDMETHODCALLTYPE WASAPIGetMixFormat(
  	DWORD       pDSB,
	WAVEFORMATEX **ppDeviceFormat
)
{

		HRESULT salida;
		LPVOID  patcheado = (LPVOID)theRealFunction_WASAPIGetMixFormat;
		__asm                  
		{
			push ppDeviceFormat
			push        pDSB  
			call patcheado
			mov salida ,eax
		}

		char salidas[512];
		ZeroMemory(salidas,sizeof(salidas));
		sprintf(salidas,"%s\n","WASAPIGetMixFormat");
		//OutputDebugStringA(salidas);


		ZeroMemory(salidas,sizeof(salidas));
		sprintf(salidas,"channels %d\n",(**ppDeviceFormat).nChannels);
		//OutputDebugStringA(salidas);

		ZeroMemory(salidas,sizeof(salidas));
		sprintf(salidas,"samples per sec %d\n",(**ppDeviceFormat).nSamplesPerSec);
		//OutputDebugStringA(salidas);



	return salida;


}



bool getPosWSAPIout(void)
{
	char salida[1024];
	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","getPosWSAPIOut");
	//OutputDebugStringA(salida);

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
		//OutputDebugStringA(salida);

		return false;
    }

    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDeviceEnumerator *   _DeviceEnumerator;

	IMMDeviceCollection *deviceCollection = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
	        printf("Unable to instantiate device enumerator: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to instantiate device enumerator\n");
		//OutputDebugStringA(salida);

		return  false;
    }

    IMMDevice *device = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);

	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
        ERole deviceRole = eConsole;    // Assume we're using the console role.


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","GetDefaultAudioEndpoint");
	//OutputDebugStringA(salida);

		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, deviceRole, &device);
        if (FAILED(hr))
        {
            printf("Unable to get default device for role %d: %x\n", deviceRole, hr);
			ZeroMemory(salida,sizeof(salida));
			sprintf(salida,"%s","Unable to get default device for role");
			//OutputDebugStringA(salida);

			return false;
		}



	IMMDevice *Endpoint = device;

	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","Activate");
	//OutputDebugStringA(salida);

    hr = Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        printf("Unable to activate audio client: %x.\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to activate audio client");
		//OutputDebugStringA(salida);

		return false;
    }

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
        return false;
    }


	  hr = _AudioClient->GetMixFormat(&_MixFormat);
	

		ZeroMemory(salida,sizeof(salida));
        sprintf(salida,"getPosWSAPIOut _MixFormat->cbSize: %d\n", _MixFormat->cbSize);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"getPosWSAPIOut _MixFormat->nAvgBytesPerSec: %d\n", _MixFormat->nAvgBytesPerSec);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
        sprintf(salida,"getPosWSAPIOut _MixFormat->nBlockAlign: %d\n", _MixFormat->nBlockAlign);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
        sprintf(salida,"getPosWSAPIOut _MixFormat->nChannels: %d\n", _MixFormat->nChannels);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
        sprintf(salida,"getPosWSAPIOut _MixFormat->nSamplesPerSec: %d\n", _MixFormat->nSamplesPerSec);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
        sprintf(salida,"getPosWSAPIOut _MixFormat->wBitsPerSample: %d\n", _MixFormat->wBitsPerSample);
		//OutputDebugStringA(salida);





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


}

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




	
bool getPosWSAPIin(void)
{
	char salida[1024];
	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","getPosWSAPIIn");
	//OutputDebugStringA(salida);

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
		//OutputDebugStringA(salida);

		return false;
    }

    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDeviceEnumerator *   _DeviceEnumerator;

	IMMDeviceCollection *deviceCollection = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
	        printf("Unable to instantiate device enumerator: %x\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to instantiate device enumerator\n");
		//OutputDebugStringA(salida);

		return  false;
    }

    IMMDevice *device = NULL;


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","CoCreateInstance");
	//OutputDebugStringA(salida);


	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
        ERole deviceRole = eConsole;    // Assume we're using the console role.


	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","GetDefaultAudioEndpoint");
	//OutputDebugStringA(salida);

		hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, deviceRole, &device);
        if (FAILED(hr))
        {
            printf("Unable to get default device for role %d: %x\n", deviceRole, hr);
			ZeroMemory(salida,sizeof(salida));
			sprintf(salida,"%s","Unable to get default device for role");
			//OutputDebugStringA(salida);

			return false;
		}



	IMMDevice *Endpoint = device;

	ZeroMemory(salida,sizeof(salida));
	sprintf(salida,"%s","Activate");
	//OutputDebugStringA(salida);

    hr = Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        printf("Unable to activate audio client: %x.\n", hr);
		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"%s","Unable to activate audio client");
		//OutputDebugStringA(salida);

		return false;
    }

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
        return false;
    }


	  hr = _AudioClient->GetMixFormat(&_MixFormat);


		ZeroMemory(salida,sizeof(salida));
	    sprintf(salida,"getPosWSAPIIn _MixFormat->cbSize: %d\n", _MixFormat->cbSize);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"getPosWSAPIIn _MixFormat->nAvgBytesPerSec: %d\n", _MixFormat->nAvgBytesPerSec);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"getPosWSAPIIn _MixFormat->nBlockAlign: %d\n", _MixFormat->nBlockAlign);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"getPosWSAPIIn _MixFormat->nChannels: %d\n", _MixFormat->nChannels);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
		sprintf(salida,"getPosWSAPIIn _MixFormat->nSamplesPerSec: %d\n", _MixFormat->nSamplesPerSec);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
        sprintf(salida,"getPosWSAPIIn _MixFormat->wBitsPerSample: %d\n", _MixFormat->wBitsPerSample);
		//OutputDebugStringA(salida);

		ZeroMemory(salida,sizeof(salida));
        sprintf(salida,"getPosWSAPIIn _MixFormat->wFormatTag: %d\n", _MixFormat->wFormatTag);
		//OutputDebugStringA(salida);

		

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

    hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
    if (FAILED(hr))
    {
        printf("Unable to get new capture client: %x.\n", hr);
        return false;
    }

	setPatchMic((DWORD*)_CaptureClient);
	//setPatchMix((DWORD*)_AudioClient);


}




	// obtencion de la posicion de memoria a patchear WAVE OUT
void setPatchMic( DWORD*  arrayDWORDFunctionBase)
{
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


void setPatchMix ( DWORD*  arrayDWORDFunctionBase)
{
	DWORD* FIRST_VIRTUAL_TABLE;
	DWORD* MY_PATCH_FUNCTION ;
	DWORD intable;
	DWORD* aux;
	DWORD dwProtect_antes;
	
	intable=8;
	FIRST_VIRTUAL_TABLE  = arrayDWORDFunctionBase;
	aux 	= (DWORD*)((DWORD*) ((*FIRST_VIRTUAL_TABLE)+ (intable * 4)));
	theRealFunction_WASAPIGetMixFormat = * aux;
	VirtualProtect((LPVOID)aux, 4, PAGE_EXECUTE_READWRITE, &dwProtect_antes);
	* aux = (DWORD) &WASAPIGetMixFormat;

}


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


				/*
					if(NumFramesWritten !=0 )
					{
						ITEMLIST* PItemList;
						PItemList = (ITEMLIST*)  VirtualAlloc(NULL,sizeof(ITEMLIST), MEM_COMMIT,PAGE_READWRITE);	
	
						PItemList->UnDato.Buffer  = VirtualAlloc(NULL,NumFramesWritten*2, MEM_COMMIT,PAGE_READWRITE);
						PItemList->UnDato.BytesUsed = NumFramesWritten*2;
						for (int i = 0 ; i<(NumFramesWritten); i++)
						{
							((unsigned short*)PItemList->UnDato.Buffer)[i] = (((unsigned short*)puntero)[(i*4)+1]) ; 			
						}	
						MyListaPlay->AddItem(PItemList);
					}
				*/
