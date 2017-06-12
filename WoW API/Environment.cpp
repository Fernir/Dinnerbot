#include "Environment.h"
#include "..\WoW API\Chat.h"

using namespace Offsets;

FLOAT g_fFogDensity = 1.0f;

BYTE m_FogProtectionBytes[4][6];

/* Fogcolor protection:

				_memorywrite($fogcolor_protection, $wow, "0x9090909090", "byte[5]")
				_memorywrite($fogcolor_protection + 5, $wow, "0x909090909090", "byte[6]")
				_memorywrite($fogcolor_protection - 12, $wow, "0x909090909090", "byte[6]")
				_memorywrite($fogcolor_protection + 76, $wow, "0x909090909090", "byte[6]")
				*/

VOID FogDensityChange(HWND hWnd)
{
	CHAR szBuffer[64];

	FLOAT fDensity = 1.0f;

	// Get input.
	GetDlgItemTextA(hWnd, IDC_GENERAL_EDIT_INPUT, szBuffer, 64);

	if (isfloat(szBuffer))
		fDensity = (FLOAT)atof(szBuffer);

	else if (isinteger(szBuffer))
		fDensity = (FLOAT)atoi(szBuffer);

	else return;
	
	sprintf_s(szBuffer, "FOG DENSITY: %f", fDensity);
	LogAppend(szBuffer);

	if (!IsFogColorPatched())
		PatchFogColorProtection();

	g_fFogDensity = fDensity;
	if (fDensity == 1 && IsFogColorPatched())
		RestoreFogColorProtection();
	else
		SetFogDensity(fDensity);

	sprintf_s(szBuffer, "Fog Density changed to %f.", g_fFogDensity);
	AddChatMessage(szBuffer, "DMagic", LightPurpleMessage);
}

BOOL IsFogColorPatched()
{
	BYTE aBuffer[4][6];
	BYTE aNOPSlide[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};

	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch), &aBuffer[0], sizeof(aBuffer[0]), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch + 5), &aBuffer[1], sizeof(aBuffer[1]), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch - 0x0C), &aBuffer[2], sizeof(aBuffer[2]), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch + 0x4C), &aBuffer[3], sizeof(aBuffer[3]), NULL);

	for (int x = 0; x < 4; x++)
	{
		if (memcmp(aBuffer[x], aNOPSlide, sizeof(aBuffer[x])) != 0)
			return FALSE;
	}

	return TRUE;
}

BOOL PatchFogColorProtection()
{
	BYTE aNOPSlide[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	
	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch), &m_FogProtectionBytes[0], sizeof(m_FogProtectionBytes[0]), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch + 5), &m_FogProtectionBytes[1], sizeof(m_FogProtectionBytes[1]), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch - 0x0C), &m_FogProtectionBytes[2], sizeof(m_FogProtectionBytes[2]), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(FogColorPatch + 0x4C), &m_FogProtectionBytes[3], sizeof(m_FogProtectionBytes[3]), NULL);

	SuspendWoW();
	WriteProcessMemory(WoW::handle, (LPVOID)(FogColorPatch), &aNOPSlide, sizeof(aNOPSlide), NULL);
	WriteProcessMemory(WoW::handle, (LPVOID)(FogColorPatch + 5), &aNOPSlide, sizeof(aNOPSlide), NULL);
	WriteProcessMemory(WoW::handle, (LPVOID)(FogColorPatch - 0x0C), &aNOPSlide, sizeof(aNOPSlide), NULL);
	WriteProcessMemory(WoW::handle, (LPVOID)(FogColorPatch + 0x4C), &aNOPSlide, sizeof(aNOPSlide), NULL);
	ResumeWoW();

	return TRUE;
}

BOOL RestoreFogColorProtection()
{
	if (!IsFogColorPatched()) return FALSE;

	SuspendWoW();
	SetFogDensity(1.0f);
	wpm(FogColorPatch       , &m_FogProtectionBytes[0], sizeof(m_FogProtectionBytes[0]));
	wpm(FogColorPatch + 5   , &m_FogProtectionBytes[1], sizeof(m_FogProtectionBytes[1]));
	wpm(FogColorPatch - 0x0C, &m_FogProtectionBytes[2], sizeof(m_FogProtectionBytes[2]));
	wpm(FogColorPatch + 0x4C, &m_FogProtectionBytes[3], sizeof(m_FogProtectionBytes[3]));
	ResumeWoW();

	return TRUE;
}

BOOL SetFogDensity(FLOAT fDensity)
{
	if (!IsFogColorPatched()) return false;

	WriteProcessMemory(WoW::handle, (LPVOID)(FogDensity), &fDensity, sizeof(fDensity), NULL);
	return TRUE;
}

FLOAT GetFogDensity()
{
	FLOAT fDensity = 0;

	ReadProcessMemory(WoW::handle, (LPVOID)(FogDensity), &fDensity, sizeof(fDensity), NULL);
	return fDensity;
}