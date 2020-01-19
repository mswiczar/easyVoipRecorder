// PlaySnd.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "playwave.h"	
#include "Clist.h"	
#include "stdlib.h"
#include "CrecMic.h"
#include "CMergeFile.h"


int _tmain(int argc, _TCHAR* argv[])
{
/*
	printf("Begin\n");
	CListA* MyListaPlay = new CListA;

	for (int i =1; i <  99999; i++)
	{
		ITEMLIST* PItemList = (ITEMLIST*) VirtualAlloc(NULL,sizeof(ITEMLIST), MEM_COMMIT,PAGE_EXECUTE_READWRITE);
		PItemList->UnDato.Buffer  = VirtualAlloc(NULL,5, MEM_COMMIT,PAGE_EXECUTE_READWRITE);
		PItemList->UnDato.BytesUsed = 5;
		itoa(i,(char *)PItemList->UnDato.Buffer,10);
		MyListaPlay->AddItem(PItemList);
		Sleep(1);
	}
	printf("end");
	delete MyListaPlay;
	Sleep(60000);
	PatchMemory();
		BeginRecording();
			PlaySnd("c:\\wav\\1.wav",0);	
		StopRecording();
	UnpatchMemory();
	PlaySnd("c:\\wav\\1.wav",0);	
	PatchMemory();
		BeginRecording();
	CRecMic* MyMicRecorder;
	MyMicRecorder = new  CRecMic;
	MyMicRecorder->FileName ="C:\\MicIn.dat";
	MyMicRecorder->StartRec();
	PatchMemory();
		BeginRecording();
			PlaySnd("c:\\wav\\1.wav",0);	
		StopRecording();
		MyMicRecorder->StopRec();
UnpatchMemory();
delete MyMicRecorder;
	MergeFile("c:\\out.dat","c:\\micin.dat","C:\\Skype.out");
	GenerateWaveFile("C:\\Skype.out");

*/
return 0;

}

