
#include "stdafx.h"
#include "sprite.h"
#include "winbase.h"




HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;
	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);
	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);
	SetBkColor(hdcMem, crTransparent);
	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);
	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);
	return hbmMask;
}

void CSprite::SetResizeable(bool asize)
{
	resizeable = asize;
}

void CSprite::GetPosition(LPPOINT apoint)
{
	apoint->x = this->rectangulo.left;
	apoint->y = this->rectangulo.top;
}

void CSprite::Resize(int cx)
{
	int newpos;
	CSprite * anchor_sprite = (CSprite *) anchor;
	POINT mipointanchor;
	if (this->resizeable)
	{
		if (anchor_sprite != NULL)
		{
			anchor_sprite->GetPosition(&mipointanchor);
			this->rectangulo.left = mipointanchor.x - resize_margin - this->rectangulo.right ;
		}
		else
		{
			newpos = cx- (this->rectangulo.right) - resize_margin;
			if  ((newpos >= this->max_resize) &&  (max_resize!=0))
			{
				newpos = max_resize;
			}

			if (newpos >=  this->min_resize)   		  
			{
				this->rectangulo.left = newpos ;
			}
			else
			{
				this->rectangulo.left = this->min_resize;
			}
		}
	}
}

void CSprite::Create(void)
{
	HGDIOBJ hdiObjGen;
	HBITMAP HB_Bitmap;
	HBITMAP HB_BitmapT;

	if (this->resOn !=0)
	{
		this->hdcOn		= CreateCompatibleDC(_myCompatibleHdc);
		HB_Bitmap = LoadBitmap(_hInst, MAKEINTRESOURCE(this->resOn));
		GetObject(HB_Bitmap, sizeof(this->bm), &this->bm);
		this->rectangulo.right =   bm.bmWidth;
		this->rectangulo.bottom =  bm.bmHeight;
		if (this->transparent==true)
		{
			this->hdcOnMask      = CreateCompatibleDC(_myCompatibleHdc);
			HB_BitmapT =CreateBitmapMask(HB_Bitmap,this->transparentColor);
			hdiObjGen = SelectObject(this->hdcOnMask,  HB_BitmapT);
			DeleteObject(hdiObjGen);
			DeleteObject(HB_BitmapT);
		}
	};

	hdiObjGen = SelectObject(this->hdcOn,HB_Bitmap);
	DeleteObject(HB_Bitmap);
	DeleteObject(hdiObjGen);
	

	SetBkMode(this->hdcOn,TRANSPARENT);

	if (this->resOver!= 0)
	{
		this->hdcOver        = CreateCompatibleDC(_myCompatibleHdc);
		if (this->transparent==true)
		{
			this->hdcOverMask    = CreateCompatibleDC(_myCompatibleHdc);
		};

		HB_Bitmap = LoadBitmap(_hInst, MAKEINTRESOURCE(this->resOver));
		GetObject(HB_Bitmap, sizeof(this->bm), &this->bm);
		this->rectangulo_over.right =   bm.bmWidth;
		this->rectangulo_over.bottom =  bm.bmHeight;

		HB_BitmapT =CreateBitmapMask(HB_Bitmap,this->transparentColor);
		hdiObjGen = SelectObject(this->hdcOverMask,  HB_BitmapT);
		DeleteObject(hdiObjGen);
		hdiObjGen = SelectObject(this->hdcOver,HB_Bitmap);
		GetObject(hdiObjGen, sizeof(this->bm), &this->bm);
		DeleteObject(hdiObjGen);
		DeleteObject(HB_Bitmap);
		DeleteObject(HB_BitmapT);
		SetBkMode(this->hdcOver,TRANSPARENT);
	}

	if (this->resClick != 0)
	{
		this->hdcClick       = CreateCompatibleDC(_myCompatibleHdc);
		if (this->transparent==true)
		{
			this->hdcClickMask   = CreateCompatibleDC(_myCompatibleHdc);
		};

		HB_Bitmap = LoadBitmap(_hInst, MAKEINTRESOURCE(this->resClick));
		HB_BitmapT =CreateBitmapMask(HB_Bitmap,this->transparentColor);
		hdiObjGen = SelectObject(this->hdcClickMask,  HB_BitmapT);
		DeleteObject(hdiObjGen);
		hdiObjGen = SelectObject(this->hdcClick,HB_Bitmap);
		GetObject(hdiObjGen, sizeof(this->bm), &this->bm);
		DeleteObject(hdiObjGen);
		DeleteObject(HB_Bitmap);
		DeleteObject(HB_BitmapT);
		SetBkMode(this->hdcClick,TRANSPARENT);
	};


}

