#include "Hooks.h"

#include "Memory.h"
#include "Endscene.h"
#include "Hacks.h"
#include "..\WoW API\Chat.h"
#include "..\WoW API\Environment.h"

using namespace Memory;

WHook wDefaultServerLogin, wWorldFrame, wClickToMove, wLanguage, wEndScene;

// Unhook function.
BOOL RestoreCodeFlow(LPVOID lpDestination, BYTE *bRestore, size_t uRestoreSize)
{
	DWORD dwProtect = PAGE_EXECUTE_READWRITE;

    if (!VirtualProtectEx(WoW::handle, (LPVOID)lpDestination, uRestoreSize, PAGE_EXECUTE_READWRITE, &dwProtect))
		return FALSE;

	if (!wpm((DWORD)lpDestination, &*bRestore, uRestoreSize))
		return FALSE;

    if (!VirtualProtect((LPVOID)lpDestination, uRestoreSize, dwProtect, NULL))
		return FALSE;
	return TRUE;
}

// Writes jmp patch to lpDestination to jump to the code at lpSource.
BOOL RedirectCodeFlow(LPVOID lpDestination, LPVOID lpSource, BYTE *JmpToPatch, size_t patchSize)
{
	// Jump from source address to destination address.
	DWORD dwProtect = PAGE_EXECUTE_READWRITE;
    DWORD dwRelativeJmp = ((DWORD)lpDestination - (DWORD)lpSource - 5);

    if (!VirtualProtectEx(WoW::handle, (LPVOID)lpSource, patchSize, PAGE_EXECUTE_READWRITE, &dwProtect))
		return FALSE;

    memcpy(&JmpToPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(lpSource), JmpToPatch, patchSize, 0)))
		return FALSE;
	
    if (!VirtualProtect((LPVOID)lpSource, patchSize, dwProtect, NULL))
		return FALSE;
	return TRUE;
}

