#define UNICODE


#include "AudioSupport.h"
#include "reg.h"
#include "..\DllWasaPi\WAV.h"
#include "helper.h"

void GetFileNameOut(WCHAR* afilein, WCHAR* afileout,int to,WCHAR *prefix);
void Convert2MonoWav(WCHAR* source, WCHAR* destination);


bool convertWav2mp3(WCHAR* wavinput , WCHAR* destination);
bool convertWav2Ogg(WCHAR* wavinput , WCHAR* destination);
bool convertWav2speex(WCHAR* wavinput , WCHAR* destination);
bool convertWav2Wma(WCHAR* wavinput , WCHAR* destination);


bool convertOgg2Wav(WCHAR* sourceOgg , WCHAR* destinationwav);
bool convertWma2Wav(WCHAR* input, WCHAR* destination);
bool convertmp32Wav(WCHAR* input, WCHAR* destination);
bool convertspeex2Wav(WCHAR* input , WCHAR* destination);





void GetJustNameNoExtension(WCHAR* infile ,WCHAR* out)
{
	int tamano;
	int fin =0;
	tamano = wcslen(infile);
	int inicio =0;
	for (int i= tamano-1;i>0;i--)
	{
		if (infile[i]==L'\\'&& (inicio==0))
		{
			inicio =i+1;
			break;
		}
		if (infile[i]==L'.'&& (fin==0))
		{
			fin =i;
		}

	}
	wcscat(out,&infile[inicio]);
	out[fin-inicio]=0x00;
}


void FromType2Extension (WCHAR* textext , int extension)
{
	if (extension==0)
	{
		wcscat(textext,L".WAV");
		return;
	}

	if (extension==1)
	{
		wcscat(textext,L".OGG");
		return;
	}

	if (extension==2)
	{
		wcscat(textext,L".SPX");
		return;
	}

	if (extension==3)
	{
		wcscat(textext,L".MP3");
		return;
	}

	if (extension==4)
	{
		wcscat(textext,L".WMA");
		return;
	}

}




void GetFileNameOut(WCHAR* afilein, WCHAR* afileout,int to,WCHAR *prefix )
{
	WCHAR filename[128];
	WCHAR  FolderDestination[128];
	WCHAR  extension[64];

	ZeroMemory(filename,sizeof(filename));
	ZeroMemory(FolderDestination,sizeof(FolderDestination));
	ZeroMemory(extension,sizeof(extension));

	FromType2Extension(extension,to);
	GetJustNameNoExtension(afilein,filename);
	ReadRegistryExportPath(FolderDestination);

	wsprintf(afileout,L"%s%s%s%s",FolderDestination,prefix,filename,extension);
}


void GetWavInputName(WCHAR * infile, WCHAR *wavinput,WCHAR *prefix )
{
	WCHAR filename[128];
	WCHAR FolderDestination[128];

	ZeroMemory(filename,sizeof(filename));
	ZeroMemory(FolderDestination,sizeof(FolderDestination));

	GetJustNameNoExtension(infile,filename);
	ReadRegistryExportPath(FolderDestination);

	wsprintf(wavinput,L"%stemp_%s%s%s",FolderDestination,prefix,filename,L".Wav");

}




int FromExtension2Type (WCHAR* extension)
{
	if (lstrcmpi(extension,L"WAV")==0)
	{
		return 0; 
	}

	if (lstrcmpi(extension,L"OGG")==0)
	{
		return 1; 
	}

	if (lstrcmpi(extension,L"SPX")==0)

	{
		return 2; 
	}

	if (lstrcmpi(extension,L"MP3")==0)
	{
		return 3; 
	}

	if (lstrcmpi(extension,L"WMA")==0)
	{
		return 4; 
	}
	return -1;

}



