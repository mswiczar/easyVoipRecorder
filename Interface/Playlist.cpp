#include ".\playlist.h"
#include ".\reg.h"
#include "commctrl.h"
#include "helper.h"


CPlaylist::CPlaylist(void)
{
	GlobalMutexSearchpos = CreateMutex(NULL,false,MRecorderDBMutex);
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
		FileMapping=OpenFileMapping(FILE_MAP_WRITE|FILE_MAP_READ,true,MRecorderMapedFile);     
		if (FileMapping==NULL)
		{
			ZeroMemory(FileNameFile,sizeof(FileNameFile));
			ReadRegistryDBPath(FileNameFile);
			wcscat(FileNameFile,L"Database\\HistoryCalls.dat");
			File = CreateFile(FileNameFile , GENERIC_READ | GENERIC_WRITE , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL);
			if (INVALID_HANDLE_VALUE ==File)
			{
					this->InitFile();
				File = CreateFile(FileNameFile , GENERIC_READ | GENERIC_WRITE , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL);
			}
			FileMapping= CreateFileMapping(File,NULL,PAGE_READWRITE|SEC_COMMIT,0,0,MRecorderMapedFile);
		};
		PointerFileInMemory= MapViewOfFile(FileMapping,FILE_MAP_ALL_ACCESS,0,0,0);
	ReleaseMutex(GlobalMutexSearchpos);

}

CPlaylist::~CPlaylist(void)
{
	CloseHandle(PointerFileInMemory);
	CloseHandle(FileMapping);
	CloseHandle(File);
	CloseHandle(GlobalMutexSearchpos);

}


bool CPlaylist::Search_RecordByFileName(PRECORD_PLAYLIST arecord)
{
	bool salida=false;
	WCHAR number[256];
	ZeroMemory(number,sizeof(number));
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int cantidad_elementos=6000;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if (aglobalrecord[i].Status!=0)
		{
			if (lstrcmpi(aglobalrecord[i].FileName,arecord->FileName)==0)
			{
				memcpy(arecord,&aglobalrecord[i],sizeof(RECORD_PLAYLIST));
				salida=true;
				break;
			}
		}
		else
		{
			break;
		}


	}
	ReleaseMutex(GlobalMutexSearchpos);
	return salida;
}



bool CPlaylist::Search_RecordById(PRECORD_PLAYLIST arecord)
{
	bool salida=false;
	WCHAR number[256];
	ZeroMemory(number,sizeof(number));
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int cantidad_elementos=6000;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if (aglobalrecord[i].Id== arecord->Id)
		{
			memcpy(arecord,&aglobalrecord[i],sizeof(RECORD_PLAYLIST));
			salida=true;
			break;
		}


	}
	ReleaseMutex(GlobalMutexSearchpos);
	return salida;


}

void CPlaylist::Modify_Record(PRECORD_PLAYLIST arecord)
{
	WCHAR number[256];
	ZeroMemory(number,sizeof(number));
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int cantidad_elementos=6000;
	for (int i=0;i<cantidad_elementos;i++)

	{
		if (aglobalrecord[i].Id== arecord->Id)
		{
			memcpy(&aglobalrecord[i],arecord,sizeof(RECORD_PLAYLIST));
			break;
		}


	}
	ReleaseMutex(GlobalMutexSearchpos);

}

void CPlaylist::Delete_Record(PRECORD_PLAYLIST arecord)
{
	WCHAR number[256];
	ZeroMemory(number,sizeof(number));
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int cantidad_elementos=6000;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if (aglobalrecord[i].Id== arecord->Id)
		{
			aglobalrecord[i].Status = 2;
			break;
		}


	}
	ReleaseMutex(GlobalMutexSearchpos);
}


void CPlaylist::Insert_Record(PRECORD_PLAYLIST arecord)
{
	PRECORD_PLAYLIST aglobalrecord ;
	aglobalrecord = (PRECORD_PLAYLIST) this->FindFirstEmpty();
	if (aglobalrecord ==NULL)
	{
		// no se encontro lugar, entonces aumentar el tamano
		//SetMoreData();
	};
	aglobalrecord = (PRECORD_PLAYLIST) this->FindFirstEmpty();
	arecord->Id = aglobalrecord->Id;
	arecord->Status=1;
	memcpy(aglobalrecord,arecord,sizeof(RECORD_PLAYLIST));
	
	//aca hay que ver si callback
	//si hay callback, llamarlo para que se agregue en la lista de archivos
	
	return;
}



// private methods

LPVOID CPlaylist::FindFirstEmpty(void)
{
	LPVOID salida=NULL;
	WCHAR number[256];
	ZeroMemory(number,sizeof(number));
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int cantidad_elementos=6000;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if (aglobalrecord[i].Status == 0)
		{
			salida= (LPVOID)&aglobalrecord[i];
			break;
		}
	}
	ReleaseMutex(GlobalMutexSearchpos);
	return salida;
}

