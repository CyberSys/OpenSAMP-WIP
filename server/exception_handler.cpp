
#ifdef WIN32

#include "main.h"
#include <Tlhelp32.h>

PCONTEXT pContextRecord;
HANDLE	 hInstance;
CHAR	 szErrorString[16384];

//----------------------------------------------------

void DumpMemory(BYTE *pData, DWORD dwCount, PCHAR sz, BOOL bAsDWords = FALSE)
{
	char s[16384];

	if(bAsDWords)
	{
		for(int i=0; i<(int)dwCount; i += 16)
		{
			sprintf(s, "+%04X: 0x%08X   0x%08X   0x%08X   0x%08X\r\n", i,
					*(DWORD*)(pData+i+0), *(DWORD*)(pData+i+4),
					*(DWORD*)(pData+i+8), *(DWORD*)(pData+i+12)
				);
			strcat(sz,s);
		}
	}
	else
	{
		for(int i=0; i<(int)dwCount; i += 16)
		{
			sprintf(s, "+%04X: %02X %02X %02X %02X   %02X %02X %02X %02X   "
					"%02X %02X %02X %02X   %02X %02X %02X %02X\r\n", i,
					pData[i+0], pData[i+1], pData[i+2], pData[i+3],
					pData[i+4], pData[i+5], pData[i+6], pData[i+7],
					pData[i+8], pData[i+9], pData[i+10], pData[i+11],
					pData[i+12], pData[i+13], pData[i+14], pData[i+15]
				);
			strcat(sz,s);
		}
	}
}

void DumpCrashInfo(char * szFileName)
{
	int x=0;
	DWORD *pdwStack;

	FILE *f = fopen(szFileName,"a");
	if(!f) return;

	fputs("\r\n--------------------------\r\n",f);

	sprintf(szErrorString,
		"SA-MP Server: 0.3z\r\n\r\n"
		"Exception At Address: 0x%08X\r\n\r\n"
		"Registers:\r\n"
		"EAX: 0x%08X\tEBX: 0x%08X\tECX: 0x%08X\tEDX: 0x%08X\r\n"
		"ESI: 0x%08X\tEDI: 0x%08X\tEBP: 0x%08X\tESP: 0x%08X\r\n"
		"EFLAGS: 0x%08X\r\n\r\nStack:\r\n",
		pContextRecord->Eip,
		pContextRecord->Eax,
		pContextRecord->Ebx,
		pContextRecord->Ecx,
		pContextRecord->Edx,
		pContextRecord->Esi,
		pContextRecord->Edi,
		pContextRecord->Ebp,
		pContextRecord->Esp,
		pContextRecord->EFlags);

	pdwStack = (DWORD *)pContextRecord->Esp;
	DumpMemory((BYTE *)pdwStack, 320, szErrorString, TRUE);

	fputs(szErrorString,f);
	fclose(f);
}

LONG WINAPI exc_handler(_EXCEPTION_POINTERS* exc_inf)
{
	pContextRecord = exc_inf->ContextRecord;
	
	DumpCrashInfo("crashinfo.txt");

#ifdef _DEBUG
	return EXCEPTION_CONTINUE_SEARCH;
#else
	return EXCEPTION_EXECUTE_HANDLER;
#endif
}

#endif