bool GetTypeFromName(WCHAR* source, int* adestino)
{
	int tamano;
	tamano = wcslen(source);
	for (int i= tamano-1;i>0;i--)
	{
		if ((source[i]==L'.'))
		{
			*adestino = FromExtension2Type(&source[i+1]);
			if ((*adestino) ==-1)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
return false;
}




bool Convert2Mono(WCHAR* infile, WCHAR* out,HWND hDlg)
{
	//tengo el filename in
	//tengo que conseguir el filename out
	WCHAR wavinput[512];
	WCHAR wavmono[512];
	ZeroMemory(wavinput,sizeof(wavinput));
	ZeroMemory(wavmono,sizeof(wavmono));

	int tipoorigen=0;
	GetTypeFromName(infile,&tipoorigen);
	int to;
	to= tipoorigen;

	if(tipoorigen==0)
	{
		// es wav
		// solo convertirlo a wav y terminar
		wcscat(wavinput,infile);
		GetFileNameOut(infile ,out, to,L"Mono_");
		Convert2MonoWav(infile,out);
		return true;
	}

	GetFileNameOut(infile ,out, to,L"Mono_");
	GetWavInputName(infile, wavinput,L"Mono_");			
	switch (tipoorigen)
		{
			case 1:
				// converto ogg to wav
				convertOgg2Wav(infile, wavinput);
				break;
			case 2:
				// converto spx to wav
				convertspeex2Wav(infile, wavinput);
				break;
			case 3:
				// converto mp3 to wav
				convertmp32Wav(infile, wavinput);
				break;
			case 4:
				// converto wma to wav
				convertWma2Wav(infile, wavinput);
				break;
		}
	
	// ahora tengo el wav
	GetWavInputName(infile, wavmono,L"Mono_temp_");			
	Convert2MonoWav(wavinput,wavmono);
	DeleteFile(wavinput);

	switch (to)
		{
			case 1:
				// converto to ogg
				convertWav2Ogg(wavmono, out);
				break;
			case 2:
				// converto to spx
				convertWav2speex(wavmono, out);
				break;
			case 3:
				// converto to mp3
				convertWav2mp3(wavmono,out);
				break;
			case 4:
				// converto to wma
				convertWav2Wma(wavmono ,out);
				break;
		};

		DeleteFile(wavmono);

	return true;
}




void MonoIt(LPVOID mybufferIN, LPVOID mybufferOUT,DWORD asize)
{
	// aca esta 16 bits+16 bits
	//tengo que convertir en solo 16 bits

		// estereo
		short * channels = ( short *) mybufferIN;
		short * charcharBuffOut = (short *) mybufferOUT;
		asize = (asize/2 -1)/2;
		for (DWORD i = 0; i <= asize; i++)
		{
			charcharBuffOut[i] = ( channels[i*2] /2 )  + ( channels[i*2+1] /2 ) ;
		}

}



void Convert2MonoWav(WCHAR* source, WCHAR* destination)
{
	DWORD filesizeHightBytes;
	WCHAR destinationaux[512];
	ZeroMemory(destinationaux,sizeof(destinationaux));
	wcscat(destinationaux, destination);
	wcscat(destinationaux, L".tmp");


	LPVOID mybuffer1   = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	LPVOID mybufferOut = VirtualAlloc(NULL,16384, MEM_COMMIT,PAGE_EXECUTE_READWRITE);	
	HANDLE myfileRead1 = CreateFile(source, GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_READONLY ,NULL);
	HANDLE myfileWrite = CreateFile(destinationaux , GENERIC_WRITE , 0 , NULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);
	DWORD  filesizeLowBytes1 = GetFileSize(myfileRead1,&filesizeHightBytes);
	DWORD readedBytes1;
	DWORD writenBytes;
	//Leer hasta el fin de la cabecera
	// poner el puntero de myfileRead1
	
	TWAVERECORD awav;
	ReadFile(myfileRead1,&awav,sizeof(TWAVERECORD),&readedBytes1,NULL);

	while ((ReadFile(myfileRead1,mybuffer1,16384,&readedBytes1,NULL) != 0) &&  (readedBytes1 !=0))
	{
			MonoIt(mybuffer1,mybufferOut,readedBytes1);
			WriteFile(myfileWrite,mybufferOut,(readedBytes1)/2,&writenBytes,NULL);
	}
	CloseHandle(myfileRead1);
	CloseHandle(myfileWrite);
	
	GenerateWaveFile(destinationaux,destination,awav.myw.nSamplesPerSec,1);
	
	DeleteFile(destinationaux);

	VirtualFree(mybuffer1,16384,MEM_DECOMMIT);
	VirtualFree(mybuffer1,0,MEM_RELEASE);
	VirtualFree(mybufferOut,16384,MEM_DECOMMIT);
	VirtualFree(mybufferOut,0,MEM_RELEASE);
	}

void GetJustName(WCHAR* infile ,WCHAR* out)
{
	int tamano;
	tamano = wcslen(infile);
	int inicio =0;
	for (int i= tamano-1;i>0;i--)
	{
		if (infile[i]==L'\\'&& (inicio==0))
		{
			inicio =i+1;
			break;
		}
	}
	wcscat(out,&infile[inicio]);
}



bool ConvertFromTo(WCHAR* infile ,WCHAR* out, int to, HWND hDlg)
{
	// buscar el desde
	//compara el desde si es = que el hasta 
	WCHAR wavinput[512];
	ZeroMemory(wavinput,sizeof(wavinput));
	bool must_delete_input=false;
	int tipoorigen=0;
	GetTypeFromName(infile,&tipoorigen);
	if (to==tipoorigen)
	{
		MessageBox(hDlg,L"The source format is equal to the output format \n No conversion needed",MRecorder,48);
		return false;
	}
	else
	{
		if(tipoorigen==0)
		{
			wcscat(wavinput,infile);
			must_delete_input=false;
			GetFileNameOut(infile ,out, to,L"");
		}
		else
		{
			must_delete_input=true;
			// convertirlo todo a wav
			GetFileNameOut(infile ,out, to,L"");
			GetWavInputName(infile, wavinput,L"");			
			switch (tipoorigen)
			{
				case 1:
					// converto to ogg
					convertOgg2Wav(infile, wavinput);
					break;
				case 2:
					// converto to spx
					convertspeex2Wav(infile, wavinput);
					break;
				case 3:
					// converto to mp3
					convertmp32Wav(infile, wavinput);
					break;
				case 4:
					// converto to wma
					convertWma2Wav(infile, wavinput);
					break;
			}
		}
		switch (to)
			{
				case 0:
					MoveFile(wavinput,out);
					break;
				case 1:
					// converto to ogg
					convertWav2Ogg(wavinput, out);
					break;
				case 2:
					// converto to spx
					convertWav2speex(wavinput, out);
					break;
				case 3:
					// converto to mp3
					convertWav2mp3(wavinput,out);
					break;
				case 4:
					// converto to wma
					convertWav2Wma(wavinput ,out);
					break;
			};

	}
	if (must_delete_input)
	{
		DeleteFile(wavinput);
	}


	return true;
}




///

bool convertWav2Ogg(WCHAR* wavinput , WCHAR* destination)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"%soggenc.exe\" \"%s\" -o \"%s\"",oggdecpathandexe,wavinput,destination);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;

}

