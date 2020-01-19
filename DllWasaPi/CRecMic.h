#include "windows.h"
#include "mmsystem.h"
#include "Mmreg.h"
#include "CList.h"

#define TAMANO_BUFFER 10

class CRecMic
{

	
public:
	WAVEFORMATEX MyWaveFormatex;
	CListA* MyListaMic;
	HWAVEIN  mywaveIn;
	unsigned int GMyDeviceIn;
	WAVEHDR myWaveHdr1[TAMANO_BUFFER];
	DWORD sample;
	DWORD sampleBase;

	CRecMic(WCHAR* afilename);
	CRecMic(WCHAR* filename , WCHAR* objectName , DWORD HZorigen, DWORD hzFin);

	~CRecMic();
	bool IsPaused;
	void StartRec(void);
	void StopRec(void);
	void SetWaveFormat(void);
	void Pause(void);
	void Continue(void);


};

