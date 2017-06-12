#pragma once
#include <Windows.h>

#include "WoW API\Object Classes\Unit.h"

#define D_UNKNOWN	0xFF
#define D_JUMPING	0x03
#define D_FOLLOWING 0x02
#define D_RUNNING	0x01
#define D_IDLE		0x00

#define BG_NOT_QUEUED 0x00
#define BG_QUEUED	  0x01
#define BG_QUEUE_POP  0x02

BOOL GetWoWWindow();
BOOL InitializeBot();
WNDENUMPROC CALLBACK WoWWindowProc(HWND hWnd, LPARAM lParam);

namespace Test
{
	VOID Debug();
	VOID SpawnKhadgarServant();
};

extern CUnit	LocalPlayer;
extern HWND		g_hwWoW;
extern DWORD	g_dwCurrentManager;
