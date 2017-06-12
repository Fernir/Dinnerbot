#pragma once

#include <Windows.h>
#include <string>

#include "Main\Structures.h"

VOID UpdateMainDisplay(HWND hWnd);

VOID onDebugPlayerDumpButton();
VOID onDumpBaseObjectButton();
VOID onDumpUnitFieldButton();
VOID onDumpLocationButton();
VOID onDumpAurasButton();

std::string getFishingProfileFromCombo();

DWORD GetDlgCheck(HWND hWnd, uint dlgEntry);
DWORD SetDlgCheck(HWND hWnd, uint dlgEntry, DWORD set);

BOOL GetMagicCheck(INT dlgItem);
BOOL SetMagicCheck(int dlgItem, bool state);

extern CHAR g_lastAccount[256];
extern CHAR g_lastPass[256];