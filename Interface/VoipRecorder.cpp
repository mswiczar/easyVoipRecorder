#pragma comment ( lib, "psapi.lib") 
#pragma comment ( lib, "comctl32.lib") 

#define tiempo 1000 * 11 * 5
#define MSG_REC_START    WM_USER + 9002
#define MSG_REC_STOP     WM_USER + 9003
#define MSG_REC_PAUSE    WM_USER + 9004
#define MSG_REC_CONTINUE WM_USER + 9005

#define MSG_NEWAPP   WM_USER + 9014
#define MSG_DELAPP   WM_USER + 9015

#define MSG_MICBEGIN   WM_USER + 9012
#define MSG_MICEND   WM_USER + 9013

#define MSG_GETSOURCE   WM_USER + 9020


#define MSG_SPEAKERDATA   WM_USER + 9030
#define MSG_MICDATA   WM_USER + 9040

#define MSG_SIZEKB   WM_USER + 9050


#define MSG_REFRESHPLAYLIST   WM_USER + 8000
#define MSG_SHOWDASHBOARD   WM_USER + 8001



#include "stdafx.h"
#include <shellapi.h>
#include "psapi.h"
	

#include "Sprite.h"
#include "reg.h"

#include "VoipRecorder.h"
#include "ping.h"

#include <commctrl.h>
#define MAX_LOADSTRING 100

#include "playlist.h"
#include "helper.h"

#include "audiosupport.h"

#include "SkypeApi.h"

#include "WasapiNames.h"


//SkypeApi MySkypeApi;

WCHAR destinationshare[512];

DWORD Global_pid;
int Position_array=-1;
int posiI=0;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szURL2Deniz[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
NOTIFYICONDATA IconData; 
WNDCLASSEX wcex;

int factor_grabacion;

HWND Global_hWnd;
HWND  Global_hWndhWndSprite;
HWND Global_STATIC;
HWND  Global_STATICSource;
HWND Global_PlayList;
HWND m_Video;

HFONT hFont;
WCHAR alista[1024*1024];

WCHAR aini[512];

CPlaylist * Global_aplaylist;

void ClickStop(LPVOID avoid);

HANDLE fakeevent;

 void  __stdcall function(DWORD a , DWORD b,DWORD c)
{
	ClickStop((LPVOID) 1);
	return;
}

 void functionpatch(char* acode,char* posicion)
{
	BOOL a=false;
	DWORD b=true;
	WCHAR ab[2];
	
	memcpy(&posicion[5],acode,4);
	posicion[9]=0X90;

	fakeevent=CreateEvent(NULL,a,b,ab);
	CloseHandle(fakeevent);
	return;
}


const   WCHAR *ComboBoxItems_upload[] = { MRecorder,NULL, L"FTP",NULL, L"SayAndPost",
                                    NULL,L"http://www.youtube.com/",NULL};

const WCHAR *ComboBoxItems_encoding[] = { L"Wav", L"Ogg",NULL};

const WCHAR *ComboBoxItems_encoding_quality[] = { L"High", L"Medium", L"Low",
                                    L"Very Low",NULL};

WCHAR filenameRecord[512];


void GetInipath(void)
{
 ZeroMemory(aini,sizeof(aini));
 ReadRegistryPath(aini);
 wcscat(aini,MRecorder L".ini");
}


bool paused=false;

CGaugeSprite* Gauge_Mic;
CGaugeSprite* Gauge_Speaker;

CTextSprite * TimeRecording;
CTextSprite * KBRecording;


CSprite* Pause_disabled;
CSprite* Pause_enabled;

CSprite* Stop_disabled;
CSprite* Stop_enabled;

CSprite* Rec_disabled;
CSprite* Rec_enabled;

unsigned long segundos;

void Refresh_Listview(HWND hDlg);


bool GetFIlenameFromPlaylist( HWND hDlg,WCHAR* anametxt)
{
	LRESULT iSlected;
	iSlected=SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
	if(iSlected==-1)
	{
			return false;
	}

		WCHAR Text[255]={0};
		LVITEM  LvItem;
		memset(&LvItem,0,sizeof(LvItem));
		LvItem.mask=LVIF_TEXT;
		LvItem.iSubItem=4;
		LvItem.pszText=Text;
		LvItem.cchTextMax=256;
		LvItem.iItem=iSlected;

		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_GETITEMTEXT, 
					iSlected, (LPARAM)&LvItem); 

		wsprintf(anametxt,Text);


	return true;
}


void PrintProcessNameAndID(DWORD Pid)
{
	
	WCHAR salida[1024];
	ZeroMemory(salida,sizeof(salida));
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, Pid );
    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseNameW( hProcess, hMod, salida, 
                               sizeof(salida)/2 );
			SetWindowText(Global_STATICSource,salida);
			//MessageBox(0,salida,salida,0);

		}

    }

	ZeroMemory(salida,sizeof(salida));
    CloseHandle( hProcess );
}




bool is_ogg(void)
{
	RECORDINGOPTIONS arec;	
	ReadRegistryRecordingOptions(&arec);
	if (arec.Format==1)
	return true;
	else
		return false;
}



void ClickRecord(LPVOID avoid)
{
// si es ogg es 4 
// si es wav es asdfas

reintentar:
	HWND awindow = FindWindow(MRecorderService, NULL);
	int intento=0;
	if (awindow != NULL)
	{

		if (0==SendMessage(awindow,MSG_REC_START,Position_array,0))
		{
			factor_grabacion=0;
			paused=false;
			Pause_enabled->SetVisible(true);
			Pause_disabled->SetVisible(false);
			Stop_enabled->SetVisible(true);
			Stop_disabled->SetVisible(false);

			Rec_disabled->SetVisible(true);
			Rec_enabled->SetVisible(false);

			Rec_enabled->onover=false;

			::Gauge_Mic->porcentaje=0;
			::Gauge_Speaker->porcentaje=0;

			::Gauge_Mic->SetVisible(true);
			::Gauge_Speaker->SetVisible(true);


			::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_RECORD,MF_GRAYED|MF_BYCOMMAND);
			::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_PAUSE,MF_ENABLED|MF_BYCOMMAND);
			::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_STOP,MF_ENABLED|MF_BYCOMMAND);
			::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOSTARTRECORDING,MF_GRAYED|MF_BYCOMMAND);


			::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_EQUALIZE,MF_GRAYED|MF_BYCOMMAND);
			::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_SUBMIT,MF_GRAYED|MF_BYCOMMAND);
			::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_ATACHTOEMAIL,MF_GRAYED|MF_BYCOMMAND);

			segundos =0;

			ZeroMemory(TimeRecording->CurrentCaption,sizeof(TimeRecording->CurrentCaption));
			wsprintf(TimeRecording->CurrentCaption,L"Time: %02d:%02d:%02d.%02d",(segundos/60/60),( segundos/60)%60,segundos % 60,0);
			
			ZeroMemory(KBRecording->CurrentCaption,sizeof(KBRecording->CurrentCaption));
			wsprintf(KBRecording->CurrentCaption,L"%d KBytes",0);

			SetWindowText(Global_STATIC,L" Status: Recording.");
			SetTimer(Global_hWndhWndSprite,98,100,NULL);
			SetTimer(Global_hWndhWndSprite,99,tiempo,NULL);
			SetTimer(Global_hWndhWndSprite,97,tiempo/2,NULL);

		}
		else
		{
			// 		
			MessageBox(Global_hWnd,L"Warning, there is no active VOIP conversation to be recorded. \n Please wait until the comunication has been established and both parties reach the call.",MRecorder,48);
		}
	}
	else
	{
		int salida;
		if (intento <3)
		{
			salida= MessageBox(Global_hWnd,L"The easyVoipRecorderMonitor is not running now \n Would you like to start it now?",MRecorder,36);
			if (salida == 6)
			{
				intento++;
				EjecutarProgram(L"./easyVoipRecorderMonitor.exe",false,SW_SHOW);
				Sleep(2000);

/*
				SC_HANDLE hServCtrl = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
				if (hServCtrl == NULL) 
				{
				
				}
				else
				{
					SERVICE_STATUS sta;
					SC_HANDLE hServ = OpenService(hServCtrl, FULL_SERV_NAME, SC_MANAGER_ALL_ACCESS);
					if (hServ == NULL) 
					{
						//return GetLastError();
					}
					else
					{
						StartService(hServ,0,NULL);
						Sleep(2000);
					}
					CloseServiceHandle(hServCtrl);			
				}
*/
				goto reintentar;


			}
			else
			{
				::MessageBox(Global_hWnd,L"The easyVoipRecorderMonitor could not run without this Service \n Please contact to our customer support \n Thanks",MRecorder,16);	
			}
		}
		else
		{
				::MessageBox(Global_hWnd,L"The easyVoipRecorderMonitor could not start the Service \n Please contact to our customer support \n Thanks",MRecorder,16);	
		}
	}
};

void ClickStop(LPVOID avoid)
{
	//Stop(Global_hWnd);
	KillTimer(Global_hWndhWndSprite,97);

	HWND awindow = FindWindow(MRecorderService, NULL);
	SendMessage(awindow,MSG_REC_STOP,Position_array,0);



	paused=false;

	Pause_enabled->SetVisible(false);
	Pause_disabled->SetVisible(true);

	Stop_enabled->SetVisible(false);
	Stop_disabled->SetVisible(true);
	Stop_enabled->onover=false;

	::Gauge_Mic->porcentaje=0;
	::Gauge_Speaker->porcentaje=0;

	::Gauge_Mic->SetVisible(false);
	::Gauge_Speaker->SetVisible(false);

	Rec_enabled->SetVisible(true);
	Rec_disabled->SetVisible(false);


	::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_RECORD,MF_ENABLED|MF_BYCOMMAND);
	::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOSTARTRECORDING,MF_ENABLED|MF_BYCOMMAND);

	::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_PAUSE,MF_GRAYED|MF_BYCOMMAND);
	::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_STOP,MF_GRAYED|MF_BYCOMMAND);

   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_EQUALIZE,MF_ENABLED|MF_BYCOMMAND);
   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_SUBMIT,MF_ENABLED|MF_BYCOMMAND);
   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_ATACHTOEMAIL,MF_ENABLED|MF_BYCOMMAND);
	
	
	SetWindowText(Global_STATIC,L" Status: Ready.");
	KillTimer(Global_hWndhWndSprite,90);
	KillTimer(Global_hWndhWndSprite,98);
	KillTimer(Global_hWndhWndSprite,99);

	
	if	((int)avoid==1)
	{
		REGDATA aoptions;
		ReadRegistration(&aoptions);
		
		if (lstrcmpi(aoptions.CODE,L"")==0)
		{
			if (6==MessageBox(Global_hWndhWndSprite,L"You have an unregistered version of " MRecorder L"\nThis version has a recording  limit time of  ~30 seconds recording\nYou has reached this limit\nDo you want to purchase and register it now?", MRecorder,36))
			{
				ShellExecute(0,L"Open",L"http://www.easyvoiprecorder.com/index.php?option=com_content&task=view&id=14&Itemid=31",NULL,NULL,SW_SHOW);
				
			};
		};
	}

};