// Hook function
BOOL RedirectCodeFlow(LPVOID lpDestination, LPVOID lpSource)
{
	DWORD dwProtect = PAGE_EXECUTE_READWRITE;
    DWORD dwRelativeJmp = ((DWORD)lpDestination - (DWORD)lpSource - 5);
    BYTE  bJmpPatch[] = {0xE9, 0x90, 0x90, 0x90, 0x90, 0x90};

    if (!VirtualProtectEx(WoW::handle, lpSource, sizeof(bJmpPatch), PAGE_EXECUTE_READWRITE, &dwProtect))
		return FALSE;

    memcpy(&bJmpPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
	if (!wpm((DWORD)lpSource, &bJmpPatch, sizeof(bJmpPatch)))
		return FALSE;
	
    if (!VirtualProtect(lpSource, sizeof(bJmpPatch), dwProtect, NULL))
		return FALSE;
	return TRUE;
}

// Change cave code.
BOOL ChangeHookFunction(DWORD dwHookedFunction, DWORD dwCodecave, BYTE *bNewFunctionCode, size_t uFunctionSize)
{
	// NewFunctionCode is for adding in code later. DO NOT REMOVE.
	// Changes code of hooked function. If the given function is not already hooked, then it is hooked.
	DWORD dwRelativeJmp = NULL;

	if (bNewFunctionCode == NULL && uFunctionSize != 0)
		uFunctionSize = 0;

	if (dwHookedFunction == Endscene.getAddress())
	{
		// Jump back to original subroutine.
		dwRelativeJmp = Endscene.getAddress() - (wEndScene.Cave + uFunctionSize + wEndScene.RestoreSize);
		memcpy(&wEndScene.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
		
		// Write our function code.
		if (bNewFunctionCode != NULL)
			if (!wpm(dwCodecave, &*bNewFunctionCode, uFunctionSize))
				return FALSE;

		// Write endscene's original code.
		if (!wpm(dwCodecave + uFunctionSize, &wEndScene.RestorePatch, wEndScene.RestoreSize))
			return FALSE;
		
		// Write the patch to jump back to endscene.
		if (!wpm(dwCodecave + wEndScene.RestoreSize + uFunctionSize, &wEndScene.JmpBackPatch, sizeof(wEndScene.JmpBackPatch)))
			return FALSE;

		wEndScene.CaveSize = wEndScene.RestoreSize + uFunctionSize + sizeof(wEndScene.JmpBackPatch);
		return TRUE;
	}

	switch (dwHookedFunction)
	{
	case Offsets::CGlueMgr__DefaultServerLogin:
		// Jump back to original subroutine.
		dwRelativeJmp = Offsets::CGlueMgr__DefaultServerLogin - (wDefaultServerLogin.Cave + uFunctionSize + wDefaultServerLogin.RestoreSize);
		memcpy(&wDefaultServerLogin.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
		
		if (bNewFunctionCode != NULL)
			if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave), &*bNewFunctionCode, uFunctionSize, 0)))
				return FALSE;

		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + uFunctionSize), &wDefaultServerLogin.RestorePatch, wDefaultServerLogin.RestoreSize, 0)))
			return FALSE;
		
		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + wDefaultServerLogin.RestoreSize + uFunctionSize), &wDefaultServerLogin.JmpBackPatch, sizeof(wDefaultServerLogin.JmpBackPatch), 0)))
			return FALSE;

		wDefaultServerLogin.CaveSize = wDefaultServerLogin.RestoreSize + uFunctionSize + sizeof(wDefaultServerLogin.JmpBackPatch);
		break;

	case Offsets::CGGameChat__AddChatMessage:
		// Jump back to original subroutine.
		dwRelativeJmp = Offsets::CGGameChat__AddChatMessage + (wLanguage.RestoreSize - sizeof(wLanguage.JmpBackPatch)) - (wLanguage.Cave + uFunctionSize + wLanguage.RestoreSize);
		memcpy(&wLanguage.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
		
		if (bNewFunctionCode != NULL)
			if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave), &*bNewFunctionCode, uFunctionSize, 0)))
				return FALSE;

		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + uFunctionSize), &wLanguage.RestorePatch, wLanguage.RestoreSize, 0)))
			return FALSE;
		
		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + wLanguage.RestoreSize + uFunctionSize), &wLanguage.JmpBackPatch, sizeof(wLanguage.JmpBackPatch), 0)))
			return FALSE;

		wLanguage.CaveSize = wLanguage.RestoreSize + uFunctionSize + sizeof(wLanguage.JmpBackPatch);
		break;

	case Offsets::CGWorldFrame__OnFrameRender:
		dwRelativeJmp = Offsets::CGWorldFrame__OnFrameRender - (wWorldFrame.Cave + uFunctionSize + wWorldFrame.RestoreSize);
		memcpy(&wWorldFrame.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
		
		if (bNewFunctionCode != NULL)
			if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave), &*bNewFunctionCode, uFunctionSize, 0)))
				return FALSE;

		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + uFunctionSize), &wWorldFrame.RestorePatch, wWorldFrame.RestoreSize, 0)))
			return FALSE;
		
		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + wWorldFrame.RestoreSize + uFunctionSize), &wWorldFrame.JmpBackPatch, sizeof(wWorldFrame.JmpBackPatch), 0)))
			return FALSE;

		wWorldFrame.CaveSize = wWorldFrame.RestoreSize + uFunctionSize + sizeof(wWorldFrame.JmpBackPatch);
		break;

	case Offsets::CGPlayer_C__ClickToMove:
		// Relative jump distance.
		dwRelativeJmp = Offsets::CGPlayer_C__ClickToMove - (wClickToMove.Cave + uFunctionSize + wClickToMove.RestoreSize);
		memcpy(&wClickToMove.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp)); // Create JMP patch with relative distance.
		
		if (bNewFunctionCode != NULL)
			if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave), &*bNewFunctionCode, uFunctionSize, 0)))
				return FALSE;

		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + uFunctionSize), &wClickToMove.RestorePatch, wClickToMove.RestoreSize, 0)))
			return FALSE;
		
		if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCodecave + wClickToMove.RestoreSize + uFunctionSize), &wClickToMove.JmpBackPatch, sizeof(wClickToMove.JmpBackPatch), 0)))
			return FALSE;
		
		wClickToMove.CaveSize = wClickToMove.RestoreSize + uFunctionSize + sizeof(wClickToMove.JmpBackPatch);
		break;

	}

	return TRUE;
}

BOOL HookWorldFrameRender()
{   
	DWORD dwRelativeJmp = NULL;
	BYTE bPatch[] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	
	memset(&wWorldFrame, 0, sizeof(wWorldFrame));
	memcpy(&wWorldFrame.JmpBackPatch, &bPatch, sizeof(bPatch));

	// Allocate memory for cave. (1 KB is more than enough)
	wWorldFrame.Cave = (DWORD)VirtualAllocEx(WoW::handle, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (wWorldFrame.Cave == NULL)
		return FALSE;
	
	// Calculate relative distance from the Render function and the allocated cave.
	dwRelativeJmp = Offsets::CGWorldFrame__OnFrameRender - wWorldFrame.Cave - 5;

	// Make a copy of the data for future restoration.
	wWorldFrame.RestoreSize = 6;
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::CGWorldFrame__OnFrameRender), &wWorldFrame.RestorePatch, wWorldFrame.RestoreSize, 0))
		return FALSE;

	memcpy(&wWorldFrame.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
	ChangeHookFunction(Offsets::CGWorldFrame__OnFrameRender, wWorldFrame.Cave, NULL, 0);	// Replace null jmp with cave location.
	RedirectCodeFlow((LPVOID)wWorldFrame.Cave, (LPVOID)Offsets::CGWorldFrame__OnFrameRender);

	log("Hooked CGWorldFrame__Render");
	LogFile("Hooked CGWorldFrame__Render");
	return TRUE;
}

