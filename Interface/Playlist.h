#pragma once
#include "stdafx.h"	

typedef struct  _RECORD_PLAYLIST
{
	int Id;
	int Status;
	FILETIME	datetime;
	WCHAR FileName[256];
	WCHAR Source[64];
	WCHAR Destination[64];
	WCHAR Comments[512];
} RECORD_PLAYLIST, * PRECORD_PLAYLIST ;
class CPlaylist
{
	public:
		CPlaylist(void);
		~CPlaylist(void);
		bool Search_RecordById(PRECORD_PLAYLIST arecord);
		bool Search_RecordByFileName(PRECORD_PLAYLIST arecord);

		void Modify_Record(PRECORD_PLAYLIST arecord);
		void Delete_Record(PRECORD_PLAYLIST arecord);
		void Insert_Record(PRECORD_PLAYLIST arecord);
		LPVOID FindFirstEmpty(void);
		void InitFile(void);
		int ItemCount(void);

		void GetSources(HWND combo);
		void GetStatus(HWND combo);
		void GetDestination(HWND combo);

		void GetMinDate(HWND datepicker);
		void GetMaxDate(HWND datepicker);

	HANDLE  File;
	HANDLE FileMapping;
	HANDLE PointerFileInMemory;
	HANDLE GlobalMutexSearchpos;
	WCHAR FileNameFile[512];
};
