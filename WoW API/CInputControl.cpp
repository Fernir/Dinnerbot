#include "CInputControl.h"
#include "Common\Common.h"

#include "Memory\Memory.h"
#include "Memory\Endscene.h"

using namespace Memory;

DWORD GetCInputControl()
{
	DWORD buffer = NULL;

	rpm(Offsets::CInputControl, &buffer, sizeof(buffer));
	return buffer;
}

void CInputControl::setMovementFlags()
{
	DWORD executeAddress = Offsets::CInputControl__SetControlBit;

	if (!WoW::InGame()) return;

	if (!Endscene.CanExecute() || !Endscene.IsHandlerStarted()) return;

	BYTE code[] =
	{
		/** Call function segment. **/
		0xBF, 0x00, 0x00, 0x00, 0x00,		// mov edi, executeAddress
		0x8B, 0x35, 0xA4, 0x99, 0xB4, 0x00,	// mov esi, [g_LastHardwareAction]
		0x8B, 0x0D, 0x54, 0x49, 0xC2, 0x00,	// mov ecx, [g_CInputControl]
		0x6A, 0x01,							// push 01
		0x56,								// push esi
		0xFF, 0xD7,							// call edi
		0xC3								// retn
	};

	DWORD timeStamp = OsGetAsyncTimeMs();
	wpm(Offsets::LastHardwareAction, &timeStamp, sizeof(timeStamp));

	memcpy(&code[1], &executeAddress, sizeof(executeAddress));

	// Create stub whose destination is the Endscene function space, clear the space,
	// then inject stub into space.
	Stub stub(Endscene.getFunctionSpace(), code, sizeof(code));
	Endscene.ClearFunctionSpace();
	stub.inject();

	// Execute stub.
	Endscene.Execute(stub);
}

// Used to update player after teleporting. Previous method of SendKey('A', ...) was not very elegant.
void CInputControl::updatePlayer()
{
	// CInputControl::UpdatePlayer(g_CInputControl, g_LastHardwareAction, 1);
	DWORD executeAddress = Offsets::CInputControl__UpdatePlayer;

	if (!WoW::InGame()) return; 

	if (!Endscene.CanExecute() || !Endscene.IsHandlerStarted()) return;

	BYTE code[] = 
	{
		/** Call function segment. **/
			0xBF, 0x00, 0x00, 0x00, 0x00,		// mov edi, executeAddress
			0x8B, 0x35, 0xA4, 0x99, 0xB4, 0x00,	// mov esi, [g_LastHardwareAction]
			0x8B, 0x0D, 0x54, 0x49, 0xC2, 0x00,	// mov ecx, [g_CInputControl]
			0x6A, 0x01,							// push 01
			0x56,								// push esi
			0xFF, 0xD7,							// call edi
			0xC3								// retn
	};

	DWORD timeStamp = OsGetAsyncTimeMs();
	wpm(Offsets::LastHardwareAction, &timeStamp, sizeof(timeStamp));

	memcpy(&code[1], &executeAddress, sizeof(executeAddress));

	// Create stub whose destination is the Endscene function space, clear the space,
	// then inject stub into space.
	Stub stub(Endscene.getFunctionSpace(), code, sizeof(code));
	Endscene.ClearFunctionSpace();
	stub.inject();

	// Execute stub.
	Endscene.Execute(stub);
	return;
}