void CSprite::SetTransparent(bool aval)
{
	this->transparent = aval;
}

CSprite::CSprite(HDC myCompatibleHdc)
{
	this->_myCompatibleHdc = myCompatibleHdc;
	this->resize_left    = false;
	this->onover         = false;
	this->onclick        = false;
	this->SpriteHint	 = NULL;

	this->resizeable     = false;
	this->visible        = false;
	this->ClickEventUp   = NULL;
	this->MouseMoveEvent = NULL;
	this->changeiconOver = false;
	this->anchor         = NULL;
	this->max_resize     = 0;
	this->SpriteText     = NULL;
	this->captioned = false;
	this->menuid = -1;
	ZeroMemory(this->CurrentCaption,sizeof(this->CurrentCaption));
}

CSprite::~CSprite(void)
{
	DeleteDC(this->hdcOn);		   
	
	
	if (this->resOver!= 0)
	{
		DeleteDC(this->hdcOver);      
	}

	if (this->resClick!= 0)
	{
		DeleteDC(this->hdcClick);     
	};

	if (this->transparent)
	{
		DeleteDC(this->hdcOnMask);    
		DeleteDC(this->hdcOverMask);
		DeleteDC(this->hdcClickMask); 
	};


}

void  CSprite::SetVisible(bool a)
{
	this->visible = a;
}

bool CSprite::GetVisible(void)
{
	return visible;
}


void CSprite::pintar(HDC hdc_BackBuffer)
{

	if (this->visible)
	{
		CSprite * anchor_sprite = (CSprite *)this->anchor;
		if (anchor_sprite != NULL)
		{
			POINT mipointanchor;
			anchor_sprite->GetPosition(&mipointanchor);
			this->rectangulo.left = mipointanchor.x - resize_margin - this->rectangulo.right ;
		}
		if (this->onclick)
		{
				if (this->resClick != 0)
				{
					this->hdcpintar = this->hdcClick;
					this->hdcpintarMask = this->hdcClickMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}
		}
		else
		{
			if (this->onover)
			{
				if (this->resOver!= 0)
				{
					this->hdcpintar = this->hdcOver;
					this->hdcpintarMask = this->hdcOverMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}
			}
			else
			{
				this->hdcpintar = this->hdcOn;
				this->hdcpintarMask = this->hdcOnMask;
			}
		};

		if (this->captioned)
		{
			hFont = CreateFont(18, 0 , 0 , 0 , FW_NORMAL , FALSE , FALSE , FALSE ,
			ANSI_CHARSET , OUT_DEFAULT_PRECIS ,
			CLIP_DEFAULT_PRECIS , DEFAULT_QUALITY , 
			VARIABLE_PITCH | FF_DONTCARE , NULL);
			RECT a;
			a.left = 0;
			a.right = this->rectangulo.right;
			a.top = 0;
			a.bottom = this->rectangulo.bottom;
			SetTextColor(this->hdcOn,RGB(255,255,255));
			BitBlt(this->hdcOn,0,0,this->rectangulo.right, this->rectangulo.top,this->hdcOver,0,0,BLACKNESS);
			SelectObject(this->hdcOn, hFont);
			EnterCriticalSection(&((CSpriteManager*)this->_aSpriteManager)->CriticalSection);
				DrawTextW(this->hdcOn,this->CurrentCaption,wcslen(this->CurrentCaption),&a,DT_CENTER|DT_NOCLIP|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);
			LeaveCriticalSection (&((CSpriteManager*)this->_aSpriteManager)->CriticalSection);
	//		SelectObject(this->hdcOn, GetStockObject(SYSTEM_FONT));
			DeleteObject(this->hFont);


		}

		if (this->transparent)
		{
			BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom, this->hdcpintarMask, 0, 0, SRCAND);
			BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom, this->hdcpintar, 0, 0, SRCPAINT);
		}
		else
		{
			// todo hacer esto para over bien
			if ((this->onover) && (this->resOver!= 0))
			{
				BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo_over.right, this->rectangulo_over.bottom, this->hdcpintar, 0, 0, SRCCOPY);
			}
			else
			{
				BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom, this->hdcpintar, 0, 0, SRCCOPY);
			}
		};
	};
}







