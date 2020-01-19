#include "windows.h"
#include "mmsystem.h"
#include "Mmreg.h"


#ifndef WAV_H
#define WAV_H

#include "mmsystem.h"
#include "Mmreg.h"
#include "WAV.h"


typedef struct {
    WORD  wFormatTag;
    WORD  nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD  nBlockAlign;
    WORD  wBitsPerSample;
	} WAVEFORMATEXAZ;



typedef struct  WaveRecord
{
	char riff[4];
	DWORD   tamano;
	char WAVEfmt[8];
	DWORD   formato;
	WAVEFORMATEXAZ myw;
	char datos[4];
	DWORD bytes_muestra;
} TWAVERECORD;




int PlaySnd(char* wav );
int EmotiPlayer(char* wav, LPWAVEHDR amywavehdr );

void GenerateWaveFile(WCHAR*  FileName,WCHAR*  FileNameout, DWORD HZ, WORD CHANNELS  );

void HzConVerter( LPWAVEHDR in_mywavehdr ,  LPWAVEFORMATEX in_mywaveformatex, LPWAVEHDR amywavehdr);

 

#endif
