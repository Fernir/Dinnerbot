#pragma once

#include "..\Common\Common.h"

FLOAT GetFogDensity();

BOOL IsFogColorPatched();
BOOL PatchFogColorProtection();
BOOL RestoreFogColorProtection();
BOOL SetFogDensity(FLOAT fDensity);

VOID FogDensityChange(HWND hWnd);

extern FLOAT g_fFogDensity;