void CPlaylist::GetMinDate(HWND datepicker)
{
	SYSTEMTIME asystime;
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int cantidad_elementos=6000;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if (aglobalrecord[i].Status==0)
		{
			break;
		};
		if (aglobalrecord[i].Status == 1)
		{
			FileTimeToSystemTime(&aglobalrecord[i].datetime,&asystime);
			break;
		}
	}

	SendMessage(datepicker,DTM_SETSYSTEMTIME,GDT_VALID,(LPARAM)&asystime);

	ReleaseMutex(GlobalMutexSearchpos);
	return;
}
void CPlaylist::GetMaxDate(HWND datepicker)
{
	SYSTEMTIME asystime;
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int cantidad_elementos=6000;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if (aglobalrecord[i].Status==0)
		{
			break;
		};
		if (aglobalrecord[i].Status == 1)
		{
			FileTimeToSystemTime(&aglobalrecord[i].datetime,&asystime);
		}
	}
	SendMessage(datepicker,DTM_SETSYSTEMTIME,GDT_VALID,(LPARAM)&asystime);
	ReleaseMutex(GlobalMutexSearchpos);
	return;
}

void CPlaylist::GetSources(HWND combo)
{
	// tengo una lista auxiliar para guardar datos
	// asi recorrerla y no meter duplicados
	WCHAR TotalRegistros[100][512];
	ZeroMemory(TotalRegistros,sizeof(TotalRegistros));


	SendMessage(combo,CB_RESETCONTENT,0,0);
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>((WCHAR*)L" All"));

	bool encontro;
	int cantidad_elementos=6000;
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int posicion_libre=0;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if ((aglobalrecord[i].Status != 0) && (aglobalrecord[i].Status !=2))
		{
			encontro=false;
			for (int x=0;x<=posicion_libre;x++)
			{
				if (lstrcmpi(TotalRegistros[x],aglobalrecord[i].Source)==0)
				{
					encontro=true;		
				}
			}
			if (!encontro)
			{
				SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>((WCHAR*)aglobalrecord[i].Source));
				wcscat(TotalRegistros[posicion_libre],aglobalrecord[i].Source);
				posicion_libre++;
			}
			if (aglobalrecord[i].Status==0)
			{
				break;
			};

		}
};

	SendMessage(combo,CB_SETCURSEL,0,0);
	ReleaseMutex(GlobalMutexSearchpos);
	return;
};
void CPlaylist::GetStatus(HWND combo)
{
	SendMessage(combo,CB_RESETCONTENT,0,0);
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L" All"));
	SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"No Listen"));
	SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"Deleted"));
	SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"Listen"));
	SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"Encrypted"));
	
	SendMessage(combo,CB_SETCURSEL,0,0);

	
	// STATUS 0 LIBRE
	// STATUS 1 OK
	// STATUS 2 DELETE
	// STATUS 3 READED
	ReleaseMutex(GlobalMutexSearchpos);
	return ;
};

void CPlaylist::GetDestination(HWND combo)
{
	// tengo una lista auxiliar para guardar datos
	// asi recorrerla y no meter duplicados
	WCHAR TotalRegistros[100][512];
	ZeroMemory(TotalRegistros,sizeof(TotalRegistros));


	SendMessage(combo,CB_RESETCONTENT,0,0);
	WaitForSingleObject(GlobalMutexSearchpos,INFINITE);
	
	SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>((WCHAR*)L" All"));

	// recorrer toda la lista
	// sacar el texto
	bool encontro;
	int cantidad_elementos=6000;
	PRECORD_PLAYLIST aglobalrecord = (PRECORD_PLAYLIST)PointerFileInMemory;
	int posicion_libre=0;
	for (int i=0;i<cantidad_elementos;i++)
	{
		if ((aglobalrecord[i].Status != 0) && (aglobalrecord[i].Status !=2))
		{
			encontro=false;
			for (int x=0;x<=posicion_libre;x++)
			{
				if (lstrcmpi(TotalRegistros[x],aglobalrecord[i].Destination)==0)
				{
					encontro=true;		
				}
			}
			if (!encontro)
			{
				SendMessage(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>((WCHAR*)aglobalrecord[i].Destination));
				wcscat(TotalRegistros[posicion_libre],aglobalrecord[i].Destination);
				posicion_libre++;
			}
		}
		if (aglobalrecord[i].Status==0)
		{
			break;
		};

};

	SendMessage(combo,CB_SETCURSEL,0,0);
	ReleaseMutex(GlobalMutexSearchpos);
	return;

	return ;
};



int CPlaylist::ItemCount(void)
{
	return 6000;
}




void CPlaylist::InitFile(void)
{
	ZeroMemory(FileNameFile,sizeof(FileNameFile));
	ReadRegistryDBPath(FileNameFile);
	wcscat(FileNameFile,L"Database\\HistoryCalls.dat");
	HANDLE myfileWrite = CreateFile(FileNameFile , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	PRECORD_PLAYLIST a = new RECORD_PLAYLIST;
	ZeroMemory(a,sizeof(RECORD_PLAYLIST));
	DWORD writenBytes;
	for (int i=0;i<6000;i++)
	{
		ZeroMemory(a,sizeof(RECORD_PLAYLIST));
		a->Id=i;
		a->Status = 0;

		WriteFile(myfileWrite,a,sizeof(RECORD_PLAYLIST),&writenBytes,NULL);
	};
	CloseHandle(myfileWrite);
};




