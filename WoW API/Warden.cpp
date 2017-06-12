#include "Warden.h"

#include "Game\Game.h"
#include "Memory\Memory.h"
#include "Main\Constants.h"

DWORD g_wardenScanModule = NULL;

DWORD Warden::GetBase()
{
	DWORD moduleAddress = NULL;

	// Return true for fail-safe.
	if (!rpm(BasePtr, &moduleAddress, sizeof(DWORD)))
		return true;

	if (!rpm(moduleAddress, &moduleAddress, sizeof(DWORD)))
		return true;

	return moduleAddress;
}

DWORD Warden::GetClass()
{
	DWORD buffer = NULL;

	rpm(ClassPtr, &buffer, sizeof(buffer));
	return buffer;
}

DWORD Warden::IsLoaded()
{
	return Warden::GetBase() >= 0;
}

/*
extern "C" __declspec(naked) DWORD WardenCave(void *null)
{
	__asm
	{
		mov ScanAddress,esi		//Address

		pushfd;
		pushad;
	};

	if (0 == 0x005191C0)
	{
		__asm
		{
			popad;
			popfd;

			je Jes1					//Warden Code
			repe movsd

			Jes1:
			mov cl,03
			and ecx,edx
			je Jes2

			mov WardenScanResults, edi; // Save warden scan results ptr.
			repe movsb;

			pushad;
			pushfd;
		};

		//FixScanResults(LuaScanResults, 2);

		__asm
		{
			popfd;
			popad;

			Jes2:
			pop edi;
			pop esi;

			jmp dword ptr 0; //Jump back...
		};
	}

	__asm
	{
		popad;
		popfd;

		je Je1					//Warden Code
		repe movsd

		Je1:
		mov cl,03
		and ecx,edx
		je Je2
		repe movsb 

		Je2:
		pop edi
		pop esi

		mov edx, ScanLength;

		jmp dword ptr 0; //Jump back...
	};

	// End of function mark.
	__asm
	{
		jump dword ptr g_wardenScanModule;
	};
}*/

BOOL Warden::HookWardenScan(DWORD base)
{
	// We need three global variables to store the results obtained from the scan, then however many addresses that need to be protected.
	// Store the local tick count in a variable so dinner can check the last time a scan was performed. This is also to be sure that warden's scan is hooked properly.

	BYTE wardenScanSub[] = {
		//
		0x89, 0x35,	0x00, 0x00, 0x00, 0x00,							// mov scanAddress, esi
		0x89, 0x15, 0x00, 0x00, 0x00, 0x00,							// mov scanSize, edx
																	// mov scanTimestamp, [0x00B1D618]

		// Warden scan code:
		// Scans the values at edx and stores them into edi.
		// Before each scan, we save the scan result pointer so we can patch it later.

		0x74, 0x02,													// je loc1
		0x89, 0x3D, 0x00, 0x00, 0x00, 0x00,							// mov [scanResults],edi
		0xF3, 0xA5,													// repe movsd (Stores scanSize words into [edi])
		0xB1, 0x03,													// mov cl,03 - loc1
		0x23, 0xCA,													// and ecx,edx
		
		0x74, 0x02,													// je loc2
		0x89, 0x3D, 0x00, 0x00, 0x00, 0x00,							// mov [scanResults],edi
		0xF3, 0xA4,													// repe movsb (Stores scanSize bytes into [edi])
		0x5F,														// pop edi - loc2
		0x5E,														// pop esi

		0x60,														// pushad
		0x9C,														// pushfd

		0x81, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // cmp [00000000],00000000
		0x75, 0x3B,													// jne wardenScanSub+55 ************* Need to figure out proper jump distance once finished.
		0x61,														// popfd
		0x9D,														// popfd
		
		0x0F, 0xB6, 0x05, 0x00, 0x00, 0x00, 0x00,					// movzx eax,byte ptr [scanSize]
		0x50,														// push eax
		0x68, 0x00, 0x00, 0x00, 0x00,								// push 
		0x68, 0x00, 0x00, 0x00, 0x00,								// push scanResults
	};

	return 1;
}

DWORD Warden::GetScanModule()
{
	// Don't need pattern scanning anymore.
	/*BYTE pattern[] = {0x74, 0x02, 0xF3, 0xA5, 0xB1, 0x03, 0x23, 0xCA};
	char mask[] = {'x' , 'x' , 'x' , 'x' , 'x' , 'x' , 'x' , 'x'};
	
	return PatternScanRemote(pattern, mask, sizeof(pattern));	*/

	return (Warden::GetBase() != NULL)? (Warden::GetBase() + ScanFunctionOffset): NULL;
}

BOOL WardenData()
{
	DWORD  callAddress = Offsets::Warden__Data;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallBytes[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwWardenShutdownAddress		
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH num
			0x68, 0x00,	0x00, 0x00, 0x00,		// PUSH ptr
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallBytes);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	memcpy(&Sub_CallBytes[24], &callAddress, sizeof(callAddress));

	if (dwSubroutineAddress == NULL)
		return FALSE;

	if (!wpm(dwSubroutineAddress, &Sub_CallBytes, sizeof(Sub_CallBytes)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;
	
	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return !Warden::IsLoaded();
}

BOOL Warden::Unload()
{
	DWORD  dwWardenShutdownAddress = Offsets::Warden__ShutdownAndUnload;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallWardenShutdown[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwWardenShutdownAddress				
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallWardenShutdown);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	memcpy(&Sub_CallWardenShutdown[24], &dwWardenShutdownAddress, sizeof(dwWardenShutdownAddress));

	if (dwSubroutineAddress == NULL)
		return FALSE;

	if (!wpm(dwSubroutineAddress, &Sub_CallWardenShutdown, sizeof(Sub_CallWardenShutdown)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;
	
	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return !Warden::IsLoaded();
}

BOOL Warden::IsHackProtected(DWORD base)
{
	ScanCavePtr;
	return false;
}

BOOL Warden::Load()
{
	DWORD  dwWardenShutdownAddress = Offsets::WardenClient_Initialize;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallWardenShutdown[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwWardenShutdownAddress				
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallWardenShutdown);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	memcpy(&Sub_CallWardenShutdown[24], &dwWardenShutdownAddress, sizeof(dwWardenShutdownAddress));

	if (dwSubroutineAddress == NULL)
		return FALSE;

	if (!wpm(dwSubroutineAddress, &Sub_CallWardenShutdown, sizeof(Sub_CallWardenShutdown)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return Warden::IsLoaded();
}