void CSprite::SetPosition(LPPOINT apoint)
{
	this->rectangulo.left =  apoint->x;
	this->rectangulo.top  =  apoint->y;
};


bool CSprite::ProcessMouseMove(LPPOINT apoint)
{
	if ((apoint->x>= this->rectangulo.left) && (apoint->x<= this->rectangulo.left +  this->rectangulo.right))
		{
			if ((apoint->y>= this->rectangulo.top) && (apoint->y<= this->rectangulo.top + this->rectangulo.bottom))
				{
						this->onover = true;
						if (this->MouseMoveEvent != NULL)
						{
							this->MouseMoveEvent(this);
						};
				}
				else
				{
						this->onover = false;
				}
		}
	else
			{
				this->onover = false;
			}

	if ((this->onover) && (this->changeiconOver) )
	{
		return true;

	}
	else
	{
		return false;
	}
}

bool CSprite::ProcessMouseClick(LPPOINT apoint)
{
	if ((apoint->x>= this->rectangulo.left) && (apoint->x<= this->rectangulo.left +  this->rectangulo.right))
		{
			if ((apoint->y>= this->rectangulo.top) && (apoint->y<= this->rectangulo.top + this->rectangulo.bottom))
				{
						this->onclick = true;

				}
				else
				{
						this->onclick = false;

				}
		}
	else
			{
					this->onclick = false;

			}
	return this->onclick;
}

bool CSprite::ProcessMouseClickUp(LPPOINT apoint)
{
	if ((apoint->x>= this->rectangulo.left) && (apoint->x<= this->rectangulo.left +  this->rectangulo.right))
		{
			if ((apoint->y>= this->rectangulo.top) && (apoint->y<= this->rectangulo.top + this->rectangulo.bottom))
				{
						if (this->ClickEventUp != NULL)
						{
							this->ClickEventUp(this);
						};
						this->onclick = false;

				}
				else
				{
						this->onclick = false;

				}
		}
	else
			{
					this->onclick = false;

			}
	return this->onclick;
}





////////////////////////////////
// CSpriteManager
////////////////////////////////



void CSpriteManager::CreateSpriteFondo (HDC acompatible)
{
/*
	
	
	POINT apoint;
	SpriteFondo[0] = new CSprite(acompatible);
		SpriteFondo[0]->resOn = 2003;
		SpriteFondo[0]->resOver = 0;
		SpriteFondo[0]->resClick = 0;
		SpriteFondo[0]->SetVisible(true);
		SpriteFondo[0]->SetResizeable(false);
			SpriteFondo[0]->resize_margin = 0;
			SpriteFondo[0]->min_resize = 0;
		SpriteFondo[0]->SetTransparent(false);
		SpriteFondo[0]->transparentColor = RGB(255,0,0);
		SpriteFondo[0]->_hInst = this->mylibrary;
		apoint.x = 0;
		apoint.y = 0;
		SpriteFondo[0]->SetPosition(&apoint);
		SpriteFondo[0]->_aSpriteManager = this;
		SpriteFondo[0]->Create();
		::InterlockedExchange(&this->actual_fondo,0);


	SpriteFondo[1] = new CSprite(acompatible);
		SpriteFondo[1]->resOn = 2002;
		SpriteFondo[1]->resOver = 0;
		SpriteFondo[1]->resClick = 0;
		SpriteFondo[1]->SetVisible(true);
		SpriteFondo[1]->SetResizeable(false);
			SpriteFondo[1]->resize_margin = 0;
			SpriteFondo[1]->min_resize = 0;
		SpriteFondo[1]->SetTransparent(false);
		SpriteFondo[1]->transparentColor = RGB(255,0,0);
		SpriteFondo[1]->_hInst = this->mylibrary;
		apoint.x = 0;
		apoint.y = 0;
		SpriteFondo[1]->SetPosition(&apoint);
		SpriteFondo[1]->_aSpriteManager = this;

		SpriteFondo[1]->Create();



	SpriteFondo[2] = new CSprite(acompatible);
		SpriteFondo[2]->resOn = 2004;
		SpriteFondo[2]->resOver = 0;
		SpriteFondo[2]->resClick = 0;
		SpriteFondo[2]->SetVisible(true);
		SpriteFondo[2]->SetResizeable(false);
			SpriteFondo[2]->resize_margin = 0;
			SpriteFondo[2]->min_resize = 0;
		SpriteFondo[2]->SetTransparent(false);
		SpriteFondo[2]->transparentColor = RGB(255,0,0);
		SpriteFondo[2]->_hInst = this->mylibrary;
		apoint.x = 0;
		apoint.y = 0;
		SpriteFondo[2]->SetPosition(&apoint);
		SpriteFondo[2]->_aSpriteManager = this;
		SpriteFondo[2]->Create();
		DWORD a;
		GetFondoId(&a);
		::InterlockedExchange(&this->actual_fondo,a);


	SpriteFondo[3] = new CSprite(acompatible);
		SpriteFondo[3]->resOn = 2003;
		SpriteFondo[3]->resOver = 0;
		SpriteFondo[3]->resClick = 0;
		SpriteFondo[3]->SetVisible(true);
		SpriteFondo[3]->SetResizeable(false);
			SpriteFondo[3]->resize_margin = 0;
			SpriteFondo[3]->min_resize = 0;
		SpriteFondo[3]->SetTransparent(false);
		SpriteFondo[3]->transparentColor = RGB(255,0,0);
		SpriteFondo[3]->_hInst = this->mylibrary;
		apoint.x = 0;
		apoint.y = 0;
		SpriteFondo[3]->SetPosition(&apoint);
		SpriteFondo[3]->aSpriteManager = this;
		SpriteFondo[3]->Create();
		*/
}



