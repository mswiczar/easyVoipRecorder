#include "stdafx.h"
#include "windows.h"
#include "Winreg.h"
#include "config.h"
#include "time.h"
#include "stdlib.h"
#include "malloc.h"






void  GetFilename( char * filename)
{
	tm *today;
	
	today = (tm *)malloc (sizeof(tm));

	filename[8] ='0';
	filename[9] ='0';
    __time64_t ltime;
	_time64( &ltime );
  	today = _localtime64( &ltime );	
	itoa(today->tm_year + 1900,&filename[0] ,10);
	filename[4] ='-';
	itoa(today->tm_mon +1 ,&filename[5] ,10);
	if (filename[6] == 0 ) 
	{
			filename[6]=filename[5];
			filename[5]='0';
	}
	filename[7] ='-';
	itoa(today->tm_mday ,&filename[8] ,10);
	if (filename[9] == 0 ) 
	{
			filename[9]=filename[8];
			filename[8]='0';
	};
	filename[10] ='_';
	itoa(today->tm_hour,&filename[11] ,10);
	if (filename[12] == 0 ) 
	{
			filename[12]=filename[11];
			filename[11]='0';
	};
	itoa(today->tm_min,&filename[13] ,10);
	if (filename[14] == 0 ) 
	{
			filename[14]=filename[13];
			filename[13]='0';
	};
	itoa(today->tm_sec,&filename[15] ,10);
}
		

