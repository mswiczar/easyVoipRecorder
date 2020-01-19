#include "stdlib.h"
#include "stdafx.h"
#include "CRecMic.h"
#define TAMANO_BUFFER 10





void CALLBACK WaveInProcedure(HWAVEIN hwi, UINT uMsg,    DWORD dwInstance,    DWORD dwParam1,   DWORD dwParam2     )
{
	CRecMic* MyRecMic;
	ITEMLIST* PItemList;
	LPWAVEHDR myLPWAVEHDR;
	if (uMsg = WIM_DATA) 
	{
		myLPWAVEHDR = (LPWAVEHDR) dwParam1;
		if (myLPWAVEHDR != NULL)
		{
				MyRecMic =(CRecMic*) myLPWAVEHDR->dwUser;
				waveInUnprepareHeader  (MyRecMic->mywaveIn,  myLPWAVEHDR, sizeof(WAVEHDR));
				PItemList =	(ITEMLIST*) VirtualAlloc(NULL,sizeof(ITEMLIST), MEM_COMMIT,PAGE_EXECUTE_READWRITE);
				PItemList->UnDato.Buffer  =  (ITEMLIST*) VirtualAlloc(NULL,myLPWAVEHDR->dwBytesRecorded, MEM_COMMIT,PAGE_EXECUTE_READWRITE);
				PItemList->UnDato.BytesUsed = myLPWAVEHDR->dwBytesRecorded;
				memcpy(PItemList->UnDato.Buffer, myLPWAVEHDR->lpData,myLPWAVEHDR->dwBytesRecorded);
				MyRecMic->MyListaMic->AddItem(PItemList);
				myLPWAVEHDR->dwFlags = 0;
				waveInPrepareHeader    (MyRecMic->mywaveIn,  myLPWAVEHDR, sizeof(WAVEHDR));
				waveInAddBuffer    (MyRecMic->mywaveIn,myLPWAVEHDR, sizeof(WAVEHDR));

		}
	}
}


void CRecMic::StartRec(void)
{
	waveInStart(this->mywaveIn);
	IsPaused = false;

};


void CRecMic::StopRec(void)
{
	waveInStop(mywaveIn);
	Sleep(300);
	waveInClose(mywaveIn);
}


void CRecMic::Pause(void)
{
	if (IsPaused != true)
	{
		waveInStop(mywaveIn);
		IsPaused = true;
	};

}

void CRecMic::Continue(void)
{
	if (IsPaused )
	{
		waveInStart(mywaveIn);
		IsPaused = false;
	};

}


void CRecMic::SetWaveFormat(void)
{
	this->MyWaveFormatex.cbSize          = sizeof(WAVEFORMATEX);
	this->MyWaveFormatex.nChannels       = 1;
	this->MyWaveFormatex.nSamplesPerSec  = 48000;
	this->MyWaveFormatex.wBitsPerSample  = 16;
	this->MyWaveFormatex.wFormatTag      = 1;
	this->MyWaveFormatex.nBlockAlign     = this->MyWaveFormatex.nChannels * (this->MyWaveFormatex.wBitsPerSample/8);
	this->MyWaveFormatex.nAvgBytesPerSec = this->MyWaveFormatex.nBlockAlign * this->MyWaveFormatex.nSamplesPerSec;
}

CRecMic::CRecMic(WCHAR* afilename)
{
	DWORD  myThreadIdIN;
	GMyDeviceIn        = WAVE_MAPPER;   
	int tamano;
	int i = 0;
	MyListaMic = new CListA(afilename,0,0,0,0);

	MMRESULT LRet;
	this->SetWaveFormat();

}



CRecMic::CRecMic(WCHAR* filename , WCHAR* objectName , DWORD HZorigen, DWORD hzFin)
{
	DWORD  myThreadIdIN;
	GMyDeviceIn        = WAVE_MAPPER;   
	int tamano;
	int i = 0;
	MyListaMic = new CListA(filename,HZorigen,hzFin,objectName,false);
	MMRESULT LRet;
	this->SetWaveFormat();

}




CRecMic::~CRecMic()
{
	waveInClose(mywaveIn);
}
