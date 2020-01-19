
#include "windows.h"
#include "CmixerChange.h"


void GenerateWaveFile(LPCTSTR  FileName,LPCTSTR  FileNameout);

int PlaySnd(char* wav ,unsigned int mydevice );
void Initialize(void);

void PatchMemory(int idinarray);
void UnpatchMemory();

int GetPosicionInArray(void);



WCHAR afilename [512];


void SetHWND(HWND ahwnd);

void BeginAll(WCHAR* afilename);
void StopAll();
void PauseAll();
void ContinueAll();

void SetIsSkype (bool is_skype);
void SetIsMSN   (bool ais_MSN);
void SetOlympic (bool ais_ol);
void SetisOther (bool ais_other);


