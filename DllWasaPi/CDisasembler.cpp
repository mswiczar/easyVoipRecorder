#include "stdafx.h"
#include "CDisasembler.h"

int GetPosition(LPVOID buffer, int elementos)
{
	int salida = 0;
	// buscar E8 y luego 4 bytes que es call  4 bytes
	// buscar 85C0 que es test eax,eax
	unsigned char * buffer_instructions = ( unsigned char *) buffer;
	for (int i=0;i<=elementos;i++)
	{
		if (buffer_instructions[i] == 0xE8) 
		{
			if (buffer_instructions[i+5]== 0x85)
			{
				if (buffer_instructions[i+6] == 0xC0)
				{
					salida = i;
					break;
				}
			}
		}
	}
	if (salida == 0)
	{
		//:6D6C7591 83EC2C                  sub esp, 0000002C
		//:6D6C7594 56                      push esi
		// parche windows vista
		for (int i=0;i<=elementos;i++)
		{
			if (buffer_instructions[i] == 0x83) 
			{
				if ((buffer_instructions[i+3]== 0x56)|| (buffer_instructions[i+3]== 0x57)) 
				{
						salida = i+3;
						break;
				}
			}
		}
	}
	return salida;
}


int GetPositionCOM2(LPVOID buffer, int elementos)
{
/*
73876880  mov         edi,edi 
73876882  push        ebp  
73876883  mov         ebp,esp 
73876885  push        ecx  
73876886  push        ebx  
73876887  push        esi  
73876888  mov         esi,dword ptr [ebp+8] 
7387688B  push        edi  
7387688C  xor         edi,edi 
7387688E  mov         ecx,esi 
73876890  mov         dword ptr [ebp-4],edi 
73876893  call        73876930 
*/

	//8b ff 55 8b ec 83 ec 20 8b 0d 04 60 f6 73 53 56 33
	//8b XX 55 8b XX 83 XX XX XX 

    //8b ff 55 8b ec 51 53 56 8b 75 08 57 33 ff 8b ce

	int salida = 0;
	unsigned char * buffer_instructions = ( unsigned char *) buffer;
	for (int i=0;i<=elementos;i++)
	{
		if (buffer_instructions[i] == 0x8B) 
		{
				if (buffer_instructions[i+2] == 0x55)
				{
					if (buffer_instructions[i+3] == 0x8B)
					{
						if (buffer_instructions[i+5] == 0x83)
						{
							salida = i + 8;
							break;
						}
					};
				}
		}
	}
	return salida;
}





int GetPositionCOM3(LPVOID buffer, int elementos)
{
	return 24;
}






int GetPositionCOM(LPVOID buffer, int elementos)
{
	// 8B FF                      		mov	edi,edi
	// 55                        		push	ebp

	// buscar esto PARECE QUE LAS 2 FUNCIONES LO IMPLEMENTAN
	// 8B EC                      		mov	ebp,esp
	// 8B 0D 18 70 0D 51              	mov	ecx,XX XX XX XX 
	//56                        		push	esi
//8b ff 55 8b ec 83 ec 20 8b 0d 04 60 f6 73 53 56 33
	int salida = 0;
	unsigned char * buffer_instructions = ( unsigned char *) buffer;
	for (int i=0;i<=elementos;i++)
	{
		if (buffer_instructions[i] == 0x8B) 
		{
			if (buffer_instructions[i+1]== 0xEC)
			{
				if (buffer_instructions[i+2] == 0x8B)
				{
					if (buffer_instructions[i+3] == 0x0d)
					{
						if (buffer_instructions[i+8] == 0x56)
						{
							salida = i + 9;
							break;
						}
					};
				}
			}
		}
	}
	return salida;
}


// otra funcion que necesito
// Como en windows 2000 no puedo saber la posicion del 

bool iswinXP (void)
{
	OSVERSIONINFO lpVersionInfo;
	lpVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&lpVersionInfo);
	return
   ( (lpVersionInfo.dwMajorVersion > 5) ||
   ( (lpVersionInfo.dwMajorVersion == 5) && (lpVersionInfo.dwMinorVersion >= 1) ));
	
	 	    
}


LPVOID GetVirtualTableFrom(BYTE * buffer) 
{
	DWORD *  salida=0;
	for (int i =0 ; i< 200 ; i++)
	{
		if  ((buffer[i] == 0x0) &&  (buffer[i+1] == 0x0)  &&  (buffer[i+2] == 0x0)  &&  (buffer[i+3] == 0x0))
		{
			salida =  (DWORD *) (buffer + i+4);
			break;
		}
	}

	return &salida;
}




