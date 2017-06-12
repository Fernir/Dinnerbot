#include "ClickToMove.h"
#include "Common\Common.h"

#include <Windows.h>

using namespace Memory;
using namespace Engine;

DWORD Navigation::GetAction()
{
	DWORD buffer = NULL;

	rpm(Offsets::CTM_Base + Offsets::CTM_Action, &buffer, sizeof(buffer));
	return buffer;
}

bool Navigation::idle()
{
	DWORD buffer = GetAction();
	return buffer == Offsets::CTM_Idle || buffer == Offsets::CTM_None;
}

bool Navigation::move(const WOWPOS &p)
{
	return Navigation::move(p.X, p.Y, p.Z);
}

bool Navigation::face(const WOWPOS &p)
{
	return Navigation::face(p.X, p.Y, p.Z);
}

bool Navigation::face(float x, float y, float z)
{
	vlog("x, y, z: %f %f %f", x, y, z);
	return Navigation::ctm(x, y, z, CTM_Face);
}

bool Navigation::move(float X, float Y, float Z)
{
	return Navigation::ctm(X, Y, Z, CTM_Move);
}

bool Navigation::setTolerance(float tol)
{
	return wpm(Offsets::CTM_Tolerance, &tol, sizeof(tol));
}

bool Navigation::ctm(float X, float Y, float Z, byte action)
{
	DWORD  CTMAddress = Offsets::CGPlayer_C__ClickToMove;
	DWORD  vectorAddress = NULL;
	DWORD  guidLocation = NULL;
	DWORD  codeAddress = NULL;
	DWORD  totalSize = NULL;
	HANDLE thread = NULL;

	if (!WoW::InGame())
		return false;

	// __thiscall
	BYTE Sub_CallClickToMove[] = {
		// Updates TLS 
		0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
		0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
		0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
		0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
		0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
		0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress	
		0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ECX, dwObjectBase
		0x51,								// PUSH ECX							DWORD this_PlayerBase
		0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Pos							_D3DVECTOR Pos	- Arg3
		0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Unknown						DWORD Unknown	- Arg2
		0x6A, 0x00,							// PUSH CTM_State					DWORD CTM_STATE	- Arg1
		0xFF, 0xD7,							// CALL EDI
		//0x83, 0xC4, 0x10,					// ADD ESP, 10
		0xC3								// RETN
	};

	totalSize = sizeof(Sub_CallClickToMove)+4 * sizeof(float)+sizeof(guidLocation);
	codeAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, totalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	guidLocation = codeAddress + sizeof(Sub_CallClickToMove) + sizeof(X) + sizeof(Y) + sizeof(Z);

	// Unable to allocate memory.
	if (codeAddress == NULL)
		return false;

	// Inject position vector.
	if (!wpm(codeAddress + sizeof(Sub_CallClickToMove), &X, sizeof(X)))
		return false;

	if (!wpm(codeAddress + sizeof(Sub_CallClickToMove) + sizeof(X), &Y, sizeof(Y)))
		return false;

	if (!wpm(codeAddress + sizeof(Sub_CallClickToMove) + sizeof(X) + sizeof(Y), &Z, sizeof(Z)))
		return false;

	// Inject guid.
	if (!wpm(codeAddress + sizeof(Sub_CallClickToMove)+sizeof(X)+sizeof(Y)+sizeof(Z), &guidLocation, sizeof(guidLocation)))
		return false;

	// Inject ?
	/*if (!wpm(codeAddress + totalSize, &thread, sizeof(thread)))
		return false;*/

	// Fill in code parameters.
	vectorAddress = codeAddress + sizeof(Sub_CallClickToMove);
	memcpy(&Sub_CallClickToMove[24], &CTMAddress, sizeof(CTMAddress));
	memcpy(&Sub_CallClickToMove[29], &LocalPlayer.Obj.BaseAddress, sizeof(LocalPlayer.base()));
	memcpy(&Sub_CallClickToMove[35], &vectorAddress, sizeof(vectorAddress));
	memcpy(&Sub_CallClickToMove[40], &guidLocation, sizeof(guidLocation));
	memcpy(&Sub_CallClickToMove[45], &action, sizeof(action));

	// Inject code.
	if (!wpm(codeAddress, &Sub_CallClickToMove, sizeof(Sub_CallClickToMove)))
		return false;

	// Suspend WoW and execute remote thread at our code.
	SuspendWoW();
	thread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)codeAddress, NULL, NULL, NULL);

	if (thread == NULL)
		return false;

	WaitForSingleObject(thread, 1000);
	CloseHandle(thread);

	// Resume WoW and free the memory we allocated.
	ResumeWoW();
	return VirtualFreeEx(WoW::handle, (LPVOID)codeAddress, totalSize, MEM_RELEASE);
}