BOOL IsSubroutineHooked(DWORD dwDestination)
{
	BYTE bBuffer = 0x00;

	if (WoW::handle == NULL) return false;

	if (!rpm(dwDestination, &bBuffer, sizeof(BYTE)))
		return true;

	if (bBuffer == 0xE9)
		return true;

	return false;
}

/* TODO: Generalize the hook functions 

	WHOOK array for CleanAllHooks, etc. */
BOOL HookDefaultServerLogin()
{
	DWORD dwRelativeJmp = NULL;
	CHAR szBuffer[64];
	BYTE bPatch[] = {0xE9, 0x90, 0x90, 0x90, 0x90,};
	BYTE bPatch2[] = {0xE9, 0x90, 0x90, 0x90, 0x90, /* Extra bytes --->> */ 0x90, 0x90, 0x90, 0x90, 0x90}; // We need exactly 2 extra bytes of padding so that the following asm executes properly.
	
	memset(&wDefaultServerLogin, 0, sizeof(wDefaultServerLogin));
	memcpy(&wDefaultServerLogin.JmpToPatch, &bPatch2, sizeof(bPatch2));
	memcpy(&wDefaultServerLogin.JmpBackPatch, &bPatch, sizeof(bPatch));

	// Allocate memory for cave. (1 KB is more than enough)
	wDefaultServerLogin.Cave = (DWORD)VirtualAllocEx(WoW::handle, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (wDefaultServerLogin.Cave == NULL)
		return FALSE;
	
	sprintf(szBuffer, "HookDefaultServerLogin: Cave = 0x%p", wDefaultServerLogin.Cave);
	LogFile(szBuffer);
	// Calculate relative distance from the CTM function and the allocated cave.
	dwRelativeJmp = Offsets::CGlueMgr__DefaultServerLogin - wDefaultServerLogin.Cave - 5;

	// Make a copy of the data for future restoration.
	wDefaultServerLogin.RestoreSize = 10;
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::CGlueMgr__DefaultServerLogin), &wDefaultServerLogin.RestorePatch, wDefaultServerLogin.RestoreSize, 0))
		return FALSE;

	// Set cave code to wDefaultServerLogin.Cave.
	memcpy(&wDefaultServerLogin.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
	ChangeHookFunction(Offsets::CGlueMgr__DefaultServerLogin, wDefaultServerLogin.Cave, NULL, 0);

	// Jmp patch directing to our cave.
	RedirectCodeFlow((LPVOID)wDefaultServerLogin.Cave, (LPVOID)Offsets::CGlueMgr__DefaultServerLogin, bPatch2, sizeof(bPatch2));

	log("Hooked Offsets::CGlueMgr__DefaultServerLogin");
	LogFile("HookDefaultServerLogin: Hooked Offsets::CGlueMgr__DefaultServerLogin");
	return TRUE;
}

