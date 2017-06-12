#pragma once

#include <Windows.h>

LRESULT CALLBACK ObjectWatchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern CHAR g_szSearch[MAX_PATH];

extern BOOL g_Searching;
extern BOOL g_Transport;
extern BOOL g_OWReport;

extern HWND g_hwObjectWatch;