void ClickPause(LPVOID avoid)
{
	//Pause(Global_hWnd);
	HWND awindow = FindWindow(MRecorderService, NULL);
	if (paused)
	{
		SendMessage(awindow,MSG_REC_CONTINUE,Position_array,0);
		SetWindowText(Global_STATIC,L" Status: Recording.");
		::Gauge_Mic->SetVisible(true);
		::Gauge_Speaker->SetVisible(true);

	}
	else
	{
		SendMessage(awindow,MSG_REC_PAUSE,Position_array,0);
		SetWindowText(Global_STATIC,L" Status: Paused.");
		::Gauge_Mic->porcentaje=0;
		::Gauge_Speaker->porcentaje=0;

		::Gauge_Mic->SetVisible(false);
		::Gauge_Speaker->SetVisible(false);

	}
	Pause_enabled->onover=false;



	paused=!paused;
};



//sprite variables
HDC hdcCompatibleOffscreen;

CSprite* Graph;

CSpriteManager *MySpriteManager;


void InitSprite(void)
{
	CSprite *Botones;
	HDC myhdc = CreateDC(L"DISPLAY", NULL, NULL, NULL); 
	hdcCompatibleOffscreen = CreateCompatibleDC(myhdc);
	HBITMAP abitmap = CreateCompatibleBitmap(myhdc, 
                     300, 
                     300); 


	SelectObject(hdcCompatibleOffscreen, abitmap);

	POINT apoint;

//Fondo
	Botones = new CSprite(myhdc);
		Botones->resOn = 1001;
		Botones->resOver = 0;
		Botones->resClick = 0;
		Botones->SetVisible(true);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 1;
		apoint.y = 2;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =NULL;
		Botones->changeiconOver = false;
		Botones->SpriteText =   NULL;
		Botones->MouseMoveEvent = NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);

//rec
	Botones = new CSprite(myhdc);
		Botones->resOn = 5000;
		Botones->resOver = 5001;
		Botones->resClick = 0;
		Botones->SetVisible(true);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 6;
		apoint.y = 10;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =ClickRecord;
		Botones->changeiconOver = true;
		Botones->SpriteText =   NULL;
		Botones->MouseMoveEvent = NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);
		Rec_enabled = Botones;
//rec

	Botones = new CSprite(myhdc);
		Botones->resOn = 5000;
		Botones->resOver = 5000;
		Botones->resClick = 0;
		Botones->SetVisible(false);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 6;
		apoint.y = 10;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =NULL;
		Botones->changeiconOver = true;
		Botones->SpriteText =   NULL;
		Botones->MouseMoveEvent = NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);
		Rec_disabled = Botones;
		


		
// pause
	Botones = new CSprite(myhdc);
		Botones->resOn = 3000;
		Botones->resOver =  3001;
		Botones->resClick = 0;
		Botones->SetVisible(false);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 50;
		apoint.y = 10;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =ClickPause;
		Botones->changeiconOver = true;
		Botones->SpriteText =   NULL;
		Botones->MouseMoveEvent = NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);
		Pause_enabled = Botones;


	Botones = new CSprite(myhdc);
		Botones->resOn = 3000;
		Botones->resOver =  3000;
		Botones->resClick = 0;
		Botones->SetVisible(true);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 50;
		apoint.y = 10;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =NULL;
		Botones->changeiconOver = true;
		Botones->SpriteText =   NULL;
		Botones->MouseMoveEvent = NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);
		Pause_disabled = Botones;

		

// stop
	Botones = new CSprite(myhdc);
		Botones->resOn = 4000;
		Botones->resOver =4001;
		Botones->resClick = 0;
		Botones->SetVisible(false);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 94;
		apoint.y = 10;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =ClickStop;
		Botones->changeiconOver = true;
		Botones->SpriteText =   NULL;
		//memcpy(Botones->CurrentCaption,L"Adelante",wcslen(L"Adelante")*2);
		Botones->MouseMoveEvent = NULL;
		//Botones->SpriteHint2= NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);
		Stop_enabled = Botones;

	Botones = new CSprite(myhdc);
		Botones->resOn = 4000;
		Botones->resOver =4000;
		Botones->resClick = 0;
		Botones->SetVisible(true);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 94;
		apoint.y = 10;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =NULL;
		Botones->changeiconOver = true;
		Botones->SpriteText =   NULL;
		Botones->MouseMoveEvent = NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);
		Stop_disabled = Botones;


	Botones = new CSprite(myhdc);
		Botones->resOn = 100;
		Botones->resOver =0;
		Botones->resClick = 0;
		Botones->SetVisible(true);
		Botones->SetResizeable(false);
			Botones->resize_margin = 0;
			Botones->min_resize = 0;
		Botones->SetTransparent(false);
		Botones->transparentColor = RGB(255,0,0);
		Botones->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 150;
		apoint.y = 10;
		Botones->SetPosition(&apoint);
		Botones->m_spWebBrowser2 = NULL;
		Botones->ClickEventUp =NULL;
		Botones->changeiconOver = false;
		Botones->SpriteText =   NULL;
		Botones->MouseMoveEvent = NULL;
		Botones->Create();
		MySpriteManager->AddSprite(Botones);

		Graph = Botones;


		CGaugeSprite* BotonesGauge; 

	BotonesGauge = new CGaugeSprite(myhdc);
		BotonesGauge->porcentaje=0;
		BotonesGauge->resOn = 101;
		BotonesGauge->resOver =0;
		BotonesGauge->resClick = 0;
		BotonesGauge->SetVisible(true);
		BotonesGauge->SetResizeable(false);
			BotonesGauge->resize_margin = 0;
			BotonesGauge->min_resize = 0;
		BotonesGauge->SetTransparent(false);
		BotonesGauge->transparentColor = RGB(255,0,0);
		BotonesGauge->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 76;
		apoint.y = 63;
		BotonesGauge->SetPosition(&apoint);
		BotonesGauge->m_spWebBrowser2 = NULL;
		BotonesGauge->ClickEventUp =NULL;
		BotonesGauge->changeiconOver = false;
		BotonesGauge->SpriteText =   NULL;
		BotonesGauge->MouseMoveEvent = NULL;
		BotonesGauge->Create();
		MySpriteManager->AddSprite(BotonesGauge);
		Gauge_Speaker = BotonesGauge;

	BotonesGauge = new CGaugeSprite(myhdc);
		BotonesGauge->porcentaje=0;
		BotonesGauge->resOn = 101;
		BotonesGauge->resOver =0;
		BotonesGauge->resClick = 0;
		BotonesGauge->SetVisible(true);
		BotonesGauge->SetResizeable(false);
			BotonesGauge->resize_margin = 0;
			BotonesGauge->min_resize = 0;
		BotonesGauge->SetTransparent(false);
		BotonesGauge->transparentColor = RGB(255,0,0);
		BotonesGauge->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 76;
		apoint.y = 83;
		BotonesGauge->SetPosition(&apoint);
		BotonesGauge->m_spWebBrowser2 = NULL;
		BotonesGauge->ClickEventUp =NULL;
		BotonesGauge->changeiconOver = false;
		BotonesGauge->SpriteText =   NULL;
		BotonesGauge->MouseMoveEvent = NULL;
		BotonesGauge->Create();
		MySpriteManager->AddSprite(BotonesGauge);
		Gauge_Mic = BotonesGauge;


	 TimeRecording = new CTextSprite(myhdc);
		TimeRecording->resOn = 99;
		TimeRecording->resOver =0;
		TimeRecording->resClick = 99;
		TimeRecording->SetVisible(true);
		TimeRecording->SetResizeable(false);
			TimeRecording->resize_margin = 0;
			TimeRecording->min_resize = 0;
		TimeRecording->SetTransparent(false);
		TimeRecording->transparentColor = RGB(255,0,0);
		TimeRecording->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 154;
		apoint.y = 15;
		TimeRecording->SetPosition(&apoint);
		TimeRecording->m_spWebBrowser2 = NULL;
		TimeRecording->ClickEventUp =NULL;
		TimeRecording->changeiconOver = false;
		TimeRecording->SpriteText =   NULL;
		TimeRecording->MouseMoveEvent = NULL;
		TimeRecording->Create();
		MySpriteManager->AddSprite(TimeRecording);
	
	 KBRecording = new CTextSprite(myhdc);
		KBRecording->resOn = 99;
		KBRecording->resOver =0;
		KBRecording->resClick = 99;
		KBRecording->SetVisible(true);
		KBRecording->SetResizeable(false);
			KBRecording->resize_margin = 0;
			KBRecording->min_resize = 0;
		KBRecording->SetTransparent(false);
		KBRecording->transparentColor = RGB(255,0,0);
		KBRecording->_hInst = MySpriteManager->mylibrary;;
		apoint.x = 154;
		apoint.y = 31;
		KBRecording->SetPosition(&apoint);
		KBRecording->m_spWebBrowser2 = NULL;
		KBRecording->ClickEventUp =NULL;
		KBRecording->changeiconOver = false;
		KBRecording->SpriteText =   NULL;
		KBRecording->MouseMoveEvent = NULL;
		KBRecording->Create();
		MySpriteManager->AddSprite(KBRecording);

		ZeroMemory(TimeRecording->CurrentCaption,sizeof(TimeRecording->CurrentCaption));
		wsprintf(TimeRecording->CurrentCaption,L"Time: %02d:%02d:%02d.%02d",(segundos/60/60),( segundos/60)%60,segundos % 60,0);
		
		ZeroMemory(KBRecording->CurrentCaption,sizeof(KBRecording->CurrentCaption));
		wsprintf(KBRecording->CurrentCaption,L"%d KBytes",0);


}






// Global Variables:

#define WM_TRAYICONCLICKED WM_USER +100
#define STRRIGHT L"To use the " MRecorder L" click here"

#define SPRITECLASS L"SpriteManager"

void ShowMyPopupMenu(HWND hwnd);

void clear_Opacity(void)
{
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_OPAQUE,MF_UNCHECKED|MF_BYCOMMAND);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_90,MF_UNCHECKED|MF_BYCOMMAND);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_70,MF_UNCHECKED|MF_BYCOMMAND);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_50,MF_UNCHECKED|MF_BYCOMMAND);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_30,MF_UNCHECKED|MF_BYCOMMAND);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_10,MF_UNCHECKED|MF_BYCOMMAND);

}


void SetOpacity(int porc)
{
		SetLayeredWindowAttributes(Global_hWnd,RGB(255,255,255),255*porc/100,LWA_ALPHA);
		WriteRegistryOpacity(porc);
}

void SetMenuOpacity(int porc)
{
	clear_Opacity();

	switch (porc) 
	{
	case 100:
		::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_OPAQUE,MF_CHECKED|MF_BYCOMMAND);
		break;
	case 90:
		::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_90,MF_CHECKED|MF_BYCOMMAND);
		break;
	case 70:
		::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_70,MF_CHECKED|MF_BYCOMMAND);
		break;
	case 50:
		::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_50,MF_CHECKED|MF_BYCOMMAND);
		break;
	case 30:
		::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_30,MF_CHECKED|MF_BYCOMMAND);
		break;
	case 10:
		::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_10,MF_CHECKED|MF_BYCOMMAND);
		break;
	}



}



// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK	EditConv(HWND, UINT, WPARAM, LPARAM);


LRESULT CALLBACK	Registration(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	RecOptions(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	UploadOptions(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK	USBOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK PlayList(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProcSprite(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	HANDLE Mutex;
	Mutex =CreateMutex(NULL,true,MRecorder); 
	if (((Mutex != 0) && (GetLastError() == 0))==false)
	{
		ShowWindow(FindWindow(MRecorder,NULL),SW_SHOW);
		SetForegroundWindow(FindWindow(MRecorder,NULL));
		TerminateProcess(GetCurrentProcess(),0);
	}					    


	MSG msg;

	REGDATA aoptions;
	ReadRegistration(&aoptions);
	//MySkypeApi.Initializate();
	
		HACCEL hAccelTable;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VOIPRECORDER, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_URL2, szURL2Deniz, MAX_LOADSTRING);
	initialize_first ();


	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	GetInipath();
	WCHAR * acommand = lpCmdLine;
	
	
	
	if (wcslen(lpCmdLine) != 0)
	{
		ShowWindow(Global_hWnd,SW_HIDE);
	}
	else
	{
		ShowWindow(Global_hWnd,SW_SHOW);
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_VOIPRECORDER);

	IconData.cbSize = sizeof(IconData);
	IconData.hWnd = Global_hWnd;
	IconData.uID = 5;
	IconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	IconData.uCallbackMessage = WM_TRAYICONCLICKED;
	IconData.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_SMALL);
	
	memcpy(IconData.szTip,STRRIGHT,wcslen(STRRIGHT)*2);
	Shell_NotifyIcon(NIM_ADD, &IconData);

	SetTimer(Global_hWndhWndSprite,100,50,NULL);

	HWND awindow = FindWindow(MRecorderService, NULL);
	SendMessage(awindow,MSG_GETSOURCE,0,0);

	bool existe = ReadRegistryAutorecord();
	if (existe)
	{
		::CheckMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOSTARTRECORDING,MF_CHECKED|MF_BYCOMMAND);
	}
	else
	{
		::CheckMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOSTARTRECORDING,MF_UNCHECKED|MF_BYCOMMAND);
	}

	existe = ReadRegistryAutoUpload();
	if (existe)
	{
		::CheckMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOUPLOAD,MF_CHECKED|MF_BYCOMMAND);
	}
	else
	{
		::CheckMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOUPLOAD,MF_UNCHECKED|MF_BYCOMMAND);
	}

	existe = ReadRegistryShowPlaylist();
	if (existe)
	{
		::CheckMenuItem(GetMenu(Global_hWnd),ID_TOOLS_PLAYLIST,MF_CHECKED|MF_BYCOMMAND);
	}
	else
	{
		::CheckMenuItem(GetMenu(Global_hWnd),ID_TOOLS_PLAYLIST,MF_UNCHECKED|MF_BYCOMMAND);
	}


	Global_aplaylist = new CPlaylist();

	ReadRegistration(&aoptions);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	Shell_NotifyIcon(NIM_DELETE, &IconData);

	return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//

ATOM MyRegisterClass(HINSTANCE hInstance)
{

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_SMALL);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_VOIPRECORDER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	WNDCLASSEX wcexSprite;
	wcexSprite.cbSize		 = sizeof(WNDCLASSEX); 
	wcexSprite.style		 = CS_OWNDC;
	wcexSprite.lpfnWndProc	 = (WNDPROC)WndProcSprite;
	wcexSprite.cbClsExtra	 = 0;
	wcexSprite.cbWndExtra	 = 0;
	wcexSprite.hInstance	 = hInstance;
	wcexSprite.hIcon		 = NULL;
	wcexSprite.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wcexSprite.hbrBackground = (HBRUSH)(0);
	wcexSprite.lpszMenuName	 = NULL;
	wcexSprite.lpszClassName = SPRITECLASS;
	wcexSprite.hIconSm		 = NULL;
	wcexSprite.hIcon		 = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	RegisterClassEx(&wcexSprite);



	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
bool is_gaugevisible(void)
{
	return ReadRegistryGauge();
}

void SetPositionWindows(void)
{
	if (is_gaugevisible())
	{
		SetWindowPos(Global_hWnd,HWND_TOP,-1, -1, 275, 170,SWP_SHOWWINDOW|SWP_NOMOVE);
		SetWindowPos(Global_hWndhWndSprite,HWND_TOP,-1, -1, 300, 105,SWP_SHOWWINDOW);
		SetWindowPos(Global_STATIC,HWND_TOP,0, 104, 100, 105,SWP_SHOWWINDOW);
		SetWindowPos(Global_STATICSource,HWND_TOP,100, 104, 200, 105,SWP_SHOWWINDOW);
		CheckMenuItem(GetMenu(Global_hWnd),ID_VIEW_GAUGE,MF_CHECKED|MF_BYCOMMAND);
	}
	else
	{
		SetWindowPos(Global_hWnd,HWND_TOP,-1, -1, 275, 170-50,SWP_SHOWWINDOW|SWP_NOMOVE);
		SetWindowPos(Global_hWndhWndSprite,HWND_TOP,-1, -1, 300, 105-50,SWP_SHOWWINDOW);
		SetWindowPos(Global_STATIC,HWND_TOP,0, 104-50, 100, 105,SWP_SHOWWINDOW);
		SetWindowPos(Global_STATICSource,HWND_TOP,100, 104-50, 200, 105,SWP_SHOWWINDOW);
		CheckMenuItem(GetMenu(Global_hWnd),ID_VIEW_GAUGE,MF_UNCHECKED|MF_BYCOMMAND);
	}



}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance; // Store instance handle in our global variable
	
INITCOMMONCONTROLSEX InitCtrlEx;


	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC  = ICC_DATE_CLASSES;
	InitCommonControlsEx(&InitCtrlEx);

	MySpriteManager = new CSpriteManager();
	MySpriteManager->mylibrary = hInstance;

	DoPing();
  	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW &~WS_MAXIMIZEBOX &~WS_THICKFRAME ,
	::GetSystemMetrics(SM_CXSCREEN)-300-120-50, GetSystemMetrics(SM_CYSCREEN)-300-100, 275, 170, NULL, NULL, hInstance, NULL);
   
	Global_hWnd = hWnd;

    SetWindowLong(Global_hWnd,GWL_EXSTYLE, GetWindowLong(Global_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(Global_hWnd,0,255,LWA_ALPHA);
	int apor;
	apor= ReadRegistryOpacity();
	SetOpacity(apor);
	SetMenuOpacity(apor);

	Global_hWndhWndSprite = CreateWindow(SPRITECLASS, NULL, WS_CHILD|WS_VISIBLE ,
      -1, -1, 300, 105-50, Global_hWnd, NULL, hInstance, NULL);
	
	Global_STATIC = CreateWindow(L"STATIC", NULL, WS_CHILD|WS_VISIBLE ,
      0, 104-50, 100, 105, Global_hWnd, NULL, hInstance, NULL);

	Global_STATICSource = CreateWindow(L"STATIC", NULL, WS_CHILD|WS_VISIBLE ,
      100, 104-50, 200, 105, Global_hWnd, NULL, hInstance, NULL);
	SetPositionWindows();
	InitSprite();
	hFont = CreateFont(13, 0
							, 0, 0,
							400,
							FALSE,
							FALSE,
							FALSE,
							DEFAULT_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY,
							FF_DONTCARE ,
							L"Tahoma"
						  ); 

	SendMessage (Global_STATIC, WM_SETFONT, (LONG) hFont, TRUE);
	SendMessage (Global_STATICSource, WM_SETFONT, (LONG) hFont, TRUE);

	SetWindowText(Global_STATIC,L" Status: Ready.");
	SetWindowText(Global_STATICSource,L"No Source");

   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_RECORD,MF_ENABLED|MF_BYCOMMAND);
   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_PAUSE,MF_GRAYED|MF_BYCOMMAND);
   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_STOP,MF_GRAYED|MF_BYCOMMAND);

   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_EQUALIZE,MF_GRAYED|MF_BYCOMMAND);
   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_SUBMIT,MF_GRAYED|MF_BYCOMMAND);
   ::EnableMenuItem(GetMenu(Global_hWnd),ID_COMMAND_ATACHTOEMAIL,MF_GRAYED|MF_BYCOMMAND);

   return TRUE;
}

	
	
	
	

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	POINT apoint;
	bool ver;
	bool	existe;
	switch (message) 
	{
		case MSG_SHOWDASHBOARD:
			goto view;
		break;

		case WM_ACTIVATE:
			apoint.x = -1; 
			apoint.y = -1;
			MySpriteManager->ProcessMouseMove(&apoint);	
		break;

		case WM_MOUSEMOVE:
			apoint.x = -1; 
			apoint.y = -1;
			MySpriteManager->ProcessMouseMove(&apoint);	
		break;

		case MSG_SIZEKB:
			{
				factor_grabacion = wParam/1024;
			}
		break;


		case MSG_NEWAPP:
			{
				//::MessageBox(0,L"Interface",L"MSG_NEWAPP",16);
			}
		break;
		case MSG_MICBEGIN:
			Global_pid = wParam;
			PrintProcessNameAndID(Global_pid);
			Position_array = lParam;
			existe = ReadRegistryAutorecord();
			if (existe)
			{
				ClickRecord(NULL);
			}
			// chequear si esta gravando automaticamente
			

		break;
		
		case MSG_MICEND:
			Global_pid = -1;
			SetWindowText(Global_STATICSource,L"No Source");
			existe = ReadRegistryAutorecord();
			if (existe)
			{
				ClickStop(NULL);
			};
			
/*			existe = ReadRegistryAutoUpload();
			// hacer autoupload aca

			existe = ReadRegistryShowPlaylist();
			// hacer show playlist
*/
			break;

		case MSG_SPEAKERDATA:	
			Gauge_Speaker->porcentaje = wParam;
			break;

		case MSG_MICDATA:
			Gauge_Mic->porcentaje = wParam;
		break;


	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_VOIPRECORDER_DIALOG, hWnd, (DLGPROC)About);
			break;
		
		case IDM_HELPER:
			//aplaylist->InitFile();
			RECORD_PLAYLIST arecord;
			ZeroMemory(&arecord,sizeof(arecord));
			wcscat(arecord.FileName,L"test.11");
			FILETIME myFiletime;
			SYSTEMTIME atime;
			GetLocalTime(&atime);
			SystemTimeToFileTime(&atime,&myFiletime);
			arecord.datetime.dwHighDateTime = myFiletime.dwHighDateTime;
			arecord.datetime.dwLowDateTime = myFiletime.dwLowDateTime;
	

			Global_aplaylist->Insert_Record(&arecord);

			break;

		
		case ID_HELP_REGISTER:
		case ID_MENU_REGISTRATIONCODE:		
			DialogBox(hInst, (LPCTSTR)IDD_REGISTRATIONFORM, hWnd, (DLGPROC)Registration);
			break;

		case ID_TOOLS_CONFIGURERECORDINGDEVICES:
		case ID_MENU_RECORDINGDEVICES:		
			DialogBox(hInst, (LPCTSTR)IDD_DIALOG_USB, hWnd, (DLGPROC)USBOptions);
			break;

		case ID_MENU_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_VOIPRECORDER_DIALOG, hWnd, (DLGPROC)About);
			break;

		case ID_VIEW_GAUGE:
			ver = ReadRegistryGauge();
			WriteRegistryGauge(!ver);
			::SetPositionWindows();
			break;
			
		case ID_TOOLS_RECOPTIONS:
			DialogBox(hInst, (LPCTSTR)IDD_DIALOG_RECORDING, hWnd, (DLGPROC)RecOptions);
			break;

		case ID_TOOLS_UPLOADSOPTIONS:
			DialogBox(hInst, (LPCTSTR)IDD_DIALOG_UPLOAD, hWnd, (DLGPROC)UploadOptions);
			break;

		case ID_MENU_PLAYLIST:
		case ID_VIEW_PLAYLIST:
			//DialogBox(hInst, (LPCTSTR)IDD_PLAYLIST, hWnd, (DLGPROC)About);