CSpriteManager::CSpriteManager(void)
{
	this->SpriteHint =NULL;
	this->Hand= LoadCursor(NULL, IDC_HAND);
	this->Arrow = 	LoadCursor(NULL, IDC_ARROW);

	InitializeCriticalSectionAndSpinCount(&this->CriticalSection,0x80000400);
	this->captionSPrite=NULL;
	::InterlockedExchange(&this->count_sprites,0);
	ZeroMemory(&this->rectContenedor,sizeof(this->rectContenedor));
	ZeroMemory(&this->arraySprite[0],sizeof(this->arraySprite[0])*  MAX_SPRITES);


}


void CSpriteManager::Resize(int cx)
{
	this->rectContenedor.left = cx;

	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])!=0 )
		{
			this->arraySprite[i]->Resize(cx);
		}
	}


}

void CSpriteManager::DeleteAll(void)
{
	this->captionSPrite=NULL;
	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])!=0 )
		{
			delete (this->arraySprite[i]);
		}
	}
	//delete SpriteFondo[0];
	//delete SpriteFondo[1];
	//delete SpriteFondo[2];

	::InterlockedExchange(&this->count_sprites,0);
	ZeroMemory(&this->arraySprite[0],sizeof(this->arraySprite[0])*  MAX_SPRITES);


}


CSpriteManager::~CSpriteManager(void)
{
	this->DeleteAll();
	DeleteCriticalSection(&this->CriticalSection);

}


void CSpriteManager::AddSprite(CSprite* a)
{
	a->_aSpriteManager = this;
	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])==0 )
		{
			this->arraySprite[i] = a;
			::InterlockedIncrement(&this->count_sprites);
			break;
		}
	}
}


void CSpriteManager::RemoveSprite(CSprite* a)
{
	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])==a )
		{
			delete a;
			this->arraySprite[i] =0 ;
			::InterlockedDecrement(&this->count_sprites);
			break;
		}
	}
}

void CSpriteManager::RemoveMenuItem(int buttonid)
{
	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if (this->arraySprite[i]!=NULL )
		{
			if ((this->arraySprite[i]->menuid)==buttonid )
			{
				delete this->arraySprite[i];
				this->arraySprite[i] =0 ;
				::InterlockedDecrement(&this->count_sprites);
				break;
			}
		};
	}
}




void CSpriteManager::ChangeFondo(long posicion)
{
/*
	if ((posicion >= 0 )&& (posicion <=  MAX_FONDO-1))
	{
			::InterlockedExchange(&this->actual_fondo,posicion);
	}
	else
	{
		if (this->actual_fondo == MAX_FONDO-1)
		{
			::InterlockedExchange(&this->actual_fondo,0);
		}
		else
		{
			::InterlockedIncrement(&this->actual_fondo);
		}

	}
	SetFondoId(this->actual_fondo);
*/

}



void CSpriteManager::pintarFondo(HDC hdc_BackBuffer)
{
	//this->SpriteFondo[actual_fondo]->pintar(hdc_BackBuffer);
	//BitBlt(hdc_BackBuffer, 0, 20, 1280, 118, this->SpriteFondo[actual_fondo]->hdcOn, 0, 0, SRCCOPY);
	return;
}