/*
BOOL HookEndScene()
{   
	DWORD dwRelativeJmp = NULL;
	BYTE bPatch[] = {0xE9, 0x90, 0x90, 0x90, 0x90};
	BYTE bPatch2[] = {0xE9, 0x90, 0x90, 0x90, 0x90, /* Extra bytes --->> */ /*0x90, 0x90}; // We need exactly 2 extra bytes so that the following asm executes properly.
	
	memset(&wEndScene, 0, sizeof(wEndScene));
	memcpy(&wEndScene.JmpBackPatch, &bPatch, sizeof(bPatch));
	memcpy(&wEndScene.JmpToPatch, &bPatch2, sizeof(bPatch2));

	wEndScene.JmpToSize = sizeof(bPatch2);

	// Allocate memory for cave. (1 KB is more than enough)
	wEndScene.Cave = (DWORD)VirtualAllocEx(WoW::handle, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (wEndScene.Cave == NULL)
	{
		vflog("HookEndScene: VirtualAllocEx error: %d", GetLastError());
		return false;
	}
	
	vlog("Cave: 0x%p", wEndScene.Cave);
	vflog("wEndScene.Cave: 0x%p", wEndScene.Cave);

	// Calculate relative distance from the language function and the allocated cave.
	dwRelativeJmp = g_dwEndScene - wEndScene.Cave - 5;
	memcpy(&wEndScene.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));

	// Make a copy of the data for future restoration. Some functions require more or less bytes to be copied in order to protect the integrity of code after the jmp.
	wEndScene.RestoreSize = 7;
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(g_dwEndScene), &wEndScene.RestorePatch, wEndScene.RestoreSize, 0))
	{
		vflog("HookEndScene: RPM error: %d", GetLastError());
		return FALSE;
	}

	SuspendWoW();
	ChangeHookFunction(g_dwEndScene, wEndScene.Cave, NULL, 0);	// Replace null jmp with language hack location.
	RedirectCodeFlow((LPVOID)wEndScene.Cave, (LPVOID)g_dwEndScene, wEndScene.JmpToPatch, wEndScene.JmpToSize);
	ResumeWoW();

	log("Hooked EndScene");
	LogFile("HookEndScene: Hooked EndScene");
	return TRUE;
}*/

BOOL HookLanguageHack()
{   
	DWORD dwRelativeJmp = NULL;
	CHAR szBuffer[64];
	BYTE bPatch[] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	
	memset(&wLanguage, 0, sizeof(wLanguage));
	memcpy(&wLanguage.JmpBackPatch, &bPatch, sizeof(bPatch));

	// Allocate memory for cave. (1 KB is more than enough)
	wLanguage.Cave = (DWORD)VirtualAllocEx(WoW::handle, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (wLanguage.Cave == NULL)
		return FALSE;
	
	sprintf(szBuffer, "HookLanguageHack: Cave = 0x%x", wLanguage.Cave);
	LogFile(szBuffer);

	// Calculate relative distance from the language function and the allocated cave.
	dwRelativeJmp = Offsets::CGGameChat__AddChatMessage - wLanguage.Cave - 5;

	// Make a copy of the data for future restoration.
	wLanguage.RestoreSize = 8;
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::CGGameChat__AddChatMessage), &wLanguage.RestorePatch, wLanguage.RestoreSize, 0))
		return FALSE;

	memcpy(&wLanguage.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));
	ChangeHookFunction(Offsets::CGGameChat__AddChatMessage, wLanguage.Cave, NULL, 0);	// Replace null jmp with language hack location.
	RedirectCodeFlow((LPVOID)wLanguage.Cave, (LPVOID)Offsets::CGGameChat__AddChatMessage);

	log("Hooked CGGameChat__AddChatMessage");
	LogFile("HookLanguageHack: Hooked CGGameChat__AddChatMessage");
	return TRUE;
}

BOOL HookClickToMove()
{   
	DWORD dwRelativeJmp = NULL;
	BYTE bPatch[] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	
	memset(&wClickToMove, 0, sizeof(wClickToMove));
	memcpy(&wClickToMove.JmpBackPatch, &bPatch, sizeof(bPatch));

	// Allocate memory for cave. (1 KB is more than enough)
	wClickToMove.Cave = (DWORD)VirtualAllocEx(WoW::handle, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (wClickToMove.Cave == NULL)
		return FALSE;
	
	vflog("HookClickToMove: Cave = 0x%p", wClickToMove.Cave);

	// Calculate relative distance from the CTM function and the allocated cave.
	dwRelativeJmp = Offsets::CGPlayer_C__ClickToMove - wClickToMove.Cave - 5;

	// Make a copy of the data for future restoration.
	wClickToMove.RestoreSize = 6;
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::CGPlayer_C__ClickToMove), &wClickToMove.RestorePatch, wClickToMove.RestoreSize, 0))
		return FALSE;

	memcpy(&wClickToMove.JmpBackPatch[1], &dwRelativeJmp, sizeof(dwRelativeJmp));

	// Insert our code into the new cave.
	ChangeHookFunction(Offsets::CGPlayer_C__ClickToMove, wClickToMove.Cave, NULL, 0);

	// Jmp to our cave.
	RedirectCodeFlow((LPVOID)wClickToMove.Cave, (LPVOID)Offsets::CGPlayer_C__ClickToMove);

	log("Hooked CGPlayer_C__ClickToMove");
	LogFile("HookLanguageHack: Hooked CGPlayer_C__ClickToMove");
	return TRUE;
}