view:
			HWND aeasyPlay;
			aeasyPlay= FindWindow(NULL,MRecorder L" DashBoard");
			if (aeasyPlay==NULL)
			{
				//Global_PlayList = CreateDialog(hInst, (LPCTSTR)IDD_PLAYLIST, Global_hWnd, (DLGPROC)PlayList);
				Global_PlayList = CreateDialog(hInst, (LPCTSTR)IDD_PLAYLIST, 0, (DLGPROC)PlayList);

			}
			else
			{
				ShowWindow(Global_PlayList,SW_SHOW);
				ShowWindow(Global_PlayList,SW_RESTORE);
				SetForegroundWindow(Global_PlayList);
			}


			//SetWindowPos(Global_PlayList,
			ShowWindow(Global_PlayList,SW_SHOW);
			break;
		case ID_RIGHTMENU_GOTOFOLDER:
				WCHAR afile[512];
				ZeroMemory(afile,sizeof(afile));
				ReadRegistryDestinationPath(afile);
				ShellExecute(0,L"open",afile,NULL,NULL,SW_SHOW);
				break;

		case ID_RIGHTMENU_GOTOEXFOLDER:
				ZeroMemory(afile,sizeof(afile));
				ReadRegistryExportPath(afile);
				ShellExecute(0,L"open",afile,NULL,NULL,SW_SHOW);
				break;

		case ID_MENU_EXIT:
			DestroyWindow(hWnd);
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		case ID_FILE_HIDE:
				ShowWindow(hWnd,SW_HIDE);
			break;

		case ID_MENU_SHOW:
			if (IsWindowVisible(hWnd))
			{
				ShowWindow(hWnd,SW_HIDE);
			}
			else
			{
				ShowWindow(hWnd,SW_SHOW);
			}
		break;

		case ID_OPACITY_OPAQUE:
			clear_Opacity();
			SetOpacity(100);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_OPAQUE,MF_CHECKED|MF_BYCOMMAND);
			break;
		
		case ID_OPACITY_90:
			clear_Opacity();
			SetOpacity(90);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_90,MF_CHECKED|MF_BYCOMMAND);

			break;
		case ID_OPACITY_70:
			clear_Opacity();
			SetOpacity(70);

			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_70,MF_CHECKED|MF_BYCOMMAND);
		break;
		case ID_OPACITY_50:
			clear_Opacity();
			SetOpacity(50);
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_50,MF_CHECKED|MF_BYCOMMAND);
		break;
		case ID_OPACITY_30:
			SetOpacity(30);
			clear_Opacity();
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_30,MF_CHECKED|MF_BYCOMMAND);
			break;
		case ID_OPACITY_10:
			SetOpacity(10);
			clear_Opacity();
			::CheckMenuItem(GetMenu(Global_hWnd),ID_OPACITY_10,MF_CHECKED|MF_BYCOMMAND);
		break;

	

		case ID_MENU_HELP:
			ShellExecute(hWnd,L"Open",L"http://www.easyvoiprecorder.com/index.php?option=com_content&task=view&id=19&Itemid=47",NULL,NULL,SW_SHOW);
			break;
		case ID_HELP_HELPTOPICS:
			ShellExecute(hWnd,L"Open",L"http://www.easyvoiprecorder.com/index.php?option=com_content&task=view&id=19&Itemid=47",NULL,NULL,SW_SHOW);
			break;

		case ID_MENU_BUY:
			ShellExecute(hWnd,L"Open",L"http://www.easyvoiprecorder.org/index.php?option=com_content&task=view&id=14&Itemid=28",NULL,NULL,SW_SHOW);

			break;

		case ID_MENU_CHECKFORUPDATES:
				chek4update(hWnd);
			break;
			

		case ID_COMMAND_RECORD:
			ClickRecord(NULL);
			break;

		case ID_COMMAND_PAUSE:
			ClickPause(NULL);
			//MessageBox(hWnd,L"Pause",L"-",0);
			break;

		case ID_RUNATSTARTUP:
			bool existe;
			existe = ReadRegistryRunatStart();
			WriteRegistryRunatStart(!existe,hInst);
			break;


		case ID_COMMAND_STOP:
			ClickStop(NULL);
			//MessageBox(hWnd,L"Stop",L"-",0);
			break;
			
		
		case ID_COMMAND_AUTOSTARTRECORDING:
			existe = ReadRegistryAutorecord();
			WriteRegistryAutorecord(!existe);
			if (!existe)
			{
				::CheckMenuItem(GetMenu(hWnd),ID_COMMAND_AUTOSTARTRECORDING,MF_CHECKED|MF_BYCOMMAND);
			}
			else
			{
				::CheckMenuItem(GetMenu(hWnd),ID_COMMAND_AUTOSTARTRECORDING,MF_UNCHECKED|MF_BYCOMMAND);
			}

			break;

		case ID_COMMAND_AUTOUPLOAD:
			existe = ReadRegistryAutoUpload();
			WriteRegistryAutoUpload(!existe);
			if (!existe)
			{
				::CheckMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOUPLOAD,MF_CHECKED|MF_BYCOMMAND);
			}
			else
			{
				::CheckMenuItem(GetMenu(Global_hWnd),ID_COMMAND_AUTOUPLOAD,MF_UNCHECKED|MF_BYCOMMAND);
			}
			break;

			
		case ID_TOOLS_PLAYLIST:
			existe = ReadRegistryShowPlaylist();
			WriteRegistryShowPlaylist(!existe);
			if (!existe)
			{
				::CheckMenuItem(GetMenu(Global_hWnd),ID_TOOLS_PLAYLIST,MF_CHECKED|MF_BYCOMMAND);
			}
			else
			{
				::CheckMenuItem(GetMenu(Global_hWnd),ID_TOOLS_PLAYLIST,MF_UNCHECKED|MF_BYCOMMAND);
			}
			break;




		case ID_COMMAND_EQUALIZE:
			MessageBox(hWnd,L"ID_COMMAND_EQUALIZE",L"-",0);
			break;

		case ID_COMMAND_SUBMIT:
			MessageBox(hWnd,L"ID_COMMAND_SUBMIT",L"-",0);
			break;
			
		case ID_COMMAND_ATACHTOEMAIL:
			MessageBox(hWnd,L"ID_COMMAND_ATACHTOEMAIL",L"-",0);
			break;
			
			

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;


	case  WM_TRAYICONCLICKED:
			 if (lParam == WM_RBUTTONUP)
			 {
				ShowMyPopupMenu(hWnd);
			};
			 if (lParam == WM_LBUTTONUP)
			 {
				if (IsWindowVisible(Global_hWnd)==false)
				{
					ShowWindow(Global_hWnd,SW_SHOW);
				}
				else
				{
					::SetForegroundWindow(Global_hWnd);				
				};
			};

			 break;
	case WM_QUIT:
	//		ShowWindow(hWnd,SW_HIDE);
			break;

	case WM_SYSCOMMAND:
		{
			if ((wParam ==SC_MINIMIZE) ||(wParam ==SC_CLOSE  ) )
			{
					ShowWindow(hWnd,SW_HIDE);
					break;
			}
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



void InsertPhones(WCHAR* alista , HWND hListBox)
{

	WCHAR* comparar;
	size_t nexpos = 0;
	comparar = alista;	
	while (true)
	{
		comparar = &comparar[nexpos];	
		if (comparar[0]==0x00) 
		{
			break;
		};
		SendMessage( hListBox, LB_ADDSTRING, 0, (LPARAM) comparar);

		nexpos= wcslen(&comparar[0])+1;
	};
	return ;
}

LRESULT CALLBACK	EditConv(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hListBox;
	HFONT hOrigFont;
	LOGFONT lf;
	WCHAR miversion[512];
	switch (message)
	{
	case WM_INITDIALOG:
		// buscar los datos/
		// mostrar 
		RECORD_PLAYLIST arecordplaylist;
		ZeroMemory(&arecordplaylist,sizeof(arecordplaylist));
		wcscat(arecordplaylist.FileName,filenameRecord);
		Global_aplaylist->Search_RecordByFileName(&arecordplaylist);
		
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_DESTINATION),arecordplaylist.Destination);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COMMENT),arecordplaylist.Comments);
		//SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COMMENT),arecordplaylist.Source);
		
		break;

	case WM_CTLCOLORSTATIC:
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}

		if (LOWORD(wParam) == IDOK) 
		{
			RECORD_PLAYLIST arecordplaylist;
			ZeroMemory(&arecordplaylist,sizeof(arecordplaylist));
			wcscat(arecordplaylist.FileName,filenameRecord);
			Global_aplaylist->Search_RecordByFileName(&arecordplaylist);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_DESTINATION),arecordplaylist.Destination,sizeof(arecordplaylist.Destination));
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_COMMENT),arecordplaylist.Comments,sizeof(arecordplaylist.Comments));

			Global_aplaylist->Modify_Record(&arecordplaylist);
			EndDialog(hDlg, LOWORD(wParam));
			
			return TRUE;
		}

		break;
	}
	return FALSE;

}


// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hListBox;
	HFONT hOrigFont;
	LOGFONT lf;
	WCHAR miversion[512];
	switch (message)
	{
	case WM_INITDIALOG:
		SendMessage(hDlg,WM_SETICON,ICON_BIG,(LPARAM)LoadIcon(hInst, (LPCTSTR)IDI_SMALL));
		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT); // Set style

		ZeroMemory(alista,sizeof(alista));
		GetPrivateProfileSectionW(
		L"Supported",
		alista,
		1024*1024,
		aini);
		hListBox = ::GetDlgItem(hDlg, IDC_LIST_VOIPPHONES);
		InsertPhones(alista,hListBox);

		hOrigFont= (HFONT) SendMessage(GetDlgItem(hDlg,IDC_STATIC_WEB), WM_GETFONT, 0, 0);
		GetObject(hOrigFont, sizeof(lf), &lf);
		lf.lfUnderline = TRUE;
		hFont= CreateFontIndirect(&lf);
		SendMessage (GetDlgItem(hDlg,IDC_STATIC_WEB), WM_SETFONT, (LONG) hFont, TRUE);
		SendMessage (GetDlgItem(hDlg,IDC_STATIC_EMAIL), WM_SETFONT, (LONG) hFont, TRUE);
		ZeroMemory(miversion,1024);
		ReadVersion(miversion);
		WCHAR Aux[512];
		ZeroMemory(Aux,1024);
		if (isRegistered())
		{
			wsprintf(Aux,L"Registered Version: %s",miversion);
		}
		else
		{
			wsprintf(Aux,L"UnRegistered Version: %s",miversion);
		
		}

		SetWindowText(GetDlgItem(hDlg,IDC_STATIC_VER),Aux);

		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_LINK),destinationshare);

		return TRUE;
		break;

	case WM_CTLCOLORSTATIC:
		if (((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_WEB)) || ((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_EMAIL)))
		{
			HDC hdcStatic;
			hdcStatic= (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 255));
			SetBkMode(hdcStatic, TRANSPARENT);
			return (LONG)GetSysColorBrush(COLOR_3DFACE);
				
		};
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}

		if (LOWORD(wParam)==IDC_STATIC_WEB) 
		{
			ShellExecute(hDlg,L"Open",L"http://www.easyvoiprecorder.com",NULL,NULL,SW_SHOW);
		return true;
		}
		if (LOWORD(wParam)==IDLINK) 
		{
			OpenClipboard(hDlg);
			EmptyClipboard();
			HGLOBAL amem = GlobalAlloc(GMEM_MOVEABLE,(wcslen(destinationshare)+1)*2);
			WCHAR* lptstrCopy = (WCHAR*)GlobalLock(amem); 
			ZeroMemory(lptstrCopy,(wcslen(destinationshare)+1)*2);
			wcscat(lptstrCopy,destinationshare);
			GlobalUnlock(amem); 
			SetClipboardData(CF_UNICODETEXT, amem);
			CloseClipboard();
			return true;
		}


		if (LOWORD(wParam)==IDC_STATIC_EMAIL) 
		{
			ShellExecute(hDlg,L"Open",L"mailto://sales@easyvoiprecorder.com",NULL,NULL,SW_SHOW);
			return true;
		}
		

		break;
	}
	return FALSE;
}



void ShowDataInDialogRecOptions(HWND hDlg, PRECORDINGOPTIONS arecoptions )
{
			WCHAR auxText[512];

			//ZeroMemory(auxText,1024);
			//wsprintf(auxText,L"%u",arecoptions->KHZ);
			//SetWindowText(GetDlgItem(hDlg,IDC_EDIT_KHZ),auxText);
			//ZeroMemory(auxText,1024);
			//wsprintf(auxText,L"%u",arecoptions->Format);
			//SetWindowText(GetDlgItem(hDlg,IDC_COMBO_FORMAT),auxText);
			//ZeroMemory(auxText,1024);
			//wsprintf(auxText,L"%u",arecoptions->BitRate);
			//SetWindowText(GetDlgItem(hDlg,IDC_COMBO_BITRATE),auxText);			
			//SendMessage ( GetDlgItem(hDlg,IDC_COMBO_BITRATE), CB_ADDSTRING, 0,(LPARAM) L"First string added.") ;
			//SendMessage ( GetDlgItem(hDlg,IDC_COMBO_BITRATE), CB_SETCURSEL, 0,(LPARAM) 0) ;

			int i=0;
			SendMessage(GetDlgItem(hDlg,IDC_COMBO_FORMAT),
								CB_RESETCONTENT,0,0);
			do
			{
				SendMessage(GetDlgItem(hDlg,IDC_COMBO_FORMAT),
									CB_ADDSTRING,
									0,
									reinterpret_cast<LPARAM>((WCHAR*)ComboBoxItems_encoding[i]));
				i++;
			}
			while (ComboBoxItems_encoding[i] != NULL);
			SendMessage ( GetDlgItem(hDlg,IDC_COMBO_FORMAT), CB_SETCURSEL, (WPARAM)arecoptions->Format,(LPARAM) 0) ;

			i=0;
			SendMessage(GetDlgItem(hDlg,IDC_COMBO_BITRATE),
								CB_RESETCONTENT,0,0);

			do
			{
				SendMessage(GetDlgItem(hDlg,IDC_COMBO_BITRATE),
									CB_ADDSTRING,
									0,
									reinterpret_cast<LPARAM>((WCHAR*)ComboBoxItems_encoding_quality[i]));
				i++;
			}
			while (ComboBoxItems_encoding_quality[i] != NULL);
	

			SendMessage ( GetDlgItem(hDlg,IDC_COMBO_BITRATE), CB_SETCURSEL, (WPARAM)arecoptions->BitRate,(LPARAM) 0) ;
			
			i=0;
			SendMessage(GetDlgItem(hDlg,IDC_EDIT_KHZ),
								CB_RESETCONTENT,0,0);

			do
			{
				SendMessage(GetDlgItem(hDlg,IDC_EDIT_KHZ),
									CB_ADDSTRING,
									0,
									reinterpret_cast<LPARAM>((WCHAR*)ComboBoxItems_encoding_quality[i]));
				i++;
			}
			while (ComboBoxItems_encoding_quality[i] != NULL);
	

			SendMessage ( GetDlgItem(hDlg,IDC_EDIT_KHZ), CB_SETCURSEL, (WPARAM)arecoptions->KHZ,(LPARAM) 0) ;


};


LRESULT CALLBACK RecOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hListBox;
	HFONT hOrigFont;
	LOGFONT lf;
	RECORDINGOPTIONS arecoptions;


	switch (message)
	{	
	case WM_INITDIALOG:
		ReadRegistryRecordingOptions(&arecoptions);
		ShowDataInDialogRecOptions(hDlg,&arecoptions);
		return TRUE;
		break;

	case WM_CTLCOLORSTATIC:
		if (((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_WEB)) || ((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_EMAIL)))
		{
			HDC hdcStatic;
			hdcStatic= (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 255));
			SetBkMode(hdcStatic, TRANSPARENT);
			return (LONG)GetSysColorBrush(COLOR_3DFACE);
				
		};
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) 
		{
			//IDC_EDIT_KHZ
			//IDC_COMBO_FORMAT
			//IDC_COMBO_BITRATE
			arecoptions.KHZ = SendMessage(GetDlgItem(hDlg,IDC_EDIT_KHZ),CB_GETCURSEL,0,0);
			arecoptions.Format = SendMessage(GetDlgItem(hDlg,IDC_COMBO_FORMAT),CB_GETCURSEL,0,0);
			arecoptions.BitRate = SendMessage(GetDlgItem(hDlg,IDC_COMBO_BITRATE),CB_GETCURSEL,0,0);
	
			WriteRegistryRecordingOptions(&arecoptions);
			EndDialog(hDlg, LOWORD(wParam));

			//arecoptions.BitRate =arecoptions.BitRate+1; 
			return TRUE;
		};
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
		}
		return TRUE;
		break;
	}
	return FALSE;
}

void ShowDataInDialogUploadOptions(HWND hDlg, PUPLOADOPTIONS arecoptions )
{
	// IDC_COMBO_TRANSFER

	// IDC_EDIT_IP
	// IDC_EDIT_USERNAME 
	// IDC_EDIT_PASS

	WCHAR auxText[512];

	ZeroMemory(auxText,1024);
	wsprintf(auxText,L"%s",arecoptions->DOMAIN_NAME);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_IP),auxText);
/*
	ZeroMemory(auxText,1024);
	wsprintf(auxText,L"%s",arecoptions->USERNAME);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_USERNAME),auxText);

	ZeroMemory(auxText,1024);
	wsprintf(auxText,L"%s",arecoptions->PASSWORD);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PASS),auxText);
*/
	
	SendMessage(GetDlgItem(hDlg,IDC_COMBO_TRANSFER),
                        CB_RESETCONTENT,0,0);

	int i=0;
	do
	{
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_TRANSFER),
							CB_ADDSTRING,
							0,
							reinterpret_cast<LPARAM>((WCHAR*)ComboBoxItems_upload[i]));
		i++;
	}
	while (ComboBoxItems_upload[i] != NULL);
	

	SendMessage ( GetDlgItem(hDlg,IDC_COMBO_TRANSFER), CB_SETCURSEL, (WPARAM)0,(LPARAM) 0) ;


};




LRESULT CALLBACK	UploadOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hListBox;
	HFONT hOrigFont;
	LOGFONT lf;
	UPLOADOPTIONS auploadoption;
	switch (message)
	{	
	case WM_INITDIALOG:
		ZeroMemory(&auploadoption,sizeof(auploadoption));
		ReadRegistryUploadOptions(&auploadoption);
		ShowDataInDialogUploadOptions(hDlg,&auploadoption);
		return TRUE;
		break;

	case WM_CTLCOLORSTATIC:
		if (((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_WEB)) || ((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_EMAIL)))
		{
			HDC hdcStatic;
			hdcStatic= (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 255));
			SetBkMode(hdcStatic, TRANSPARENT);
			return (LONG)GetSysColorBrush(COLOR_3DFACE);
				
		};
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL )
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		};

		if (LOWORD(wParam) == IDOK )
			
		{
			// IDC_COMBO_TRANSFER
			// IDC_EDIT_IP
			// IDC_EDIT_USERNAME 
			// IDC_EDIT_PASS
			//ReadRegistryUploadOptions(&auploadoption);
			ZeroMemory(&auploadoption,sizeof(auploadoption));
			
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_IP),auploadoption.DOMAIN_NAME,512);
			//GetWindowText(GetDlgItem(hDlg,IDC_EDIT_USERNAME),auploadoption.USERNAME,512);
			//GetWindowText(GetDlgItem(hDlg,IDC_EDIT_PASS),auploadoption.PASSWORD,512);
			
			auploadoption.METHOD =  SendMessage(GetDlgItem(hDlg,IDC_COMBO_TRANSFER), CB_GETCURSEL,0,0);
			
			if (SendSerialSharing(hDlg,auploadoption.DOMAIN_NAME))
			{
				WriteRegistryUploadOptions(&auploadoption);
				EndDialog(hDlg, LOWORD(wParam));
			};
			return TRUE;
		};

		break;
	}
	return FALSE;
}



void ShowMyPopupMenu(HWND hwnd)
{
  HMENU menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU));
  HMENU subMenu = GetSubMenu(menu, 0);
  POINT pt;
  ::GetCursorPos(&pt);
  ::SetForegroundWindow(hwnd);
  bool existe = ReadRegistryRunatStart();
  if (existe)
  {
	  ::CheckMenuItem(menu,ID_RUNATSTARTUP,MF_CHECKED|MF_BYCOMMAND);
  }
  else
  {
	  ::CheckMenuItem(menu,ID_RUNATSTARTUP,MF_UNCHECKED|MF_BYCOMMAND);
  }
  TrackPopupMenu(subMenu, TPM_RIGHTALIGN | TPM_VCENTERALIGN | TPM_RIGHTBUTTON, pt.x, pt.y-100, 0, hwnd, NULL);
  DestroyMenu(menu);
}