bool convertWav2mp3(WCHAR* wavinput, WCHAR* destination)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"lame.exe\" \"%s\"  \"%s\"",wavinput,destination);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;
}

bool convertWav2Wma(WCHAR* wavinput, WCHAR* destination)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"lame.exe\" \"%s\"  \"%s\"",wavinput,destination);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;
}

bool convertWav2speex(WCHAR* wavinput , WCHAR* destination)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"%sspeexenc.exe \" -w \"%s\"  \"%s\"",oggdecpathandexe,wavinput,destination);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;

}

/// others to wav

bool convertOgg2Wav(WCHAR* sourceOgg , WCHAR* destinationwav)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"%soggdec.exe\" \"%s\" -o \"%s\"",oggdecpathandexe,sourceOgg,destinationwav);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;

}


bool convertspeex2Wav(WCHAR* input , WCHAR* destination)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"%sspeexdec.exe \" -w \"%s\"  \"%s\"",oggdecpathandexe,input,destination);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;

}
bool convertmp32Wav(WCHAR* input, WCHAR* destination)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"lame.exe\" \"%s\"  \"%s\"",input,destination);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;
}

bool convertWma2Wav(WCHAR* input, WCHAR* destination)
{
	WCHAR toexec[512];
	ZeroMemory(toexec,sizeof(toexec));
	WCHAR oggdecpathandexe[512];
	ZeroMemory(oggdecpathandexe,sizeof(oggdecpathandexe));
	ReadRegistryUtilsPath(oggdecpathandexe);
	wsprintf(toexec,L"\"lame.exe\" \"%s\"  \"%s\"",input,destination);
	EjecutarProgram(toexec,true,SW_HIDE);
	return true;
}