void CSpriteManager::pintar(HDC hdc_BackBuffer)
{
	DWORD	count = 0;

	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])!=0 )
		{
			count++;
			if (this->arraySprite[i]->GetVisible())
			{
				this->arraySprite[i]->pintar (hdc_BackBuffer);
			};
		};
		if (this->count_sprites == count)
		{
			break;
		}

	};
}




void CSpriteManager::ProcessMouseMove(LPPOINT apoint)
{
	bool acursor = false;
	DWORD	count = 0;
	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])!=0 )
		{
			count++;
			if (this->arraySprite[i]->GetVisible())
			{
				if (this->arraySprite[i]->ProcessMouseMove(apoint))
				{
					acursor = true;
				}
			};
		}
		if (this->count_sprites == count)
		{
			break;
		}

	}
	
	if (acursor)
	{
		SetCursor(this->Hand);
	}
	else
	{
		SetCursor(this->Arrow);
		// aca tengo que poner en blanco el caption
		EnterCriticalSection(&this->CriticalSection);
		if (this->captionSPrite != NULL) 
		{
			ZeroMemory(this->captionSPrite->CurrentCaption,sizeof(this->captionSPrite->CurrentCaption));		
		};
		LeaveCriticalSection (&this->CriticalSection);
		//this->SpriteHint->SetVisible(false);
		
		
		
	}


}

void CSpriteManager::ProcessMouseClick(LPPOINT apoint) 
{
	DWORD	count = 0;
	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])!=0 )
		{
			count++;
			if (this->arraySprite[i]->GetVisible())
			{
				if (this->arraySprite[i]->ProcessMouseClick(apoint))
				{
					
				}
			}
			if (this->count_sprites == count)
			{
				break;
			}

		}
	}

}
void CSpriteManager::ProcessMouseClickUp(LPPOINT apoint)
{
	DWORD	count = 0;
	for (int i = 0 ; i< MAX_SPRITES; i++)
	{
		if ((this->arraySprite[i])!=0 )
		{
			count++;
			if (this->arraySprite[i]->GetVisible())
			{
				if (this->arraySprite[i]->ProcessMouseClickUp(apoint))
				{
					
				}
			}
			if (this->count_sprites == count)
			{
				break;
			}

		}
	}
}




void CTextSprite::pintar(HDC hdc_BackBuffer)
{
	if (this->visible)
	{
		if (this->onclick)
		{
				if (this->resClick != 0)
				{
					this->hdcpintar = this->hdcClick;
					this->hdcpintarMask = this->hdcClickMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}

				SetTextColor(this->hdcpintar,RGB(50,50,50));
		}
		else
		{
			if (this->onover)
			{
				if (this->resOver != 0)
				{
					this->hdcpintar = this->hdcOver;
					this->hdcpintarMask = this->hdcOverMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}
				SetTextColor(this->hdcpintar,RGB(255,255,255));
			}
			else
			{
				this->hdcpintar = this->hdcOn;
				this->hdcpintarMask = this->hdcOnMask;
				SetTextColor(this->hdcpintar,RGB(255,255,255));
			}
		}
		
		long lfHeight = -MulDiv(10, GetDeviceCaps(hdcOn, LOGPIXELSY), 96);


		hFont = CreateFont(
							13, 5	, 0, 0,
							200,
							FALSE,
							FALSE,
							FALSE,
							ANSI_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY,
							DEFAULT_PITCH ,
							L""
						  ); 

		RECT a;
		a.left = 3;
		a.right = this->rectangulo.right;
		a.top = 0;
		a.bottom = this->rectangulo.bottom;

		BitBlt(this->hdcpintar,0,0,this->rectangulo.right, this->rectangulo.top,this->hdcClick,0,0,SRCCOPY);

		SelectObject(this->hdcpintar, hFont);
			DrawTextW(this->hdcpintar,this->CurrentCaption,wcslen(this->CurrentCaption),&a,DT_CENTER|DT_NOCLIP|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);
			SelectObject(this->hdcpintar, GetStockObject(SYSTEM_FONT));
		DeleteObject(this->hFont);

		if (this->transparent)
		{
			BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom, this->hdcpintarMask, 0, 0, SRCAND);
			BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom, this->hdcpintar, 0, 0, SRCPAINT);
		}
		else
		{
			BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom, this->hdcpintar, 0, 0, SRCCOPY);
		};
	};
}


