#pragma once

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

#include <Windows.h>
#include "Structures.h"
#include <Geometry\Coordinate.h>
#include <vector>
#include <string>

// Build mode
//#define RELEASE

// Build version
#define RELEASE_VERSION 0
#define MAJOR_VERSION 4
#define MINOR_VERSION 0

#define IDT_TIMER 0x100

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#define _CRT_SECURE_NO_WARNINGS			// God damn do I hate deprecation warnings.

VOID WoWInfoLoop();
VOID UncheckMagic();
VOID onKeyDown_Up(HWND hWnd);
VOID onKeyDown_Down(HWND hWnd);
VOID HandleCommandInput_Wrapper(CONST CHAR *msg);
VOID CALLBACK MainTimerProc(HWND hWnd, UINT uMessage, UINT_PTR uEventId, DWORD dwTime);

namespace Dinner
{
	extern std::string versionName;
	extern std::vector<std::string> names;

	std::string getVersionName();
	std::string &getDankName();
};

// Dinnerbot properties.
extern BOOL ReleaseMode;

extern HWND g_hwMainWindow;

// Hack settings.
extern BOOL g_VerboseInGameChat;
extern BOOL g_InGameChat;
extern BOOL g_LogVisible;

extern INT g_relogger;
extern INT g_mapTeleport;

extern BOOL g_VFly;
extern BOOL g_LuaCVar;
extern BOOL g_AntiAFK;
extern BOOL g_chatLog;
extern BOOL g_bFlyHack;
extern BOOL g_DFish_Hide;
extern BOOL g_rosterMode;
extern BOOL g_bPreAnimate;
extern BOOL g_DFish_Logout;
extern BOOL g_updateWoWChat;

extern WOWPOS g_TeleportPos;

extern HINSTANCE g_hInst;
