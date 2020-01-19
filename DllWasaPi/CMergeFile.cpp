#include "stdafx.h"
#include "CMergeFile.h"
#include "stdlib.h"

// junta los 2 archivos en uno solo generando una salida del tamano doble buffer*2
void Merge(LPVOID mybuffer1, LPVOID mybuffer2, LPVOID mybufferOut, DWORD mysize)
{
	short channel1, channel2 ;
		// estereo
		unsigned char * charcharBuff1 = (unsigned char *) mybuffer1;
		unsigned char * charcharBuff2 = (unsigned char *) mybuffer2;
		unsigned char * charcharBuffOut = (unsigned char *) mybufferOut;
		for (DWORD i = 0; i <= mysize/2 -1; i++)
		{
			charcharBuffOut[i*4] = charcharBuff1[i*2];
			charcharBuffOut[i*4+1] = charcharBuff1[i*2+1];
			charcharBuffOut[i*4+2] = charcharBuff2[i*2];
			charcharBuffOut[i*4+3] = charcharBuff2[i*2+1];
		}
}

// la salida es stereo, osea el bufferout es del doble tamano que el buffer in, mide el doble al pedo

void MergeMono(LPVOID mybuffer1, LPVOID mybuffer2, LPVOID mybufferOut, DWORD mysize)
{
	short channel1, channel2,channelmerge ;
	div_t div_result;
	unsigned char * charcharBuff1 = (unsigned char *) mybuffer1;
	unsigned char * charcharBuff2 = (unsigned char *) mybuffer2;
	unsigned char * charcharBuffOut = (unsigned char *) mybufferOut;
	for (DWORD i = 0; i <= mysize/2 -1; i++)
	{
		MoveMemory(&channel1,&charcharBuff1[i*2],sizeof(channel1));
		MoveMemory(&channel2,&charcharBuff2[i*2],sizeof(channel1));
		channelmerge = (channel1 / 2 ) + (channel2 /2);
		MoveMemory(&charcharBuffOut[i*4],&channelmerge,sizeof(channel1));
		MoveMemory(&charcharBuffOut[i*4+2],&channelmerge,sizeof(channel2));
	}
}

// la salida es en mono, osea el bufferout es del mismo tamano que el buffer in

void MergeMono2Mono(LPVOID mybuffer1, LPVOID mybuffer2, LPVOID mybufferOut, DWORD mysize)
{
	short channel1, channel2,channelmerge ;
	div_t div_result;
	unsigned char * charcharBuff1 = (unsigned char *) mybuffer1;
	unsigned char * charcharBuff2 = (unsigned char *) mybuffer2;
	unsigned char * charcharBuffOut = (unsigned char *) mybufferOut;
	for (DWORD i = 0; i <= mysize/2 -1; i++)
	{
		MoveMemory(&channel1,&charcharBuff1[i*2],sizeof(channel1));
		MoveMemory(&channel2,&charcharBuff2[i*2],sizeof(channel1));
		channelmerge = (channel1 / 2 ) + (channel2 /2);
		MoveMemory(&charcharBuffOut[i*2],&channelmerge,sizeof(channel1));
	}
}