void CGaugeSprite::pintar(HDC hdc_BackBuffer)
{
	if (this->visible)
	{
		if (this->onclick)
		{
				if (this->resClick != 0)
				{
					this->hdcpintar = this->hdcClick;
					this->hdcpintarMask = this->hdcClickMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}
		}
		else
		{
			if (this->onover)
			{
				if (this->resOver != 0)
				{
					this->hdcpintar = this->hdcOver;
					this->hdcpintarMask = this->hdcOverMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}
			}
			else
			{
				this->hdcpintar = this->hdcOn;
				this->hdcpintarMask = this->hdcOnMask;
			}
		}
		BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right*this->porcentaje/100, this->rectangulo.bottom, this->hdcpintar, 0, 0, SRCCOPY);
	};
}



CAnimatedSprite::CAnimatedSprite(HDC myCompatibleHdc):CSprite(myCompatibleHdc)
{
	this->_frame_actual = 0;
	this->_pintar_number=0;
	
}


void CAnimatedSprite::pintar(HDC hdc_BackBuffer)
{
	_pintar_number++;
	if ((_pintar_number % ( Movie_frame_secs / Sprite_frame_secs )  )== 0)
	{
		this->_frame_actual++;
	}
	
	if  (this->_frame_actual  == this->cant_images)
	{
		this->_frame_actual = 0;
	};


	if (this->visible)
	{
		CSprite * anchor_sprite = (CSprite *)this->anchor;
		if (anchor_sprite != NULL)
		{
			POINT mipointanchor;
			anchor_sprite->GetPosition(&mipointanchor);
			this->rectangulo.left = mipointanchor.x - resize_margin - this->rectangulo.right ;
		}
		if (this->onclick)
		{
				if (this->resClick != 0)
				{
					this->hdcpintar = this->hdcClick;
					this->hdcpintarMask = this->hdcClickMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}
		}
		else
		{
			if (this->onover)
			{
				if (this->resOver!= 0)
				{
					this->hdcpintar = this->hdcOver;
					this->hdcpintarMask = this->hdcOverMask;
				}
				else
				{
					this->hdcpintar = this->hdcOn;
					this->hdcpintarMask = this->hdcOnMask;
				}
			}
			else
			{
				this->hdcpintar = this->hdcOn;
				this->hdcpintarMask = this->hdcOnMask;
			}
		};

		if (this->captioned)
		{
			hFont = CreateFont(18, 0 , 0 , 0 , FW_NORMAL , FALSE , FALSE , FALSE ,
			ANSI_CHARSET , OUT_DEFAULT_PRECIS ,
			CLIP_DEFAULT_PRECIS , DEFAULT_QUALITY , 
			VARIABLE_PITCH | FF_DONTCARE , NULL);
			RECT a;
			a.left = 0;
			a.right = this->rectangulo.right;
			a.top = 0;
			a.bottom = this->rectangulo.bottom;
			SetTextColor(this->hdcOn,RGB(255,255,255));
			BitBlt(this->hdcOn,0,0,this->rectangulo.right, this->rectangulo.top,this->hdcOver,0,0,BLACKNESS);
			SelectObject(this->hdcOn, hFont);
			EnterCriticalSection(&((CSpriteManager*)this->_aSpriteManager)->CriticalSection);
				DrawTextW(this->hdcOn,this->CurrentCaption,wcslen(this->CurrentCaption),&a,DT_CENTER|DT_NOCLIP|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);
			LeaveCriticalSection (&((CSpriteManager*)this->_aSpriteManager)->CriticalSection);
			DeleteObject(this->hFont);
		}


		if (this->transparent)
		{
			BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom /this->cant_images , this->hdcpintarMask, 0, (this->rectangulo.bottom / this->cant_images * this->_frame_actual), SRCAND);
			BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom /this->cant_images , this->hdcpintar, 0, (this->rectangulo.bottom / this->cant_images * this->_frame_actual), SRCPAINT);
		}
		else
		{
			if ((this->onover) && (this->resOver!= 0))
			{
				BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo_over.right, this->rectangulo_over.bottom/ this->cant_images, this->hdcpintar, 0, (this->rectangulo.bottom / this->cant_images * this->_frame_actual), SRCCOPY);
			}
			else
			{
				BitBlt(hdc_BackBuffer, this->rectangulo.left, this->rectangulo.top, this->rectangulo.right, this->rectangulo.bottom / this->cant_images , this->hdcpintar, 0, (this->rectangulo.bottom / this->cant_images * this->_frame_actual), SRCCOPY);
			}
		};
	};

	return; 
}
