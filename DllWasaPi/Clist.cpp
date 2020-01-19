#include "stdafx.h"	
#include "Clist.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "CMergeFile.h"
#include "Wav.h"
#include "encoder_example.h"
#include "../Interface/helper.h"

#pragma comment ( lib, "winmm.lib") 


typedef struct _THECHUNKDATA
{
	  DWORD	BYTESTOREAD;
	  DWORD	mytick;
}THECHUNKDATA;

#define MSG_SIZEKB   WM_USER + 9050

COPYDATASTRUCT acopy;
TPROCESSLIST aprocessList;


DWORD GetSize(WCHAR* filename)
{
	DWORD filesizeHightBytes;
	HANDLE myfileRead1 = CreateFile(filename, GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	if (myfileRead1 != INVALID_HANDLE_VALUE)
	{
		DWORD  filesizeLowBytes1 = GetFileSize(myfileRead1,&filesizeHightBytes);
		CloseHandle(myfileRead1);
		return filesizeLowBytes1;
	}
	else
	{
		return 0;
	}

}


DWORD WINAPI FlushListas(LPVOID lpParameter )
{
	DWORD tiempo_espera= 8; // en centesimas
	HANDLE terminaron[2];
	CMergedList* MergedList;
	MergedList = (CMergedList*)lpParameter; 
	terminaron[0]=MergedList->Mic->MyeventFin;
	terminaron[1]=MergedList->Wave->MyeventFin;
	DWORD tamano=0;
	bool firsttime=true; 
	bool CreatedHeader=false;
	while (MergedList->running)
	{
		Sleep(tiempo_espera*100);
		ResetEvent(MergedList->Mic->MyeventFin);
		ResetEvent(MergedList->Wave->MyeventFin);
		SetEvent(MergedList->Mic->MyeventFlush);
		SetEvent(MergedList->Wave->MyeventFlush);
		WaitForMultipleObjects(2,terminaron,true,INFINITE);
		MergedList->MergeFilesSynchro();
		// aca pasaron 4 segundos.
		// si se cuantos bytes tiene el archivo
		// se lo hz en que esta grabado.

		if (MergedList->tipo==0)
		{
			if (firsttime)
			{
				// aproximado
				MergedList->AppendFilesWav();
				firsttime=false;
			}
			else
			{
				MergedList->AppendFilesWav();
			};
			tamano = GetSize(MergedList->FileNameMergedDestWav);
		}
		else
		{
			if (firsttime)
			{
				// aca pasaron 4 segundos.
				// si se cuantos bytes tiene el archivo
				// se lo hz en que esta grabado.
				//MergedList->HZ = MergedList->GetSize(MergedList->FileNameMerged)/tiempo_espera/2/2;
				//MergedList->HZ=48000;
				CreateHeader(MergedList->HZ, MergedList->bitrate,MergedList->FileNameFullPath);
				CreatedHeader=true;
				firsttime=false;
				// ver lo que hacer con los khz
			}
			ProcessData(MergedList->FileNameMerged, MergedList->FileNameFullPath);
			tamano = GetSize(MergedList->FileNameFullPath);
		}
		SendMessage(MergedList->MessageHWND,MSG_SIZEKB,tamano,0);

		DeleteFile(MergedList->Mic->FileName);
		DeleteFile(MergedList->Wave->FileName);
		// unir los 2 archivos. y agregar ceros si corresponde.
		// borrar los 2 archivos.
		//encodear si corresponde.
	}

	ResetEvent(MergedList->Mic->MyeventFin);
	ResetEvent(MergedList->Wave->MyeventFin);
	MergedList->Mic->SetFlush();
	Sleep(10);
	MergedList->Wave->SetFlush();
	Sleep(10);
	WaitForMultipleObjects(2,terminaron,true,4000);
	MergedList->MergeFilesSynchro();
		if (MergedList->tipo==0)
		{
			MergedList->AppendFilesWav();
		}
		else
		{
			if 	(CreatedHeader)
			{
				ProcessData(MergedList->FileNameMerged, MergedList->FileNameFullPath);
			};	
				//ogg
		}
	DeleteFile(MergedList->Mic->FileName);
	DeleteFile(MergedList->Wave->FileName);
	return(0);
};




void Shrink(LPVOID lpParameter,DWORD sampledest,DWORD samplebaseSource)
{
	div_t div_result;
	ITEMLIST* PItemList = (ITEMLIST*) lpParameter;
	DWORD indice;

	unsigned char * charcharBuff1 = (unsigned char *) PItemList->UnDato.Buffer;
	if (samplebaseSource != 48000)
	{
		indice = 1;
	}
	else
	{	
		indice = (samplebaseSource/sampledest);
	}
	if (PItemList->UnDato.BytesUsed != 0)
	{
		for (int i= 0 ; i <=  PItemList->UnDato.BytesUsed -1 ; i++)
		{
			div_result = div( i, (indice*2) );
			if ( div_result.rem == 0) 
			{
				charcharBuff1[i/indice]   =  charcharBuff1[i];
				charcharBuff1[i/indice+1] =  charcharBuff1[i+1];
			};
		}
	};
	PItemList->UnDato.BytesComress =  PItemList->UnDato.BytesUsed / indice;
}



DWORD WINAPI FlushListA(LPVOID lpParameter )
{
	int i;
	CListA* AListA;
	AListA = (CListA*)lpParameter; 
	HANDLE myFile;
	DWORD BytesWriten;
	ITEMLIST*	PItemList;
	THECHUNKDATA mychunk;
	while (AListA->Running)
	{
		WaitForSingleObject(AListA->MyeventFlush,INFINITE);
    		EnterCriticalSection(&AListA->MyMutex);		
				ResetEvent(AListA->MyeventFlush);
				if  (AListA->mylistaADD == AListA->mylista1)
				{
					AListA->mylistaADD   = AListA->mylista2;
					AListA->mylistaFlush = AListA->mylista1;
				}
				else
				{
					AListA->mylistaADD   = AListA->mylista1;
					AListA->mylistaFlush = AListA->mylista2;
				}
				AListA->mylistaADD->cantidadElementos = 0;
			LeaveCriticalSection(&AListA->MyMutex);
			
			PItemList = AListA->Removeitem();
			if (PItemList != NULL)
			{ 
				myFile = CreateFile(AListA->FileName,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
				SetFilePointer(myFile,0,NULL,FILE_END);
				while (PItemList != NULL)	
				{
					
					//Shrink(PItemList,AListA->mysampledest,AListA->mysampleBaseSource);
					PItemList->UnDato.BytesComress =  PItemList->UnDato.BytesUsed;
					WriteFile(myFile, PItemList->UnDato.Buffer,PItemList->UnDato.BytesComress,&BytesWriten,NULL);
					VirtualFree(PItemList->UnDato.Buffer,PItemList->UnDato.BytesUsed,MEM_DECOMMIT);
					VirtualFree(PItemList->UnDato.Buffer,0,MEM_RELEASE);
					VirtualFree(PItemList,sizeof(ITEMLIST),MEM_DECOMMIT);
					VirtualFree(PItemList,0,MEM_RELEASE);
					PItemList = AListA->Removeitem();
				}
				CloseHandle(myFile);
			}
			SetEvent(AListA->MyeventFin);

		}
		SetEvent(AListA->MyeventFin);
		return(0);
};



CListA::CListA(WCHAR * aFileName, DWORD asamplebase)
{
	ZeroMemory(this->FileName,sizeof(this->FileName));
	wcscat(this->FileName,aFileName);
	this->HZ =asamplebase;
	// borro el archivo por si existe
	HANDLE myfileWrite = CreateFile(aFileName , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	CloseHandle(myfileWrite);

	Running = true;
	MyeventFlush = CreateEvent(NULL,false,false,NULL);
	ResetEvent(MyeventFlush);
	
	MyeventFin = CreateEvent(NULL,false,false,NULL);
	ResetEvent(MyeventFin);

	MytrheadFlush = CreateThread(NULL,0,FlushListA,(LPVOID)this,CREATE_SUSPENDED,NULL);
	InitializeCriticalSection(&MyMutex);
	mylista1 = (LISTA*)VirtualAlloc(NULL,sizeof(LISTA), MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	mylista2 = (LISTA*)VirtualAlloc(NULL,sizeof(LISTA), MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	mylista1->cantidadElementos =0;
	mylista1->Header=NULL;
	mylista1->Tail =NULL;
		
	mylista2->cantidadElementos =0;
	mylista2->Header=NULL;
	mylista2->Tail =NULL;

	mylistaADD = mylista1;
	ResumeThread(MytrheadFlush);
}


/*
CListA::CListA(WCHAR * aFileName,DWORD asamplebase,DWORD asampledest,WCHAR* nombre_mutex,bool send)
{
	ZeroMemory(this->FileName,sizeof(this->FileName));
	wcscat(this->FileName,aFileName);
	this->mysampledest = asampledest;
	this->mysampleBaseSource = asamplebase; 
	// borro el archivo por si existe
	HANDLE myfileWrite = CreateFile(aFileName , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	CloseHandle(myfileWrite);

	WCHAR MyeventFlushNAME[512];
	ZeroMemory(MyeventFlushNAME,sizeof(MyeventFlushNAME));
	wcscat(MyeventFlushNAME,nombre_mutex);
	wcscat(MyeventFlushNAME,L"Flush");


	Running = true;
	if (send)
	{
		MyeventFlush = CreateEvent(NULL,false,false,MyeventFlushNAME);
	}
	else
	{
		MyeventFlush = OpenEvent(EVENT_MODIFY_STATE,false,MyeventFlushNAME);
		
	};

	ResetEvent(MyeventFlush);
	
	
	WCHAR MyeventFinNAME[512];
	ZeroMemory(MyeventFinNAME,sizeof(MyeventFinNAME));
	wcscat(MyeventFinNAME,nombre_mutex);
	wcscat(MyeventFinNAME,L"Fin");
	
	if (send)
	{
		MyeventFin = CreateEvent(NULL,false,false,MyeventFinNAME);
		MytrheadFlush=NULL;
	}
	else
	{
		MyeventFin = OpenEvent(EVENT_MODIFY_STATE,false,MyeventFinNAME);
		MytrheadFlush = CreateThread(NULL,0,FlushListA,(LPVOID)this,CREATE_SUSPENDED,NULL);
	
	};
	ResetEvent(MyeventFin);

	InitializeCriticalSection(&MyMutex);
	mylista1 = (LISTA*)VirtualAlloc(NULL,sizeof(LISTA), MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	mylista2 = (LISTA*)VirtualAlloc(NULL,sizeof(LISTA), MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	mylista1->cantidadElementos =0;
	mylista1->Header=NULL;
	mylista1->Tail =NULL;
		
	mylista2->cantidadElementos =0;
	mylista2->Header=NULL;
	mylista2->Tail =NULL;

	mylistaADD = mylista1;
	
	ZeroMemory(&aprocessList,sizeof(TPROCESSLIST));
	wcscat(aprocessList.NombreEvent,nombre_mutex);
	wcscat(aprocessList.FileName,aFileName);

	aprocessList.SampleDest = asampledest;
	aprocessList.SampleSource =asamplebase;

	ZeroMemory(&acopy,sizeof(acopy));
	acopy.cbData = sizeof(aprocessList);
	acopy.lpData = &aprocessList;
	acopy.dwData = 1;

	if (send)
	{
		HWND awindow;
		awindow = FindWindow(MRecorder,MRecorder);
		if (awindow != NULL)
		{
			SendMessage(awindow,WM_COPYDATA,0,(LPARAM)&acopy);
		};
	};
}
*/



void CListA::SetFlush(void)
{
	Running= false;
	SetEvent(MyeventFlush);
	if (MytrheadFlush !=NULL)
	{
		WaitForSingleObject(MytrheadFlush,INFINITE);
		CloseHandle(MytrheadFlush);
	}
	CloseHandle(MyeventFlush);
	DeleteCriticalSection(&MyMutex);
}


CListA::~CListA()
{

	VirtualFree(mylista1,sizeof(LISTA),MEM_DECOMMIT);
	VirtualFree(mylista1,0,MEM_RELEASE);

	VirtualFree(mylista2,sizeof(LISTA),MEM_DECOMMIT);
	VirtualFree(mylista2,0,MEM_RELEASE);
};

void CListA::AddItem(ITEMLIST* PItemList) 
{
	PItemList->UnDato.BytesComress =0;
//	PItemList->UnDato.timeSyncro= GetTickCount();
	EnterCriticalSection(&MyMutex);		
		if (mylistaADD->cantidadElementos++ == 1000)
		{
			//SetEvent(Myevent);
		}
		PItemList->NextItem = NULL;
		if (mylistaADD->Header == NULL )
		{
			mylistaADD->Header = PItemList;
			mylistaADD->Tail   = PItemList;
		}
		else
		{
	 		mylistaADD->Tail->NextItem = PItemList;
			mylistaADD->Tail = PItemList;
		}
	LeaveCriticalSection(&MyMutex);
}

ITEMLIST* CListA::Removeitem()
{
	ITEMLIST* Salida = NULL;
	if (mylistaFlush->Header != NULL)
	{
		Salida =  mylistaFlush->Header;
		mylistaFlush->Header = (ITEMLIST*) mylistaFlush->Header->NextItem;
	}
	return 	Salida;
}

//CMergedList::CMergedList(WCHAR* afile)
CMergedList::CMergedList(WCHAR* afile, TIPO_DESTINO mytipo)

{
	this->tipo = mytipo;
	ZeroMemory(this->FileName,1024);
	wcscat(this->FileName,afile);
	ZeroMemory(this->FileNameNoPath,1024);
	GetOnlyName(this->FileNameNoPath);
};


void CMergedList::Start(void)
{
	DWORD id;
	running=true;
	ThreadSync = CreateThread(NULL,NULL,FlushListas,this,false,&id);
	// aca levantar el thread que lo que hace es hacer el setevent de cada uno de los list en un intervalo de 4 segundos
};

void CMergedList::Stop(void)
{
// aca levantar el thread que lo que hace es hacer el setevent de cada uno de los list en un intervalo de 4 segundos
	running=false;
	WaitForSingleObject(ThreadSync,INFINITE);

};

void CMergedList::GetOnlyName(WCHAR* aux)
{
	int tamano;
	tamano = wcslen(this->FileName);
	int inicio =0;
	for (int i= tamano-1;i>0;i--)
	{
		if (this->FileName[i]==L'\\'&& (inicio==0))
		{
			inicio =i+1;
			break;
		}
	}
	wcscat(aux,&this->FileName[inicio]);
}


CMergedList::~CMergedList()
{
};



DWORD WINAPI CompressIt(LPVOID lpParameter )
{
	CMergedList* amergedlisat;
	amergedlisat = (CMergedList*)lpParameter; 	
	while(1)
	{
		if (WAIT_OBJECT_0==WaitForSingleObject(amergedlisat->eventCompress,30000))
		{
		
			//amergedlisat->CompressNow()		
			break;
		}
		else
		{
			//amergedlisat->CompressNow()		
		}
	}
	return(0);
};


DWORD CMergedList::GetSize(WCHAR * filenames)
{
	DWORD filesizeHightBytes;
	HANDLE myfileRead1 = CreateFile(filenames, GENERIC_READ , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	DWORD  filesizeLowBytes1 = GetFileSize(myfileRead1,&filesizeHightBytes);
	return  filesizeLowBytes1;
}


DWORD CMergedList::AppendFilesWav(void)
{
	DWORD filesizeHightBytes;
	LPVOID mybuffer1   = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	HANDLE myfileRead1 = CreateFile(this->FileNameMerged, GENERIC_READ , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	HANDLE myfileWrite = CreateFile(this->FileNameMergedDestWav , GENERIC_WRITE , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL);
	if 	(myfileWrite == INVALID_HANDLE_VALUE)
	{
		myfileWrite = CreateFile(this->FileNameMergedDestWav , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	}
	SetFilePointer(myfileWrite,0,NULL,FILE_END);

	DWORD  filesizeLowBytes1 = GetFileSize(myfileRead1,&filesizeHightBytes);
	DWORD readedBytes1;
	DWORD readedBytes2;
	DWORD writenBytes;
	while ((ReadFile(myfileRead1,mybuffer1,16384,&readedBytes1,NULL) != 0) &&  (readedBytes1 !=0))
	{
			WriteFile(myfileWrite,mybuffer1,(readedBytes1),&writenBytes,NULL);
	}
	CloseHandle(myfileRead1);
	CloseHandle(myfileWrite);
	VirtualFree(mybuffer1,16384,MEM_DECOMMIT);
	VirtualFree(mybuffer1,0,MEM_RELEASE);
	return filesizeLowBytes1;
}


void CMergedList::MergeFilesSynchro(void)
{
	DWORD total_read;
	total_read =0;
	DWORD filesizeHightBytes;

	LPVOID mybuffer1   = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	LPVOID mybuffer2   = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	LPVOID mybufferOut = VirtualAlloc(NULL,32768, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	HANDLE myfileRead1 = CreateFile(this->Mic->FileName, GENERIC_READ , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	HANDLE myfileRead2 = CreateFile(this->Wave->FileName, GENERIC_READ , 0 ,	 NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	HANDLE myfileWrite = CreateFile(this->FileNameMerged , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	DWORD  filesizeLowBytes1 = GetFileSize(myfileRead1,&filesizeHightBytes);
	DWORD  filesizeLowBytes2 = GetFileSize(myfileRead2,&filesizeHightBytes);
	DWORD readedBytes1;
	DWORD readedBytes2;
	DWORD writenBytes;
	ZeroMemory(mybuffer1,16384);
	ZeroMemory(mybuffer2,16384);
	while ((ReadFile(myfileRead1,mybuffer1,16384,&readedBytes1,NULL) != 0) &&  (readedBytes1 !=0))
	{
			total_read = total_read + readedBytes1;
			ReadFile(myfileRead2,mybuffer2,16384,&readedBytes2,NULL);
			Merge(mybuffer1,mybuffer2,mybufferOut,readedBytes1);
			WriteFile(myfileWrite,mybufferOut,(readedBytes1)*2,&writenBytes,NULL);
			ZeroMemory(mybuffer1,16384);
			ZeroMemory(mybuffer2,16384);

	}
	CloseHandle(myfileRead1);
	CloseHandle(myfileRead2);
	CloseHandle(myfileWrite);
	

	total_read= 0;
	VirtualFree(mybuffer1,16384,MEM_DECOMMIT);
	VirtualFree(mybuffer1,0,MEM_RELEASE);
	VirtualFree(mybuffer2,16384,MEM_DECOMMIT);
	VirtualFree(mybuffer2,0,MEM_RELEASE);
	VirtualFree(mybufferOut,32768,MEM_DECOMMIT);
	VirtualFree(mybufferOut,0,MEM_RELEASE);


}

void CMergedList::MergeFiles(void)
{
	DWORD total_read;
	total_read =0;
	DWORD filesizeHightBytes;

	LPVOID mybuffer1   = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	LPVOID mybuffer2   = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	LPVOID mybufferOut = VirtualAlloc(NULL,32768, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	HANDLE myfileRead1 = CreateFile(this->Mic->FileName, GENERIC_READ , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	HANDLE myfileRead2 = CreateFile(this->Wave->FileName, GENERIC_READ , 0 ,	 NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	HANDLE myfileWrite = CreateFile(this->FileNameMerged , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	DWORD  filesizeLowBytes1 = GetFileSize(myfileRead1,&filesizeHightBytes);
	DWORD  filesizeLowBytes2 = GetFileSize(myfileRead2,&filesizeHightBytes);
	DWORD readedBytes1;
	DWORD readedBytes2;
	DWORD writenBytes;
	while ((ReadFile(myfileRead1,mybuffer1,16384,&readedBytes1,NULL) != 0) &&  (readedBytes1 !=0))
	{
			total_read = total_read + readedBytes1;
			ReadFile(myfileRead2,mybuffer2,16384,&readedBytes2,NULL);
			Merge(mybuffer1,mybuffer2,mybufferOut,readedBytes1);
			WriteFile(myfileWrite,mybufferOut,(readedBytes1)*2,&writenBytes,NULL);
	}
	CloseHandle(myfileRead1);
	CloseHandle(myfileRead2);
	CloseHandle(myfileWrite);
	

	total_read= 0;
	VirtualFree(mybuffer1,16384,MEM_DECOMMIT);
	VirtualFree(mybuffer1,0,MEM_RELEASE);
	VirtualFree(mybuffer2,16384,MEM_DECOMMIT);
	VirtualFree(mybuffer2,0,MEM_RELEASE);
	VirtualFree(mybufferOut,32768,MEM_DECOMMIT);
	VirtualFree(mybufferOut,0,MEM_RELEASE);
}


void CMergedList::GenerateNames(void)
{
	WCHAR* Extensiones[] = {L".wav",L".ogg",L".spx",L".mp3",L".wma"};

	wcscat(this->FileNameNoPath,Extensiones[this->tipo]);

	ZeroMemory(this->FileNameFullPath,1024);
	wcscat(this->FileNameFullPath,this->FileName);
	wcscat(this->FileNameFullPath,Extensiones[this->tipo]);

	ZeroMemory(this->FileNameMerged,1024);
	wcscat(this->FileNameMerged,this->FileName);
	wcscat(this->FileNameMerged,L".Merged");

	ZeroMemory(this->FileNameMergedDestWav,1024);
	wcscat(this->FileNameMergedDestWav,this->FileName);
	wcscat(this->FileNameMergedDestWav,L".Merged2");

	

	ZeroMemory(this->RealDestination,1024);
	ReadRegistryDestinationPath(this->RealDestination);
	wcscat(this->RealDestination,this->FileNameNoPath);

}

void CMergedList::ProcessFiles(void)
{

	switch (this->tipo)
	{
		case TIPO_WAV:
			//this->HZ= 48000;
			GenerateWaveFile(this->FileNameMergedDestWav,this->FileNameFullPath,this->HZ,2);
			DeleteFile(this->FileNameMergedDestWav);
			DeleteFile(this->FileNameMerged);

			break;
		case TIPO_OGG:
			EndOgg();
			DeleteFile(this->FileNameMerged);
			break;

		case TIPO_SPEEX:
			break;
		case TIPO_MP3:
			break;
		case TIPO_WMA:
			break;
	}


}



void CMergedList::EncodeFiles(void)
{
}



void CMergedList::MergeFiles2Mono(void)
{
}