void ShowMyPopupMenuRight(HWND hwnd)
{
  HMENU menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_LIST));
  HMENU subMenu = GetSubMenu(menu, 0);
  POINT pt;
  ::GetCursorPos(&pt);
  ::SetForegroundWindow(hwnd);
  TrackPopupMenu(subMenu, TPM_RIGHTALIGN | TPM_VCENTERALIGN | TPM_RIGHTBUTTON, pt.x+140, pt.y +40, 0, hwnd, NULL);
  DestroyMenu(menu);
}


LRESULT CALLBACK WndProcSprite(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_MOUSEMOVE:
		POINT apoint;
		apoint.x = LOWORD(lParam); 
		apoint.y = HIWORD(lParam);
		MySpriteManager->ProcessMouseMove(&apoint);	
		break;

	case WM_LBUTTONUP:
		//POINT apoint;
		apoint.x = LOWORD(lParam); 
		apoint.y = HIWORD(lParam);
		MySpriteManager->ProcessMouseClickUp(&apoint);	
		break;
	case WM_TIMER:
			if (wParam ==100)
			{
				if (MySpriteManager != NULL)
				{
					HDC hdc =GetDC(Global_hWndhWndSprite);	
					MySpriteManager->pintar(hdcCompatibleOffscreen);
					BitBlt(hdc, 0, 0, 300-10, 104,hdcCompatibleOffscreen, 1, 3, SRCCOPY);
					ReleaseDC(Global_hWndhWndSprite,hdc);
				}
			}
			
			if (wParam ==97)
			{
				KillTimer(Global_hWndhWndSprite,97);
				REGDATA aoptions;
				ReadRegistration(&aoptions);
				if (lstrcmpi(aoptions.CODE,L"")!=0)
				{
					DWORD oldprotect;
					VirtualProtect(function, 30, PAGE_EXECUTE_READWRITE, &oldprotect);
					char codes[6]; 
					codes[5]=0x00;
					memcpy(codes,aoptions.STRING,5);

					functionpatch(codes,(char*)function);
					DWORD oldprotect2;
					VirtualProtect(function, 30, oldprotect, &oldprotect2);
				
				}

			}


			if (wParam ==99)
			{
				KillTimer(Global_hWndhWndSprite,99);
				function(NULL, NULL,NULL);
			}
			if (wParam ==98)
			{
				if (MySpriteManager != NULL)
				{
					if (!paused)
					{
						segundos++;

						ZeroMemory(TimeRecording->CurrentCaption,sizeof(TimeRecording->CurrentCaption));
						wsprintf(TimeRecording->CurrentCaption,L"Time: %02d:%02d:%02d.%d",(segundos/10/60/60),( segundos/10/60)%60,segundos/10 % 60,segundos%10);
						
						ZeroMemory(KBRecording->CurrentCaption,sizeof(KBRecording->CurrentCaption));
						if (factor_grabacion / 1024 >=1)
						{
							wsprintf(KBRecording->CurrentCaption,L"%d.%d MBytes",factor_grabacion/1024,factor_grabacion%1024);
						}
						else
						{
							wsprintf(KBRecording->CurrentCaption,L"%d KBytes",factor_grabacion);
						
						}
					};


				}
			}

			
			if (wParam ==90)
			{
				if (MySpriteManager != NULL)
				{
					if (!paused)
					{
						if (posiI > 100)
						{
							posiI=0;
						}
						else
						{
							posiI++;
						};
					};					
					//Gauge_Mic->porcentaje = posiI;
					//Gauge_Speaker->porcentaje = posiI;
				}
			}
			break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

HMENU mymenuPlayback;

LRESULT CALLBACK PlayList(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hListBox;
	HFONT hOrigFont;
	LOGFONT lf;
	LV_COLUMN LvCol;
	LVITEM  LvItem;
	WCHAR Temp[128];
	int wmId, wmEvent;

	switch (message)
	{

	case MSG_REFRESHPLAYLIST:
		Refresh_Listview(hDlg);
		break;

	case WM_INITDIALOG:
		mymenuPlayback =  LoadMenu(hInst,MAKEINTRESOURCE(IDR_MENU_PLAYLIST));
		SetMenu(hDlg,mymenuPlayback);
		hOrigFont= (HFONT) SendMessage(GetDlgItem(hDlg,IDC_STATIC_WEB), WM_GETFONT, 0, 0);
		GetObject(hOrigFont, sizeof(lf), &lf);
		lf.lfUnderline = FALSE;
		hFont= CreateFontIndirect(&lf);
		SendMessage (GetDlgItem(hDlg,IDC_STATIC_WEB), WM_SETFONT, (LONG) hFont, TRUE);
		SendMessage (GetDlgItem(hDlg,IDC_STATIC_EMAIL), WM_SETFONT, (LONG) hFont, TRUE);
		SendMessage(hDlg,WM_SETICON,ICON_BIG,(LPARAM)LoadIcon(hInst, (LPCTSTR)IDI_SMALL));
		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT); // Set style

		Global_aplaylist->GetDestination(GetDlgItem(hDlg,IDC_COMBO_DESTINATION));
		Global_aplaylist->GetSources(GetDlgItem(hDlg,IDC_COMBO_SOURCE));
		//Global_aplaylist->GetStatus(GetDlgItem(hDlg,IDC_COMBO_STATUS));
		Global_aplaylist->GetMaxDate(GetDlgItem(hDlg,IDC_DATETIMEPICKER_TO));
		Global_aplaylist->GetMinDate(GetDlgItem(hDlg,IDC_DATETIMEPICKER_FROM));



		ZeroMemory(&LvCol,sizeof(LvCol));
		LvCol.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;    // Type of mask
		LvCol.pszText=L"Date / Time";                            // First Header Text
		LvCol.cx=0x72;                                   // width of column
		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_INSERTCOLUMN,0,(LPARAM)&LvCol); // Insert/Show the coloum
		LvCol.pszText=L"Source";                            // Next coloum
		LvCol.cx=0x60;                                   // width of column
		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_INSERTCOLUMN,1,(LPARAM)&LvCol); //
		LvCol.cx=0x60;                                   // width of column
		LvCol.pszText=L"Destination";                            //
		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_INSERTCOLUMN,2,(LPARAM)&LvCol); // ...
		LvCol.pszText=L"Comments";                            //
		LvCol.cx=0x72;                                   // width of column
		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_INSERTCOLUMN,3,(LPARAM)&LvCol); //
		LvCol.pszText=L"FileName";                            //
		LvCol.cx=0x110;                                   // width of column
		SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_INSERTCOLUMN,4,(LPARAM)&LvCol); //

		Refresh_Listview(hDlg);