BOOL UnhookSubroutine(DWORD dwAddress, WHook hook)
{
	if (IsSubroutineHooked(dwAddress))
	{
		RestoreCodeFlow((LPVOID)dwAddress, hook.RestorePatch, hook.RestoreSize);
		VirtualFreeEx(WoW::handle, (LPVOID)hook.Cave, 1024, MEM_DECOMMIT);
		memset(&hook, 0, sizeof(hook));
		return true;
	}

	return false;
}

BOOL CleanPatch(DWORD hook)
{
	if (WoW::handle == NULL)
	{
		log("Invalid WoW handle");
		LogFile("CleanPatch: Invalid WoW handle");
	}

	switch (hook)
	{
		case Offsets::CGWorldFrame__OnFrameRender:
			if (UnhookSubroutine(Offsets::CGWorldFrame__OnFrameRender, wWorldFrame))
			{
				log("Clseaned WorldFrameRender hook");
				LogFile("Cleaned WorldFrameRender hook");
			}
			else
				return false;

			break;

		case Offsets::CGGameChat__AddChatMessage:
			if (UnhookSubroutine(Offsets::CGGameChat__AddChatMessage, wLanguage))
			{
				log("Cleaned AddChatMessage hook");
				LogFile("CleanPatch: Cleaned AddChatMessage hook");
			}

			break;

		case Offsets::CGPlayer_C__ClickToMove:
			if (UnhookSubroutine(Offsets::CGPlayer_C__ClickToMove, wClickToMove))
			{
				log("Cleaned ClickToMove hook");
				LogFile("CleanPatch: Cleaned ClickToMove hook");
			}
			else
			{
				velog("Unable to clean ClickToMove hook");
				return false;
			}

			break;

		case Offsets::CGlueMgr__DefaultServerLogin:
			if (UnhookSubroutine(Offsets::CGlueMgr__DefaultServerLogin, wDefaultServerLogin))
			{
				log("Cleaned Offsets::CGlueMgr__DefaultServerLogin hook");
				LogFile("CleanPatch: Cleaned Offsets::CGlueMgr__DefaultServerLogin hook");
			}

			break;

		case NULL:
			CleanPatch(Offsets::CGlueMgr__DefaultServerLogin);
			CleanPatch(Offsets::CGWorldFrame__OnFrameRender);
			CleanPatch(Offsets::CGGameChat__AddChatMessage);
			CleanPatch(Offsets::CGPlayer_C__ClickToMove);
			break;

		default:
			if (hook == Endscene.getAddress())
			{
				if (wEndScene.HookedFunction == NULL || !CEndscene::IsHooked()) return FALSE;

				DWORD oldE = Endscene.getAddress();

				// Write the original endscene location to the vtable function ptr.
				wpm(CEndscene::GetPtrAddress(), &wEndScene.HookedFunction, sizeof(wEndScene.HookedFunction));

				vlog("Cleaned Endscene (from 0x%x to 0x%x)", oldE, wEndScene.HookedFunction);
				vflog("CleanPatch: Cleaned EndScene hook: 0x%x", wEndScene.HookedFunction);

				// Old jmp hook restore.
				//UnhookSubroutine(g_dwEndScene, wEndScene);

				// Free up cave and function space.
				VirtualFreeEx(WoW::handle, (LPVOID)wEndScene.Cave, wEndScene.CaveSize, MEM_DECOMMIT);
				Endscene.update();

				memset(&wEndScene, 0, sizeof(wEndScene));
				return Endscene.CleanupHandler();
			}

			break;
	}

	return true;
}

BOOL CleanAllPatches()
{
	// Clean all hooks.
	if (WoW::handle == NULL || !WoW::isRunning()) return TRUE;
	CleanPatch(NULL);

	if (g_bPreAnimate)
		PreAnimatePatch(false);

	if (IsFogColorPatched())
		RestoreFogColorProtection();

	if (Hack::NameFilter::isPatched())
		Hack::NameFilter::Restore();

	if (WoW::InGame())
	{
		if (g_bFlyHack == true && (!LocalPlayer.isMounted() && LocalPlayer.flightSpeed() != 7))
		{
			g_bFlyHack = true;
			Hack::Movement::Fly(false, 0);
		}

		// Collision hack.
		if (Hack::Collision::isM2Enabled() || Hack::Collision::isWMOEnabled())
			Hack::Collision::All(false);

		// Collision player width.
		if (Hack::Collision::GetPlayerWidth() == 0)
		{
			if (Hack::Collision::playerWidth > 0)
				Hack::Collision::SetPlayerWidth(Hack::Collision::playerWidth);
			else
				Hack::Collision::SetPlayerWidth(0.31);
		}
	}

	return true;
}