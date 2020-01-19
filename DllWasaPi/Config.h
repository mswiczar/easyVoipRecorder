

typedef struct  _TSetings{
	  WCHAR TempPath[512];
	  WCHAR DestPath[512];
	  WCHAR Filename[512];

	  WCHAR FilenameSpeaker[512];
	  WCHAR FilenameMic[512];
	  DWORD HZ;
}TSetings , *PTSetings;


void SetSetings(PTSetings aset);
void GetSetings(PTSetings aset);