/*
		m_Video =  MCIWndCreate(hDlg,hInst,MCIWNDF_NOMENU,NULL);
		SetWindowPos(m_Video,HWND_TOPMOST,7,310,0,0,SW_SHOW);
		ShowWindow(m_Video,SW_SHOW); 
*/
		return TRUE;
		break;

	case WM_CTLCOLORSTATIC:
		if (((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_WEB)) || ((HWND)lParam== GetDlgItem(hDlg,IDC_STATIC_EMAIL)))
		{
			HDC hdcStatic;
			hdcStatic= (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 255));
			SetBkMode(hdcStatic, TRANSPARENT);
			return (LONG)GetSysColorBrush(COLOR_3DFACE);
				
		};
		break;

	case WM_NOTIFY:
	{
		switch(LOWORD(wParam))
		{
			case IDC_LIST: 
				if (((LPNMHDR)lParam)->code == NM_RCLICK)
				{
					ShowMyPopupMenuRight(hDlg);
				};
				if((((LPNMHDR)lParam)->code == NM_DBLCLK) 
				|| (((LPNMHDR)lParam)->code == NM_RETURN))
				{
					WCHAR Text[255]={0};
					WCHAR Temp[255]={0};
					WCHAR Temp1[255]={0};
					int iSlected=0;
					int j=0;
					iSlected=SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
					if(iSlected==-1)
					{
						break;
					}
					memset(&LvItem,0,sizeof(LvItem));
					LvItem.mask=LVIF_TEXT;
					LvItem.iSubItem=0;
					LvItem.pszText=Text;
					LvItem.cchTextMax=256;
					LvItem.iItem=iSlected;
					SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_GETITEMTEXT, 
								iSlected, (LPARAM)&LvItem); 
					wsprintf(Temp1,Text);
						LvItem.iSubItem=4;
						SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_GETITEMTEXT, 
									iSlected, (LPARAM)&LvItem);
					SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SELECTED),LvItem.pszText);
					
					ZeroMemory(Text,sizeof(Text));
					GetFIlenameFromPlaylist(hDlg,Text);
					ShellExecute(0, L"open", Text, NULL, NULL, SW_SHOWNORMAL);
					//EjecutarProgram(Text,false,SW_SHOW);
					/*

					if (((LPNMHDR)lParam)->code == NM_DBLCLK)
					{
							ZeroMemory(Text,sizeof(Text));
							GetFIlenameFromPlaylist(hDlg,Text);
							WCHAR aux[512];
							ZeroMemory(aux,sizeof(aux));
							wcscat(aux,L"\"\\Program Files\\Windows Media Player\\wmplayer.exe ");
							wcscat(aux, L"\"");
							wcscat(aux, L"\"");
							wcscat(aux, Text);
							wcscat(aux, L"\"");
							if (ISFileExists(L"\\Program Files\\Windows Media Player\\wmplayer.exe"))
							{
								EjecutarProgram(aux,false,SW_SHOW);
							}
							else
							{
								ZeroMemory(aux,sizeof(aux));
								wcscat(aux,L"\"\\Program Files\\Windows Media Player\\mplayer2.exe");
								wcscat(aux, L"\"");
								wcscat(aux, L" \"");
								wcscat(aux, Text);
								wcscat(aux, L"\"");
								//MessageBox(0,aux,aux,0);

								EjecutarProgram(aux,false,SW_SHOW);
							}
					}
					*/


					}

				}
			break;
		}
	break;

	case WM_COMMAND:

		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 

		WCHAR afilein[512];
		WCHAR afileout[512];

		ZeroMemory(afilein,sizeof(afilein));
		ZeroMemory(afileout,sizeof(afileout));

		// Parse the menu selections:
		switch (wmId)
		{
			
			case ID_EXPORTTO_WAV:
				if (GetFIlenameFromPlaylist(hDlg,afilein))
				{
					if (ConvertFromTo(afilein,afileout,0,hDlg))
					{
						WCHAR textoau[512];
						ZeroMemory(textoau,sizeof(textoau));
						wsprintf(textoau,L"File Successfully exported to Wav. The File was created in:\n%s",afileout);
						MessageBox(	hDlg,textoau,MRecorder,64);
					};
				}
				else
				{
					MessageBox(	hDlg,L"Please Select a file to convert",MRecorder,48);
				}
			break;

			case ID_EXPORTTO_MP3:
				if (GetFIlenameFromPlaylist(hDlg,afilein))
				{
					if (ConvertFromTo(afilein,afileout,3,hDlg))
					{
						WCHAR textoau[512];
						ZeroMemory(textoau,sizeof(textoau));
						wsprintf(textoau,L"File Successfully exported to Mp3. The File was created in:\n%s",afileout);
						MessageBox(	hDlg,textoau,MRecorder,64);
					};
				}
				else
				{
					MessageBox(	hDlg,L"Please Select a file to convert",MRecorder,48);
				}

				break;

			case ID_EXPORTTO_OGG:
				if (GetFIlenameFromPlaylist(hDlg,afilein))
				{	
					if (ConvertFromTo(afilein,afileout,1,hDlg))
					{
						WCHAR textoau[512];
						ZeroMemory(textoau,sizeof(textoau));
						wsprintf(textoau,L"File Successfully exported to ogg. The File was created in:\n%s",afileout);
						MessageBox(	hDlg,textoau,MRecorder,64);

					};
				}
				else
				{
					MessageBox(	hDlg,L"Please Select a file to convert",MRecorder,48);
				}

			break;

			case ID_EXPORTTO_SPEEX:
					if (GetFIlenameFromPlaylist(hDlg,afilein))
					{
						if (ConvertFromTo(afilein,afileout,2,hDlg))
						{
							WCHAR textoau[512];
							ZeroMemory(textoau,sizeof(textoau));
							wsprintf(textoau,L"File Successfully exported to Speex. The File was created in:\n%s",afileout);
							MessageBox(	hDlg,textoau,MRecorder,64);
						};
					}
					else
					{
						MessageBox(	hDlg,L"Please Select a file to convert",MRecorder,48);
					}
			break;

			case ID_EXPORTTO_WMA:
				if (GetFIlenameFromPlaylist(hDlg,afilein))
				{
						if (ConvertFromTo(afilein,afileout,4,hDlg))
						{
							WCHAR textoau[512];
							ZeroMemory(textoau,sizeof(textoau));
							wsprintf(textoau,L"File Successfully exported to WMA. The File was created in:\n%s",afileout);
							MessageBox(	hDlg,textoau,MRecorder,64);
						};
				}
				else
				{
					MessageBox(	hDlg,L"Please Select a file to convert",MRecorder,48);
				}

			break;


			case ID_FILE_CONVERTTOMONO:
				if (GetFIlenameFromPlaylist(hDlg,afilein))
				{
						if (Convert2Mono(afilein,afileout,hDlg))
						{
							WCHAR textoau[512];
							ZeroMemory(textoau,sizeof(textoau));
							wsprintf(textoau,L"File Successfully Converted to Mono. The File was created in:\n%s",afileout);
							MessageBox(	hDlg,textoau,MRecorder,64);
						};
				}
				else
				{
					MessageBox(	hDlg,L"Please Select a file to convert to Mono",MRecorder,48);
				}

				break;

			case ID_FILE_UPLOADCONVERSATION:
				//DialogBox(hInst, (LPCTSTR)IDD_VOIPRECORDER_DIALOG, hWnd, (DLGPROC)About);
				// aca 
				if (GetFIlenameFromPlaylist(hDlg,afilein))
				{
					WCHAR Texto[512];
					ZeroMemory(Texto,sizeof(Texto));
					if (GetUploadData(Texto,destinationshare,0))
					{
						if (ConvertFromTo(afilein,afileout,3,hDlg))
						{
							
							WCHAR serialcode[512];
							WCHAR HDserial[12];
							DWORD aserial=0;
							GetMACaddress(&aserial);
							ZeroMemory(HDserial,sizeof(HDserial));
							wsprintf(HDserial,L"%lx",aserial);

							UPLOADOPTIONS aup;
							ZeroMemory(&aup,sizeof(aup));
							ReadRegistryUploadOptions(&aup);
							ZeroMemory(serialcode,sizeof(serialcode));
							wcscat(serialcode,aup.DOMAIN_NAME);

							WCHAR filenametoexecute[512];
							ZeroMemory(filenametoexecute,sizeof(filenametoexecute));
							ReadRegistryPath(filenametoexecute);
							wcscat(filenametoexecute,L"EasyVoipRecorderUpload.exe");
							wcscat(filenametoexecute,L" \"");
							wcscat(filenametoexecute,afileout);
							wcscat(filenametoexecute,L"\"");
							wcscat(filenametoexecute,L" ");
							wcscat(filenametoexecute,HDserial);
							wcscat(filenametoexecute,L" ");
							wcscat(filenametoexecute,serialcode);
							DWORD salida;
							SetCursor(LoadCursor(NULL, IDC_WAIT));
							Sleep(100);
							salida = EjecutarProgram(filenametoexecute,true,SW_HIDE);	
							Sleep(100);
							SetCursor(LoadCursor(NULL, IDC_ARROW));

							//EjecutarProgram(filenametoexecute,true,SW_SHOW);	
							switch (salida)
							{
								case 0:
									MessageBox(	hDlg,L"File upload Successfully",MRecorder,64);
									break;
								case 1:
									MessageBox(	hDlg,L"You dont have a valid Sharing Plan serial number\nor you are using other computer\nPlease contact to our customer service \nError code =2",MRecorder,32);
									break;
								case 2:
									MessageBox(	hDlg,L"Invalida File\n Please contact to our customer service\nError code =2",MRecorder,32);

									break;
								case 3:
									MessageBox(	hDlg,L"The File already Exists",MRecorder,48);
									break;
								case 4:
									MessageBox(	hDlg,L"Server Error\nPlease contact to our customer service\nError code =4",MRecorder,64);
									break;
							}
						}
					}
					else
					{
						MessageBox(	hDlg,L"You are not subscribed to our sharing plan",MRecorder,64);
					}
				}
				else
				{
					MessageBox(	hDlg,L"Please Select a file to upload",MRecorder,48);
				};

				break;
			
			
			case ID_FILE_EXIT:
				//DialogBox(hInst, (LPCTSTR)IDD_VOIPRECORDER_DIALOG, hWnd, (DLGPROC)About);
				EndDialog(hDlg,0);			
				//MessageBox(	hDlg,L"ID_FILE_EXIT",L"ID_FILE_EXIT",0);
			break;

			case ID_RIGHTMENU_GOTOFOLDER:
				WCHAR afile[512];
				ZeroMemory(afile,sizeof(afile));
				ReadRegistryDestinationPath(afile);
				ShellExecute(0,L"open",afile,NULL,NULL,SW_SHOW);
				break;

			case ID_RIGHTMENU_GOTOEXFOLDER:
				ZeroMemory(afile,sizeof(afile));
				ReadRegistryExportPath(afile);
				ShellExecute(0,L"open",afile,NULL,NULL,SW_SHOW);
				break;

			case ID_EDIT_WITHAUDACITY:
				WCHAR filenameajecutar[512] ;
				ZeroMemory(filenameRecord,sizeof(filenameRecord));
				ZeroMemory(filenameajecutar,sizeof(filenameajecutar));
				bool salida;
				salida = GetFIlenameFromPlaylist(hDlg,filenameRecord);
				if (salida)
				{
					wsprintf(filenameajecutar,L"\\program files\\audacity\\audacity.exe \"%s\"",filenameRecord);
					EjecutarProgram(filenameajecutar,false,SW_SHOW);

				}
				else
				{
					MessageBox(hDlg,L"Please select a conversation to Edit ",MRecorder,48);
				
				}

				break;
			case IDC_SEARCH:
				Refresh_Listview(hDlg);
				break;

			case ID_EDIT_DELETERECOR:
				if (GetFIlenameFromPlaylist(hDlg,filenameRecord)==false)
				{
					MessageBox(hDlg,L"Please select a conversation to delete ",MRecorder,48);
					return 0;
				}
				RECORD_PLAYLIST arec;
				ZeroMemory(&arec,sizeof(arec));
				wcscat(arec.FileName,filenameRecord);
				Global_aplaylist->Search_RecordByFileName(&arec);
				WCHAR auxst[512];
				ZeroMemory(auxst,sizeof(auxst));
				wsprintf(auxst,L"Are you sure you want to delete:\n %s",filenameRecord);
				if (6 == MessageBox(hDlg,auxst,MRecorder,36))
				{
					DeleteFile(filenameRecord);
					Global_aplaylist->Delete_Record(&arec);
					Refresh_Listview(hDlg);
				}
			break;
			
			
			case ID_EDIT_EDITCONVERSATION:
				
				if (GetFIlenameFromPlaylist(hDlg,filenameRecord)==false)
				{
					MessageBox(hDlg,L"Please select a conversation to delete ",MRecorder,48);
					return 0;
				}
				DialogBox(hInst, (LPCTSTR)IDD_DIALOG_EDIT_CONV, hDlg, (DLGPROC)EditConv);
				Refresh_Listview(hDlg);
			break;
			
			
			case ID_VIEW_URLTOSHARE:
				
				//DialogBox(hInst, (LPCTSTR)IDD_VOIPRECORDER_DIALOG, hWnd, (DLGPROC)About);
				WCHAR Text[512];
				ZeroMemory(Text,sizeof(Text));
				if (GetFIlenameFromPlaylist(hDlg,Text)==false)
				{
					MessageBox(hDlg,L"Please select a conversation",MRecorder,48);
					return 0;
				}
				ZeroMemory(destinationshare,sizeof(destinationshare));
				if (GetUploadData(Text,destinationshare,0))
				{
					//MessageBox(	hDlg,dest,L"EasyVoipRecorder",64);
					DialogBox(hInst, (LPCTSTR)IDD_DIALOG_LINK, hDlg, (DLGPROC)About);
				}
				else
				{
					MessageBox(	hDlg,L"You are not subscribed to our sharing plan",MRecorder,64);
				}


			break;
			
			case ID_VIEW_OBJECTTAG:
				//DialogBox(hInst, (LPCTSTR)IDD_VOIPRECORDER_DIALOG, hWnd, (DLGPROC)About);
				ZeroMemory(Text,sizeof(Text));
				if (GetFIlenameFromPlaylist(hDlg,Text)==false)
				{
					MessageBox(hDlg,L"Please select a conversation",MRecorder,48);
					return 0;
				}
				ZeroMemory(destinationshare,sizeof(destinationshare));
				if (GetUploadData(Text,destinationshare,1))
				{
					//MessageBox(	hDlg,dest,L"EasyVoipRecorder",64);
					DialogBox(hInst, (LPCTSTR)IDD_DIALOG_LINK, hDlg, (DLGPROC)About);

				}
				else
				{
					MessageBox(	hDlg,L"You are not subscribed to our sharing plan",MRecorder,64);
				}

				break;
			
			case ID_PLAYBACK_PLAY:
				//MCIWndPlay(m_Video);
				ZeroMemory(Text,sizeof(Text));
				GetFIlenameFromPlaylist(hDlg,Text);

				ShellExecute(0, L"open", Text, NULL, NULL, SW_SHOWNORMAL);


				/*
				WCHAR aux[512];
				ZeroMemory(aux,sizeof(aux));
				wcscat(aux,L"\"\\Program Files\\Windows Media Player\\wmplayer.exe ");
				wcscat(aux, L"\"");
				wcscat(aux, L"\"");
				wcscat(aux, Text);
				wcscat(aux, L"\"");
				if (ISFileExists(L"\\Program Files\\Windows Media Player\\wmplayer.exe"))
				{
					EjecutarProgram(aux,false,SW_SHOW);
				}
				else
				{
					ZeroMemory(aux,sizeof(aux));
					wcscat(aux,L"\"\\Program Files\\Windows Media Player\\mplayer2.exe");
					wcscat(aux, L"\"");
					wcscat(aux, L" \"");
					wcscat(aux, Text);
					wcscat(aux, L"\"");
					//MessageBox(0,aux,aux,0);
					EjecutarProgram(aux,false,SW_SHOW);
				};
			*/
			break;
			
			

			case ID_HELP_ONLINEHELP:
				ShellExecute(hDlg,L"Open",L"http://www.easyvoiprecorder.com/index.php?option=com_content&task=view&id=17&Itemid=33",NULL,NULL,SW_SHOW);
				break;
			
			case ID_HELP_ABOUTEASYVOIPRECORDER:
				DialogBox(hInst, (LPCTSTR)IDD_VOIPRECORDER_DIALOG, hDlg, (DLGPROC)About);
			break;
		};
		break;
	case WM_SYSCOMMAND:
		{
			if ( (wParam ==SC_CLOSE  ) )
			{
				EndDialog(hDlg, LOWORD(wParam));
				DestroyWindow(hDlg);
				return FALSE;
			}
		}
	}
	return FALSE;
}



