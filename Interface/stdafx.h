// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef WINVER
#define WINVER 0x501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#define _WIN32_WINNT 0x500
//#define _WIN32_IE 0x500

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
/*
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
*/

#include <tchar.h>
#include <strsafe.h>

// TODO: reference additional headers your program requires here
