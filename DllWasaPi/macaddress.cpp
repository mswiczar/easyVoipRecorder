#include "macaddress.h"


/*
void GetMACaddress(LPVOID macaddress )
{
  IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
  // for up to 16 NICs
  DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer
  DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
    AdapterInfo,                 // [out] buffer to receive data
    &dwBufLen);                  // [in] size of receive data buffer
  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; 
  memcpy( macaddress , pAdapterInfo->Address,16);
	  
}
*/

void GetMACaddress(char * macaddress )
{

	char bytesSerial[6];
	char namebuffer[50];
	char filenamebuffer[50];
	DWORD serial_number;
	DWORD maximun;
	DWORD fileflags;
	GetVolumeInformation("C:\\",&namebuffer[0],50,&serial_number,&maximun,&fileflags,&filenamebuffer[0],50);
	memcpy(&macaddress[0], &serial_number,4);
	memcpy(&macaddress[4], &serial_number,2);
};