LRESULT CALLBACK Registration(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HFONT hOrigFont;
	LOGFONT lf;
	REGDATA Adata;
	switch (message)
	{
	case WM_INITDIALOG:
		ZeroMemory(&Adata,sizeof(Adata));
		ReadRegistration(&Adata);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_REG),Adata.CODE);
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_EMAIL),Adata.EMAIL);

		return TRUE;
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			
			//IDC_EDIT_EMAIL
			//IDC_EDIT_REG
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_REG),Adata.CODE,512);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_EMAIL),Adata.EMAIL,512);
			if (SendSerialTool(hDlg,Adata.CODE, Adata.EMAIL))
			{
				WriteRegistration(&Adata);
				EndDialog(hDlg, LOWORD(wParam));
			};

			return TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		
		break;
	}
	return FALSE;
}


bool search_criteria(PRECORD_PLAYLIST arecord,HWND hDlg)
{
	// aca buscar los datos del form
	// si es all ignorar
	// // tomar las fechas
	SYSTEMTIME timefrom;
	SYSTEMTIME timeto;

	FILETIME Filetimefrom;
	FILETIME Filetimeto;
	

	WCHAR wcharsource[128];
	WCHAR wchardest[128];
	WCHAR wcharcomments[512];

	ZeroMemory(&timefrom,sizeof(timefrom));
	ZeroMemory(&timeto,sizeof(timeto));

	ZeroMemory(wcharsource,sizeof(wcharsource));
	ZeroMemory(wchardest,sizeof(wchardest));
	ZeroMemory(wcharcomments,sizeof(wcharcomments));

	SendMessage(GetDlgItem(hDlg,IDC_DATETIMEPICKER_TO),DTM_GETSYSTEMTIME,0,(LPARAM)&timeto);
	SendMessage(GetDlgItem(hDlg,IDC_DATETIMEPICKER_FROM),DTM_GETSYSTEMTIME,0,(LPARAM)&timefrom);
	
	timeto.wHour=23;
	timeto.wMinute = 59;

	timefrom.wHour=0;
	timefrom.wMinute = 0;

	SystemTimeToFileTime(&timefrom,&Filetimefrom);
	SystemTimeToFileTime(&timeto,&Filetimeto);

	GetWindowText(GetDlgItem(hDlg,IDC_COMBO_DESTINATION),wchardest,sizeof(wchardest));
	GetWindowText(GetDlgItem(hDlg,IDC_COMBO_SOURCE),wcharsource,sizeof(wcharsource));
	GetWindowText(GetDlgItem(hDlg,IDC_EDIT_COMMENTS),wcharcomments,sizeof(wcharcomments));
	wcslwr(wcharcomments);
	 
	if  ((arecord->datetime.dwHighDateTime >= Filetimefrom.dwHighDateTime) && (arecord->datetime.dwHighDateTime <=Filetimeto.dwHighDateTime))
	{
		// ahora con el los demas
		if ((lstrcmpi(arecord->Source,wcharsource)==0) || (lstrcmpi(wcharsource,L" All")==0))
		{
			if ((lstrcmpi(arecord->Destination,wchardest)==0) || (lstrcmpi(wchardest,L" All")==0))
			{
				wcslwr(arecord->Comments);
				if ((NULL != wcsstr(arecord->Comments,wcharcomments))|| (lstrcmpi(wcharcomments,L"")==0))
				{
					return true;
				}
			}
		}
	}
	return false;

}



void Refresh_Listview(HWND hDlg)
{
	RECORD_PLAYLIST arecord;
	LVITEM  LvItem;
	WCHAR Temp[128];
	int fila=0;
	ListView_DeleteAllItems(GetDlgItem(hDlg,IDC_LIST));
	for(int x=0;x<Global_aplaylist->ItemCount();x++) // Add SubItems in a loop
	{
		arecord.Id = x;
		Global_aplaylist->Search_RecordById(&arecord);
		if (arecord.Status==0)
		{
			break;
		}
		if (arecord.Status==1)
		{
			if (search_criteria(&arecord,hDlg))
			{
				memset(&LvItem,0,sizeof(LvItem)); // Zero struct's Members
				LvItem.mask=LVIF_TEXT;   // Text Style8
				LvItem.cchTextMax = 256; // Max size of test
				LvItem.iItem=fila;          // choose item  
				SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_INSERTITEM,0,(LPARAM)&LvItem);

				LvItem.iItem=fila;          // Source
				LvItem.iSubItem=1;
				wsprintf(Temp,L"%s",arecord.Source);
				LvItem.pszText=Temp;
				SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems

				LvItem.iItem=fila;          // DateTime 
				LvItem.iSubItem=0;
				SYSTEMTIME systemtime;
				FileTimeToSystemTime(&arecord.datetime,&systemtime);
				wsprintf(Temp,L"%02d/%02d/%02d %02d:%02d:%02d",systemtime.wYear,systemtime.wMonth,systemtime.wDay,systemtime.wHour,systemtime.wMinute,systemtime.wSecond);
				//wsprintf(Temp,L"%u / %u  ",arecord.datetime.dwHighDateTime,arecord.datetime.dwLowDateTime);
				LvItem.pszText=Temp;
				SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems


				LvItem.iItem=fila;          // choose item  
				LvItem.iSubItem=3;
				wsprintf(Temp,L"%s",arecord.Comments);	
				LvItem.pszText=Temp;
				SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems

				LvItem.iItem=fila;          // choose item  
				LvItem.iSubItem=4;
				wsprintf(Temp,L"%s",arecord.FileName);
				LvItem.pszText=Temp;
				SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems


				LvItem.iItem=fila;          // choose item  
				LvItem.iSubItem=2;
				wsprintf(Temp,L"%s",arecord.Destination);
				LvItem.pszText=Temp;
				SendMessage(GetDlgItem(hDlg,IDC_LIST),LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems

				fila++;
			};
		};
	};

}




RECORD_WASAPI_NAMES ComboBoxItems_Speaker[24];
RECORD_WASAPI_NAMES ComboBoxItems_Mic[24];

void ShowDataInDialogMic(HWND hDlg)
{
	ZeroMemory(ComboBoxItems_Mic,sizeof(ComboBoxItems_Mic));
	SendMessage(GetDlgItem(hDlg,IDC_COMBO_MIC),
                        CB_RESETCONTENT,0,0);
	int i=0;
	getNames(ComboBoxItems_Mic,true);
	while (ComboBoxItems_Mic[i].hz != 0)
	{
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_MIC),
							CB_ADDSTRING,
							0,
							reinterpret_cast<LPARAM>((WCHAR*)ComboBoxItems_Mic[i].devicename));
		i++;
	}
    
	int in_HZ;
	int in_channel;
	int in_position;

	int out_HZ;
	int out_channel;
	int out_position;

	int bus_hz;

	WCHAR hz[512];
	ZeroMemory(hz,sizeof(hz));

	ReadWASAPIValues(&in_HZ , &in_channel ,  &in_position , &out_HZ , &out_channel , &out_position,&bus_hz);
	wsprintf(hz,L"%d",bus_hz);

	SetWindowText(GetDlgItem(hDlg,IDC_HZ), hz);
	SendMessage ( GetDlgItem(hDlg,IDC_COMBO_MIC), CB_SETCURSEL, (WPARAM)in_position,(LPARAM) 0) ;
};

void ShowDataInDialogSpeaker(HWND hDlg)
{
	ZeroMemory(ComboBoxItems_Speaker,sizeof(ComboBoxItems_Speaker));
	getNames(ComboBoxItems_Speaker,false);

	SendMessage(GetDlgItem(hDlg,IDC_COMBO_SPEAKER),
                        CB_RESETCONTENT,0,0);
	int i=0;
	while (ComboBoxItems_Speaker[i].hz != 0)
	{
		SendMessage(GetDlgItem(hDlg,IDC_COMBO_SPEAKER),
							CB_ADDSTRING,
							0,
							reinterpret_cast<LPARAM>((WCHAR*)ComboBoxItems_Speaker[i].devicename));
		i++;
	}

	int in_HZ;
	int in_channel;
	int in_position;

	int out_HZ;
	int out_channel;
	int out_position;

	int bus_hz;

	ReadWASAPIValues(&in_HZ , &in_channel ,  &in_position , &out_HZ , &out_channel , &out_position, &bus_hz);

	SendMessage ( GetDlgItem(hDlg,IDC_COMBO_SPEAKER), CB_SETCURSEL, (WPARAM)out_position,(LPARAM) 0) ;
};



LRESULT CALLBACK	USBOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hListBox;
	HFONT hOrigFont;
	LOGFONT lf;
	switch (message)
	{	
		case WM_INITDIALOG:
			ShowDataInDialogMic(hDlg);
			ShowDataInDialogSpeaker(hDlg);
			return TRUE;
		break;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL )
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			};
			if (LOWORD(wParam) == IDOK )
			{
				// 
				// 
				// IDC_COMBO_MIC
				// IDC_COMBO_SPEAKER
				int posicion_in  = SendMessage(GetDlgItem(hDlg,IDC_COMBO_MIC), CB_GETCURSEL,0,0);
				int posicion_out = SendMessage(GetDlgItem(hDlg,IDC_COMBO_SPEAKER), CB_GETCURSEL,0,0);
				int bus_hz;


				WCHAR hz[512];
				ZeroMemory(hz,sizeof(hz));



				GetWindowTextW(GetDlgItem(hDlg,IDC_HZ),hz,512);
				bus_hz = _wtoi(hz);
				WriteWASAPIValues(ComboBoxItems_Mic[posicion_in].hz, ComboBoxItems_Mic[posicion_in].channels , ComboBoxItems_Mic[posicion_in].pos , ComboBoxItems_Speaker[posicion_out].hz , ComboBoxItems_Speaker[posicion_out].channels , ComboBoxItems_Speaker[posicion_out].pos,bus_hz);
				EndDialog(hDlg, LOWORD(wParam));

				return TRUE;
			};
		break;
	}
	return FALSE;
}


