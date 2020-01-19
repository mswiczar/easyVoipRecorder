#pragma once
#define _WIN32_WINNT 0x500
#define _WIN32_IE 0x500

#include "stdafx.h"	


#define MAX_SPRITES 512
#define MAX_FONDO 1
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
typedef void(__cdecl *EventClick)(LPVOID);


class CSprite
{
	public:
		bool isvisibleFavo;
		CSprite(HDC myCompatibleHdc);
		virtual ~CSprite(void);
		HDC _myCompatibleHdc;
		HINSTANCE _hInst;
		virtual void pintar(HDC hdc_BackBuffer);
		int resOn;
		int resOver;
		int resClick;
		void Create(void);
		void SetVisible(bool visible);
		bool GetVisible(void);
		void SetPosition(LPPOINT apoint);
		void GetPosition(LPPOINT apoint);
		void Resize(int cx);
		void SetTransparent(bool aval);
		COLORREF transparentColor;
		void SetResizeable(bool asize);
		bool ProcessMouseMove(LPPOINT apoint); 
		bool ProcessMouseClick(LPPOINT apoint); 
		bool ProcessMouseClickUp(LPPOINT apoint); 
		int resize_margin;
		int min_resize;
		int max_resize;
		//IWebBrowser2 *m_spWebBrowser2;
		LPVOID m_spWebBrowser2;

		bool resize_left;
		EventClick ClickEventUp;
		EventClick MouseMoveEvent;
		bool changeiconOver;
		LPVOID anchor;
		LPVOID SpriteText;
		LPVOID _aSpriteManager;

		bool captioned;
		WCHAR CurrentCaption[512];
		HDC hdcOn;
		RECT rectangulo;
		RECT rectangulo_over;

		bool onover;
		DWORD idCommand;
		DWORD menuid;
		WCHAR url2Go[512];
		LPVOID SpriteHint;
		WCHAR HintText[512];

protected:
		bool visible;
		bool resizeable;		
		bool onclick;
		bool transparent;
		BITMAP bm;
		HDC hdcpintar;
		HDC hdcpintarMask;
		HDC hdcOnMask;
		HDC hdcOver;
		HDC hdcOverMask;
		HDC hdcClick;
		HDC hdcClickMask;
		HFONT hFont;
};

class CTextSprite:public CSprite
{
public:
	CTextSprite(HDC myCompatibleHdc):  CSprite(myCompatibleHdc) {};
	virtual void pintar(HDC hdc_BackBuffer);
	
};

class CGaugeSprite:public CSprite
{
public:
	CGaugeSprite(HDC myCompatibleHdc):  CSprite(myCompatibleHdc) {};
	int porcentaje;
	virtual void pintar(HDC hdc_BackBuffer);

	
};



class CAnimatedSprite:public CSprite
{
public:
	CAnimatedSprite(HDC myCompatibleHdc);
	virtual void pintar(HDC hdc_BackBuffer);
	int cant_images;
	int Sprite_frame_secs;
	int Movie_frame_secs;
private:
	int _frame_actual;
	int _pintar_number;
};



class CSpriteManager
{
public:
	CSpriteManager(void);
		void AddSprite(CSprite* a);
		void RemoveSprite(CSprite* a);
		void RemoveMenuItem(int buttonid);
		virtual ~CSpriteManager(void);
		void DeleteAll(void);

	CRITICAL_SECTION CriticalSection; 
	void CreateSpriteFondo (HDC acompatible);
	CSprite * SpriteFondo[MAX_FONDO];
	CSprite * captionSPrite;
	CSprite * SpriteHint;

	void pintarFondo(HDC ahdc);
	void ChangeFondo(long posicion);
	void pintar(HDC ahdc);
	void ProcessMouseMove(LPPOINT apoint); 
	void ProcessMouseClick(LPPOINT apoint); 
	void ProcessMouseClickUp(LPPOINT apoint); 
	void Resize(int cx);
	HCURSOR Hand;
	HCURSOR Arrow;
	HINSTANCE mylibrary;
	HWND toolband32;
	HWND m_hTextSearch;
	HWND m_hCombo;
	void LostFocusIe(void);
	LPVOID atoolband;
	CSprite* Menu_Anchor;


private:
	long volatile count_sprites;
	long volatile actual_fondo;
	RECT rectContenedor;
	CSprite *arraySprite[MAX_SPRITES];
};