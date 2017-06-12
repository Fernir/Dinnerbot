#pragma once

#include <Windows.h>

#define WARDEN_TIMEOUT 180000

class Warden
{

private:
	static const DWORD BasePtr = 0xD31A48;							// 3.3.5a 12340
	static const DWORD ScanFunctionOffset = 0x2A95;					// 3.3.5a 12340 (WardenBase + WardenScanOffset = WardenScan
	static const DWORD ClassPtr = 0x00D31A4C;
	static const DWORD ScanCavePtr = 0x00000000;

	static BOOL HookWardenScan(DWORD base);

public:
	static DWORD GetScanModule();
	static DWORD GetClass();
	static DWORD IsLoaded();
	static DWORD GetBase();

	static BOOL IsHackProtected(DWORD base);
	static BOOL Unload();
	static BOOL Load();
};