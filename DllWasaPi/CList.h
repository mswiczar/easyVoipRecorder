#include "windows.h"
#include "time.h"

#include "..\INTERFACE\REG.h"

// en donde los tipos son 0 = wav; 1 = ogg, 2 = speex, 3 = mp3; 4 = wma; 5 = mpeg; 6 = youtube. 


typedef struct _TPROCESSLIST
	{
		WCHAR NombreEvent[512];
		WCHAR FileName[512];
		DWORD SampleSource;
		DWORD SampleDest;
	} TPROCESSLIST , *PTPROCESSLIST;



typedef struct TDATOS
	{
		LPVOID      Buffer;
		DWORD       BytesUsed;
		DWORD       BytesComress;
//		__time64_t  timeSyncro;
	} DATOS ;

typedef struct	TITEMLIST
	{
		LPVOID  NextItem;
		DATOS  UnDato;  
	} ITEMLIST;
typedef struct TLISTA
	{
		ITEMLIST* Header;
		ITEMLIST* Tail;
		int cantidadElementos;
	} LISTA;
class CListA {
public:
		CListA(WCHAR* aFileName, DWORD asamplebase);


		void SetFlush(void);
		~CListA();
		void AddItem(ITEMLIST* PItemList);
		LISTA* mylistaADD;
		LISTA* mylistaFlush;
		LISTA* mylista1;
		LISTA* mylista2;
		CRITICAL_SECTION MyMutex;
		HANDLE MytrheadFlush;
		HANDLE MyeventFlush;
		HANDLE MyeventFin;
		BOOL Running;
		DWORD WINAPI Flush(LPVOID lpParameter );
		ITEMLIST* Removeitem();
		WCHAR  FileName[512];
		DWORD Tick;
		bool syncronized;
		DWORD HZ;
};

class CMergedList
{
public:

	CMergedList(WCHAR* afile, TIPO_DESTINO mytipo);
	DWORD GetSize(WCHAR * filenames);

	~CMergedList();
	CListA* Mic;
	CListA* Wave;
	HWND MessageHWND;


	HANDLE eventCompress;

	void GenerateNames(void);
	WCHAR FileName[512];
	WCHAR FileNameMerged[512];
	WCHAR FileNameMergedDestWav[512];

	WCHAR FileNameNoPath[512];
	WCHAR FileNameFullPath[512];
	
	WCHAR RealDestination[512];
	
	TIPO_DESTINO tipo; 
	
	DWORD HZ;

	void GetOnlyName(WCHAR* aux);
	void MergeFiles(void);
	void MergeFilesSynchro(void);
	
	DWORD AppendFilesWav(void);

	void ProcessFiles(void);
	void MergeFiles2Mono(void);
	void EncodeFiles(void);

	void Start(void);
	bool running;
	void Stop(void);
	HANDLE ThreadSync;
	int bitrate;
};


