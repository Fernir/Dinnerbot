#pragma once 

#include <Windows.h>
#include "..\Common\Common.h"
#include "..\WoW API\Object Classes\Object.h"

typedef struct 
{
	BYTE JmpBackPatch[5];
	BYTE RestorePatch[50];
	BYTE JmpToPatch[20];
	DWORD HookedFunction;
	DWORD Cave;
	DWORD CaveSize;
	DWORD RestoreSize;
	DWORD JmpToSize;
} WHook;

BOOL CleanAllPatches();
BOOL HookClickToMove();
BOOL HookLanguageHack();
BOOL HookWorldFrameRender();
BOOL HookDefaultServerLogin();
BOOL CleanPatch(DWORD dwHookLocation);
BOOL IsSubroutineHooked(DWORD dwDestination);
BOOL UnhookSubroutine(DWORD dwAddress, WHook hook);
BOOL ChangeHookFunction(DWORD dwHookedFunction, DWORD dwCodecave, BYTE *bNewFunctionCode, size_t uFunctionSize);

extern WHook wDefaultServerLogin, wWorldFrame, wClickToMove, wLanguage, wEndScene;
