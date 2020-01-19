#include "windows.h"
int GetPosition(LPVOID buffer, int elementos);
int GetPositionCOM(LPVOID buffer, int elementos);
int GetPositionCOM2(LPVOID buffer, int elementos);

bool iswinXP (void);

LPVOID GetVirtualTableFrom(BYTE * buffer) ;



/*
73f1e7b0  	IDirectSoundBufferImpl_Lock,
73f1e870 	IDirectSoundBufferImpl_GetCurrentPosition, STDMETHOD(GetCurrentPosition)   (THIS_ LPDWORD, LPDWORD) PURE;
73f1e8dd  	IDirectSoundBufferImpl_Unlock,

*/



/*
73f5d601 	IDirectSoundBufferImpl_QueryInterface,
73f394ea  	IDirectSoundBufferImpl_AddRef,
73f197bd 	IDirectSoundBufferImpl_Release,
73f39fe8  	IDirectSoundBufferImpl_GetCaps,

73f1e870 	IDirectSoundBufferImpl_GetCurrentPosition, STDMETHOD(GetCurrentPosition)   (THIS_ LPDWORD, LPDWORD) PURE;

73f3a247  	IDirectSoundBufferImpl_GetFormat,
73f22bc8 	IDirectSoundBufferImpl_GetVolume,
73f3a057  	IDirectSoundBufferImpl_GetPan,
73f3a0c2 	IDirectSoundBufferImpl_GetFrequency,
73f2175a  	IDirectSoundBufferImpl_GetStatus,
73f3a12d 	IDirectSoundBufferImpl_Initialize,

73f1e7b0  	IDirectSoundBufferImpl_Lock,

73f1dc9d 	IDirectSoundBufferImpl_Play,
73f1dad2  	IDirectSoundBufferImpl_SetCurrentPosition, NO HACE FALTA
73f1db2e 	IDirectSoundBufferImpl_SetFormat,
73f1dd3e  	IDirectSoundBufferImpl_SetVolume,
73f1dbac 	IDirectSoundBufferImpl_SetPan,
73f1dc25  	IDirectSoundBufferImpl_SetFrequency,
73f22c32 	IDirectSoundBufferImpl_Stop,

73f1e8dd  	IDirectSoundBufferImpl_Unlock,

73f3a2d6 	IDirectSoundBufferImpl_Restore,
73f3a32f  	IDirectSoundBufferImpl_SetFX,
73f3a416 	IDirectSoundBufferImpl_AcquireResources,
73f3a1b6	IDirectSoundBufferImpl_GetObjectInPath


 /* IUnknown methods */
    /* IDirectSoundCaptureBuffer methods 
0    IDirectSoundCaptureBufferImpl_QueryInterface,
1    IDirectSoundCaptureBufferImpl_AddRef,
2    IDirectSoundCaptureBufferImpl_Release,
3    IDirectSoundCaptureBufferImpl_GetCaps,
4    IDirectSoundCaptureBufferImpl_GetCurrentPosition,
5    IDirectSoundCaptureBufferImpl_GetFormat,
6    IDirectSoundCaptureBufferImpl_GetStatus,
7    IDirectSoundCaptureBufferImpl_Initialize,
8    IDirectSoundCaptureBufferImpl_Lock,
9    IDirectSoundCaptureBufferImpl_Start,
10   IDirectSoundCaptureBufferImpl_Stop,
11   IDirectSoundCaptureBufferImpl_Unlock,

12    IDirectSoundCaptureBufferImpl_GetObjectInPath,
13    IDirectSoundCaptureBufferImpl_GetFXStatus

*/