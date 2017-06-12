#include "Dialog.h"
#include "Resource.h"
#include "ObjectList.h"

#include "Common\Common.h"
#include "Memory\Hooks.h"
#include "Memory\Hacks.h"
#include "Memory\Endscene.h"

#include "WoW API\Lua.h"
#include "WoW API\Chat.h"
#include "WoW API\Movement.h"
#include "WoW API\Environment.h"

#include "Bot\Fishing.h"
#include "Bot\Gathering.h"

#include "Main\Debug.h"

#include <Radar\Drawing.h>

#include "Geometry\Coordinate.h"
#include "ObjectWatch.h"

#include "ProfileSettings.h"

#include <Commctrl.h>

typedef VOID (__cdecl *GeneralAccept)(HWND);

HWND pListWindow = NULL;

GeneralAccept pfnGeneralAccept = NULL;

CHAR m_szGeneralEdit[5000];

CHAR g_lastAccount[256];
CHAR g_lastPass[256];

LRESULT CALLBACK LoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHAR szBuffer[256];
	CHAR list[][2][56] = {
#ifndef RELEASE
	{"Account", "Password"},
#else
	{ "", "" }
#endif
	};

	int select = 0;

	switch (message)
	{

	case WM_INITDIALOG:
		for (int x = 0; x < sizeof(list) / sizeof(CHAR[2][56]); x++)
			SendMessage(GetDlgItem(hWnd, IDC_PROCESS_LIST_WOW), LB_ADDSTRING, 0, (LPARAM)list[x][0]);

		SetWindowText(GetDlgItem(hWnd, IDC_PROCESS_BUTTON_CHOOSE), "Login");
		SetWindowText(GetDlgItem(hWnd, IDC_PROCESS_BUTTON_REFRESH), "");
		SetWindowText(hWnd, "Select Account");
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{

		case IDC_PROCESS_BUTTON_CHOOSE:
			if (SendDlgItemMessage(hWnd, IDC_PROCESS_LIST_WOW, LB_GETCURSEL, 0, 0) >= 0)
			{
				select = SendDlgItemMessage(hWnd, IDC_PROCESS_LIST_WOW, LB_GETCURSEL, 0, 0);
				if (!WoW::InGame())
				{
					sprintf_s(szBuffer, "Logging into account: %s with password: %s", list[select][0], list[select][1]);
					log(szBuffer);
					
					sprintf_s(g_lastAccount, list[select][0]);
					sprintf_s(g_lastPass, list[select][1]);
					g_relogger = 0;

					Login(list[select][0], list[select][1]);
					EndDialog(hWnd, NULL); // End process list dialog and start main dialog.
				}
				else
					log("Unable to login since we are already in game");
			}

			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd); 
		break;

	default:
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK ProcessListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHAR szBuffer[MAX_PATH];
	INT nBuffer = 0;

	switch (message)
	{

	case WM_INITDIALOG:
		pListWindow = hWnd;
		PopulateWoWList(GetDlgItem(hWnd, IDC_PROCESS_LIST_WOW)); // List all wow processes in listbox (Character name - PID)
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_PROCESS_BUTTON_REFRESH:

			nBuffer = SendDlgItemMessage(hWnd, IDC_PROCESS_LIST_WOW, LB_GETCURSEL, 0, 0); // Get current listbox position.
			SendDlgItemMessage(hWnd, IDC_PROCESS_LIST_WOW, LB_RESETCONTENT, 0, 0); // Reset listbox.

			PopulateWoWList(GetDlgItem(hWnd, IDC_PROCESS_LIST_WOW)); // List all wow processes in listbox (Character name - PID)
			SendDlgItemMessage(hWnd, IDC_PROCESS_LIST_WOW, LB_SETCURSEL, nBuffer, 0); // Change listbox cursor position to nBuffer.
			break;

		case IDC_PROCESS_BUTTON_CHOOSE:
			if (SendDlgItemMessage(hWnd, IDC_PROCESS_LIST_WOW, LB_GETCURSEL, 0, 0) >= 0)
			{
				memset(&szBuffer, 0, sizeof(szBuffer));
				SendDlgItemMessageA(hWnd, IDC_PROCESS_LIST_WOW, LB_GETTEXT, SendDlgItemMessage(hWnd, IDC_PROCESS_LIST_WOW, LB_GETCURSEL, 0, 0), (LPARAM)&szBuffer);
				
				nBuffer = atoi(strstr(szBuffer, " - ") + 3);
				WoW::handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nBuffer);

				if (WoW::handle != NULL)
				{
					// Set up fetch threads, start main dialog, and initialize global variables.
					InitializeBot(); 
					SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_HACKS_CHECK_LANGUAGEHACK, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_TAB_MAIN_CHECK_ANTIAFK, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_FISH_CHECK_LOGOUT, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_FISH_CHECK_HIDE, BM_SETCHECK, 0, 0);
					SetDlgItemText(g_hwMainWindow, IDC_MAIN_TAB_HACKS_EDIT_FOG_DENSITY, "1.00");

					SetWindowText(g_hwMainWindow, WoW::GetAccountName().c_str());
					
					sprintf_s(szBuffer, "%s %s loaded", Dinner::getDankName().c_str(), Dinner::versionName.c_str());
					AddChatMessage(szBuffer, SystemMessage); // Append message to in-game chat. (No more race condition due to ntSuspendProcess).

					pListWindow = NULL;
					EndDialog(hWnd, NULL); // End process list dialog and start main dialog.
				}
				else
					MessageBox(hWnd, "Dinnerror", "Could not find World of Warcraft!", MB_ICONERROR); 
			}

			break;
		}
		break;

	case WM_CLOSE:
		pListWindow = NULL;
		DestroyWindow(hWnd); 
		PostQuitMessage(0);
		break;

	default:
		return FALSE;
	}
	return FALSE;
}

DWORD GetDlgCheck(HWND hWnd, uint dlgEntry)
{
	return SendDlgItemMessage(hWnd, dlgEntry, BM_GETCHECK, 0, 0);
}

DWORD SetDlgCheck(HWND hWnd, uint dlgEntry, DWORD set)
{
	return SendDlgItemMessage(hWnd, dlgEntry, BM_SETCHECK, set, 0);
}

VOID InitTabControl(HWND hWnd)
{
	INT nTabCount = 0;
	TCITEM tci;

	HWND tabControl = GetDlgItem(hWnd, IDC_MAIN_TAB);

	// Initialize tabs in main dialog.
	tci.mask = TCIF_TEXT;
	tci.pszText = "Main";
	nTabCount = SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_GETITEMCOUNT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_INSERTITEM, nTabCount, (LPARAM) (LPTCITEM) &tci);

	tci.mask = TCIF_TEXT;
	tci.pszText = "Magic";
	nTabCount = SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_GETITEMCOUNT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_INSERTITEM, nTabCount, (LPARAM) (LPTCITEM) &tci);
	
	tci.pszText = "Gathering";
	nTabCount = SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_GETITEMCOUNT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_INSERTITEM, nTabCount, (LPARAM) (LPTCITEM) &tci);

	tci.pszText = "Fishing";
	nTabCount = SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_GETITEMCOUNT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_INSERTITEM, nTabCount, (LPARAM) (LPTCITEM) &tci);

	tci.pszText = "Tools";
	nTabCount = SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_GETITEMCOUNT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_INSERTITEM, nTabCount, (LPARAM) (LPTCITEM) &tci);
	TabCtrl_SetItemSize(tabControl, 62, 23);

	tci.pszText = "Debug";
	nTabCount = SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_GETITEMCOUNT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB, TCM_INSERTITEM, nTabCount, (LPARAM) (LPTCITEM) &tci);
}

VOID UpdateMainDisplay(HWND hWnd)
{
	CHAR szBuffer[MAX_PATH];
	CHAR buffer[MAX_PATH];

	static WGUID		CurrentTarget;
	static UINT			health				= 0;
	static UINT			maxHealth			= 0;
	static UINT			power				= 0;
	static UINT			maxPower			= 0;
	static DWORD		unitFlags			= NULL;
	static ULONGLONG	gatherTime			= NULL;
	Thread *botThread = NULL;

	LocalPlayer.update(LocationInfo | UnitFieldInfo);
	if (!LocalPlayer.isValid()) return;

	// Update local player's current health
	if (LocalPlayer.health() != health || LocalPlayer.maxHealth() != maxHealth)
	{
		health = LocalPlayer.health();
		maxHealth = LocalPlayer.maxHealth();
		sprintf_s(szBuffer, "HP: %d/%d", health, maxHealth);
		SetDlgItemText(hWnd, IDC_MAIN_TAB_STATIC_HP, szBuffer);
	}

	// Update local player's current power
	if (LocalPlayer.power() != power || LocalPlayer.maxPower() != maxPower)
	{
		power = LocalPlayer.power();
		maxPower = LocalPlayer.maxPower();
		sprintf_s(szBuffer, "%s: %d/%d", LocalPlayer.getPowerString().c_str(), power, maxPower);
		SetDlgItemText(hWnd, IDC_MAIN_TAB_STATIC_MANA, szBuffer);
	}

	// Update local player's combat status
	if (LocalPlayer.unitFlags() != unitFlags)
	{
		unitFlags = LocalPlayer.unitFlags();
		sprintf_s(szBuffer, "In Combat: %s", ((LocalPlayer.inCombat()) ?"Yes":"No"));
		SetDlgItemText(hWnd, IDC_MAIN_TAB_STATIC_INCOMBAT, szBuffer);
	}

	// If any bot is active, display its current runtime.
	if ((botThread = Thread::FindType(Thread::eType::Bot)))
	{ 
		if (botThread && botThread->running())
		{
			switch (botThread->task())
			{
				//vlog("run: %d", DinnerFish::dinnerFish->getRunTime());
				// If the number of seconds elapsed differ, then update the displayed time
				// Fishing bot is active.
				case Thread::eTask::Fishing:
					if (gatherTime / 1000 != DinnerFish::GetRunTime() / 1000)
					{
						gatherTime = DinnerFish::GetRunTime();
						GetTimerFormat(gatherTime, buffer, MAX_PATH);

						// Update display.
						sprintf_s(szBuffer, "Bot Time: %s", buffer);
						SetDlgItemTextA(hWnd, IDC_MAIN_TAB_STATIC_XP, szBuffer);
					}

					break;

				// Gathering bot is active.
				case Thread::eTask::Gathering:
					if (gatherTime / 1000 != GetTickCount64() - GetGatherTime(-1) / 1000)
					{
						gatherTime = GetTickCount64() - GetGatherTime(-1);
						GetTimerFormat(gatherTime, buffer, MAX_PATH);

						// Update display.
						sprintf_s(szBuffer, "Bot Time: %s", buffer);
						SetDlgItemTextA(hWnd, IDC_MAIN_TAB_STATIC_XP, szBuffer);
					}
					break;
			}

		}
	}
	
	// Update local player's current target, provided it is different
	// from our saved target guid.
	if (CurrentTarget != GetCurrentTargetGUID())
	{   
		// Cache current target GUID.
		CurrentTarget = GetCurrentTargetGUID();

		// Update display.
		sprintf(szBuffer, "Current Target: %s", GetObjectByGUIDEx(CurrentTarget, NameInfo | BaseObjectInfo).Name);
		SetDlgItemText(hWnd, IDC_MAIN_TAB_STATIC_CURRENTTARGET, szBuffer);
	}
}

VOID UncheckMagic()
{
	SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_TAB_HACKS_CHECK_CTT, BM_SETCHECK, 0, 0);
	SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_TAB_HACKS_CHECK_VFLY, BM_SETCHECK, 0, 0);
	SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_HACKS_CHECK_ANIMATEPATCH, BM_SETCHECK, 0, 0);
	SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_HACKS_CHECK_NAMEFILTER, BM_SETCHECK, 0, 0);
	SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_HACKS_CHECK_LANGUAGEHACK, BM_SETCHECK, 0, 0);
	g_bPreAnimate = false;
	g_bFlyHack = false;
}

VOID ChangeCurrentTabWindowVisibility(HWND hWnd, INT nFlag, INT nCmdShow)
{
	switch(nFlag)
	{
	case 0: // Main
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_STATIC_CURRENTTARGET), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_BUTTON_CHANGEPROCESS), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_MAIN_CHECK_TOPMOST), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_MAIN_CHECK_ANTIAFK), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_STATIC_INCOMBAT), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_GROUPBOX_PLAYER), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_BUTTON_START), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_STATIC_MANA), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_STATIC_HP), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_STATIC_XP), nCmdShow);
		break;

	case 1: // Magic  IDC_MAIN_HACKS_CHECK_ANIMATEPATCH
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_HACKS_CHECK_CTT), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_HACKS_CHECK_VFLY), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_ANIMATEPATCH), nCmdShow);
		
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_EDIT_FLY), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_M2), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_FLY), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_WMO), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_STATIC_FLY), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_WATER), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_TERRAIN), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_HORIZON), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_WIREFRAME), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_NAMEFILTER), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_MAPTELEPORT), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_LANGUAGEHACK), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_TRACKHUMANOIDS), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_HACKS_GROUPBOX_RENDERING), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_HACKS_BUTTON_FOG_DENSITY), nCmdShow);
		break;
		
	case 2: // Gathering 
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_GATHERING_EDIT_TIME), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_GATHERING_COMBO_MODE), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_GATHERING_BUTTON_START), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_GATHERING_STATIC_RUNTIME), nCmdShow);
		break;

	case 3: // Fishing
		ShowWindow(GetDlgItem(hWnd, IDC_FISH_TAB_EDIT_TIME), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_FISH_COMBO_PROFILES), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_FISH_BUTTON_PROFILE), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_FISH_TAB_BUTTON_FISH), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_FISH_CHECK_HIDE), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_FISH_CHECK_LOGOUT), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_FISH_CHECK_RENDER), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_FISH_TAB_STATIC_MINUTES), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_FISH_TAB_GROUPBOX_LIMITS), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_FISH_TAB_STATIC_STOPAFTER), nCmdShow); 
		break;

	case 4: // Tools 
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_RADAR), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_TURN_TO), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_RUN_LUA), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_TELEPORT), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_HIDE_WOW), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_HACK_WATCH), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_TRACKHIDDEN), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_OBJECT_WATCH), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_FOLLOW_TARGET), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_BUTTON_INTERACT_TARGET), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TOOLS_CHECK_INGAMEPLAYERCOUNT), nCmdShow);
		break;

	case 5: // Debug 
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_AURAS), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_CLEANHOOKS), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_RESUME_WOW), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_SUSPEND_WOW), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_PLAYER_DUMP), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_ADDRESS_DUMP), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_DUMPLOCATION), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_DUMPUNITFIELD), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_BUTTON_DUMPBASEOBJECT), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDD_MAIN_TAB_DEBUG_BUTTON_REFRESH_PLAYER), nCmdShow);
		ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_DEBUG_GROUPBOX_CURRENTTARGET), nCmdShow);
		break;
	}
}

VOID HandleTabChange(HWND hWnd)
{
	switch(TabCtrl_GetCurSel(GetDlgItem(hWnd, IDC_MAIN_TAB)))
	{
		case 0:	// Main tab
			ChangeCurrentTabWindowVisibility(hWnd, 0, SW_SHOW);
			ChangeCurrentTabWindowVisibility(hWnd, 1, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 2, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 3, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 4, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 5, SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_EDIT_LOG), SW_SHOW);
			break;

		case 1:	// Hacks tab
			ChangeCurrentTabWindowVisibility(hWnd, 0, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 1, SW_SHOW);
			ChangeCurrentTabWindowVisibility(hWnd, 2, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 3, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 4, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 5, SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_EDIT_LOG), SW_HIDE);
			break;

		case 2:	// Fishing tab
			ChangeCurrentTabWindowVisibility(hWnd, 0, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 1, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 2, SW_SHOW);
			ChangeCurrentTabWindowVisibility(hWnd, 3, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 4, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 5, SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_EDIT_LOG), SW_SHOW);
			break;

		case 3:	// Fishing tab
			ChangeCurrentTabWindowVisibility(hWnd, 0, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 1, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 2, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 3, SW_SHOW);
			ChangeCurrentTabWindowVisibility(hWnd, 4, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 5, SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_EDIT_LOG), SW_SHOW);
			break;
			
		case 4:	// Tools tab
			ChangeCurrentTabWindowVisibility(hWnd, 0, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 1, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 2, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 3, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 4, SW_SHOW);
			ChangeCurrentTabWindowVisibility(hWnd, 5, SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_EDIT_LOG), SW_SHOW);
			break;

		case 5:	// Debug tab
			ChangeCurrentTabWindowVisibility(hWnd, 0, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 1, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 2, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 3, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 4, SW_HIDE);
			ChangeCurrentTabWindowVisibility(hWnd, 5, SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_MAIN_TAB_EDIT_LOG), SW_SHOW);
			break;

		default:
			break;
	}
}

VOID InitializeOtherControls(HWND hWnd)
{
	g_hwMainWindow = hWnd;
	HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, "Arial");

	EnableWindow(GetDlgItem(hWnd, IDC_MAIN_HACKS_CHECK_LANGUAGEHACK), false);  // Disable the language hack check until further notice.
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB_EDIT_LOG, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB_EDIT_LOG, EM_LIMITTEXT, 200000, TRUE);
	SetDlgItemText(hWnd, IDC_MAIN_TAB_GROUPBOX_PLAYER, WoW::GetLocalPlayerName().c_str());
	SetDlgItemText(hWnd, IDC_FISH_TAB_EDIT_TIME, "20");
	SetDlgItemText(hWnd, IDC_MAIN_HACKS_EDIT_FLY, "7.0");
	SetDlgItemText(hWnd, IDC_MAIN_HACKS_STATIC_FLY, "100%");
	SetDlgItemText(hWnd, IDC_MAIN_GATHERING_EDIT_TIME, "20");

	// Gathering modes
	SendDlgItemMessage(hWnd, IDC_MAIN_GATHERING_COMBO_MODE, CB_ADDSTRING,(WPARAM)0, (LPARAM)"Herbalism"); 
    SendDlgItemMessageA(hWnd, IDC_MAIN_GATHERING_COMBO_MODE, CB_ADDSTRING, (WPARAM)0, (LPARAM)"Mining"); 
	SendDlgItemMessageA(hWnd, IDC_MAIN_GATHERING_COMBO_MODE, CB_SETCURSEL, 1, 1);

	DinnerFish::PopulateProfiles(hWnd);

	SendDlgItemMessage(hWnd, IDC_MAIN_HACKS_CHECK_LANGUAGEHACK, BM_SETCHECK, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_TAB_MAIN_CHECK_ANTIAFK, BM_SETCHECK, 1, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_FISH_CHECK_LOGOUT, BM_SETCHECK, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_FISH_CHECK_HIDE, BM_SETCHECK, 0, 0);
	SendDlgItemMessage(hWnd, IDC_MAIN_HACKS_CHECK_FLY, BM_SETCHECK, 0, 0);
	SetDlgItemText(hWnd, IDC_MAIN_TAB_HACKS_EDIT_FOG_DENSITY, "1.00");
	SetTimer(hWnd, IDT_TIMER, 500, MainTimerProc);
	pfnGeneralAccept = FogDensityChange;
}

BOOL SetMagicCheck(int dlgItem, bool state)
{
	return SendDlgItemMessage(g_hwMainWindow, dlgItem, BM_SETCHECK, state, 0);
}

BOOL GetMagicCheck(INT dlgItem)
{
	return SendDlgItemMessage(g_hwMainWindow, dlgItem, BM_GETCHECK, 0, 0);
}

VOID HandleCTTCheck(DWORD dwCheck, WPARAM wParam)
{
	CHAR szBuffer[64];
	UINT playerCount = 0;

	if (!WoW::InGame()) return;

    if(HIWORD(wParam) == BN_CLICKED)
	{
		if(dwCheck == BST_CHECKED) 
		{
			if (!IsSubroutineHooked(Offsets::CGPlayer_C__ClickToMove))
			{
				ToggleCTM(TRUE);
				LogAppend("ClickToTeleport enabled");

				playerCount = PlayersInArea();
				if (playerCount > 0)
				{
					sprintf_s(szBuffer, "Be careful, there %s %d player%s in the area", (playerCount>1)?"are":"is", playerCount, (playerCount>1)?"s":"");
					AddChatMessage(szBuffer, "DMagic", RedWarningMessage);
				}

				Thread::Create(StartClickToTeleport, (LPVOID)LocalPlayer.base(), Thread::eType::Memory, Thread::ePriority::High);
			}
		}
		else
		{
			CleanPatch(Offsets::CGPlayer_C__ClickToMove);
			ToggleCTM(FALSE);
			GetWoWWindow();

			LogAppend("Disabled ClickToTeleport");
		}
	}
}

VOID HandleVFlyCheck(DWORD dwCheck, WPARAM wParam)
{
	UINT playerCount = 0;

	if (!WoW::InGame()) return;
    if(HIWORD(wParam) == BN_CLICKED)
	{
		if(dwCheck == BST_CHECKED) 
		{
	/*		playerCount = PlayersInArea();
			if (playerCount > 0)
			{
				sprintf_s(szBuffer, "Be careful, there %s %d player%s in the area.", (playerCount>1)?"are":"is", playerCount, (playerCount>1)?"s":"");
				AddChatMessage(szBuffer, "DMagic", RedWarningMessage);
			}*/

			g_VFly = TRUE;
			LogAppend("F-key teleports enabled");
		}
		else
		{
			g_VFly = FALSE;
			LogAppend("F-key teleports disabled");
		}
	}
}

VOID HandleTopmostCheck(HWND hWnd, DWORD dwCheck, WPARAM wParam)
{
	if(HIWORD(wParam) == BN_CLICKED)
	{
		if(dwCheck == BST_CHECKED) 
		{
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			LogAppend("Set main window to be topmost");
		} 
		else if(dwCheck == BST_UNCHECKED) 
		{
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			LogAppend("Restored main window z-order");
		}
	}
}

LRESULT CALLBACK TeleportProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHAR szBuffer[256];

	switch (message)
	{
	case WM_INITDIALOG:
		sprintf(szBuffer, "%0.3f", g_TeleportPos.X);
		SetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_X, szBuffer);

		sprintf(szBuffer, "%0.3f", g_TeleportPos.Y);
		SetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_Y, szBuffer);

		sprintf(szBuffer, "%0.3f", g_TeleportPos.Z);
		SetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_Z, szBuffer);
		break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_TELEPORT_BUTTON_TELEPORT:
					GetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_X, szBuffer, sizeof(CHAR)*256);
					g_TeleportPos.X = (FLOAT)atof(szBuffer);

					GetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_Y, szBuffer, sizeof(CHAR)*256);
					g_TeleportPos.Y = (FLOAT)atof(szBuffer);

					GetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_Z, szBuffer, sizeof(CHAR)*256);
					g_TeleportPos.Z = (FLOAT)atof(szBuffer);

					if (ValidCoord(g_TeleportPos))
					{
						Hack::Movement::Teleport(g_TeleportPos);
						vlog("Teleported to {%0.2f, %0.2f, %0.2f}", g_TeleportPos.X, g_TeleportPos.Y, g_TeleportPos.Z);
					}

					LogAppend(szBuffer);
				break;

				case IDC_TELEPORT_BUTTON_CTM:
					GetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_X, szBuffer, sizeof(CHAR)*256);
					g_TeleportPos.X = (FLOAT)atof(szBuffer);

					GetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_Y, szBuffer, sizeof(CHAR)*256);
					g_TeleportPos.Y = (FLOAT)atof(szBuffer);

					GetDlgItemTextA(hWnd, IDC_TELEPORT_EDIT_Z, szBuffer, sizeof(CHAR)*256);
					g_TeleportPos.Z = (FLOAT)atof(szBuffer);

					if (ValidCoord(g_TeleportPos))
					{
						ClickToMove(g_TeleportPos);
						vlog("Move to {%0.2f, %0.2f, %0.2f}", g_TeleportPos.X, g_TeleportPos.Y, g_TeleportPos.Z);
					}

					LogAppend(szBuffer);
				break;
			}

			break;
		
		case WM_CLOSE:
			EndDialog(hWnd, LOWORD(wParam));
			break;

		default:
			return FALSE;
	}

	return FALSE;
}

LRESULT CALLBACK LuaScriptProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_LUA_EDIT, m_szGeneralEdit);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_LUA_RUN:
			Lua::RunScript(hWnd);
			GetDlgItemText(hWnd, IDC_LUA_EDIT, m_szGeneralEdit, 5000);
			//EndDialog(hWnd, LOWORD(wParam));
			break;

		case IDC_LUA_CANCEL:
			GetDlgItemText(hWnd, IDC_LUA_EDIT, m_szGeneralEdit, 5000);
			EndDialog(hWnd, LOWORD(wParam));
			break;
		}
		break;

	case WM_CLOSE:
		GetDlgItemText(hWnd, IDC_LUA_EDIT, m_szGeneralEdit, 5000);
		EndDialog(hWnd, LOWORD(wParam));
		break;

	default:
		return FALSE;
	}

	return FALSE;
}

LRESULT CALLBACK GeneralInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_GENERAL_EDIT_INPUT, m_szGeneralEdit);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_GENERAL_BUTTON_ACCEPT:
			pfnGeneralAccept(hWnd);
			EndDialog(hWnd, LOWORD(wParam));
			break;

		case IDC_GENERAL_BUTTON_Cancel:
			EndDialog(hWnd, LOWORD(wParam));
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, LOWORD(wParam));
		break;

	default:
		return FALSE;
	}

	return FALSE;
}

VOID MagicChecks(HWND hWnd)
{
	CHAR szBuffer[256];
	DWORD dwCheckPos;

    dwCheckPos = SendDlgItemMessage(hWnd, IDC_MAIN_HACKS_CHECK_FLY, BM_GETCHECK, 0, 0);
	if (dwCheckPos == BST_CHECKED)
	{
		GetDlgItemTextA(hWnd, IDC_MAIN_HACKS_EDIT_FLY, szBuffer, 256);
		if (isfloat(szBuffer))
		{
			g_fFlyHack = (FLOAT)atof(szBuffer);
			Hack::Movement::Fly(true, g_fFlyHack);
			g_bFlyHack = true;
		}
	}
	else if (dwCheckPos == BST_UNCHECKED)
	{
		Hack::Movement::Fly(false, 0);
		g_bFlyHack = false;
	}
}

VOID DFishChecks(HWND hWnd)
{
	DWORD dwCheckPos;
    dwCheckPos = SendDlgItemMessage(hWnd, IDC_MAIN_FISH_CHECK_HIDE, BM_GETCHECK, 0, 0);
	if(dwCheckPos == BST_CHECKED) 
	{
		g_DFish_Hide = TRUE;
	} 
	else if(dwCheckPos == BST_UNCHECKED) 
	{
		g_DFish_Hide = FALSE;
	}

    dwCheckPos = SendDlgItemMessage(hWnd, IDC_MAIN_FISH_CHECK_LOGOUT, BM_GETCHECK, 0, 0);
	if(dwCheckPos == BST_CHECKED) 
	{
		g_DFish_Logout = TRUE;
	} 
	else if(dwCheckPos == BST_UNCHECKED) 
	{
		g_DFish_Logout = FALSE;
	}
}

// On click Dialog events.
VOID onDensityChangeButton(HWND hWnd)
{
	HWND hwWindow; 

	pfnGeneralAccept = FogDensityChange;
	sprintf_s(m_szGeneralEdit, "%f", GetFogDensity());
	hwWindow = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GENERAL_INPUT), hWnd, (DLGPROC)GeneralInputProc);
	SetWindowText(hwWindow, "Set Fog Density");
	ShowWindow(hwWindow, SW_SHOW);
}


VOID onDebugPlayerDumpButton()
{
	if (WoW::InGame())
	{
		LocalPlayer.update(BaseObjectInfo | UnitFieldInfo | LocationInfo | NameInfo);
		Debug::DumpUnitField(LocalPlayer.unitField());
		Debug::DumpBaseObject(LocalPlayer.Obj);
		Debug::DumpLocation(LocalPlayer.Obj);
	}
}

VOID onDumpUnitFieldButton()
{
	CUnit target = LocalPlayer.getTarget();
	target.update(UnitFieldInfo);

	if (target.isValid())
		Debug::DumpUnitField(target.unitField());
}
			
VOID onDumpBaseObjectButton()
{
	CUnit target = LocalPlayer.getTarget();
	target.update(NameInfo);

	if (target.isValid())
		Debug::DumpBaseObject(target.object());
}

VOID onDumpLocationButton()
{
	CUnit target = LocalPlayer.getTarget();
	target.update(LocationInfo);

	if (target.isValid())
		Debug::DumpLocation(target.object());
}

VOID onDumpAurasButton()
{
	CUnit target = LocalPlayer.getTarget();

	if (target.isValid())
		Debug::DumpAuras(target.object());
}

VOID onRefreshPlayerButton()
{
	//memset(&LocalPlayer, 0, sizeof(LocalPlayer));
	LocalPlayer.clear();
	GetCurrentManager();

	LocalPlayer.setBase(WoW::GetPlayerBase());
	LocalPlayer.update(LocationInfo | UnitFieldInfo);
}

VOID onTeleportButton(HWND hWnd)
{
	if (WoW::InGame())
		ShowWindow(CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TELEPORT), hWnd, (DLGPROC)TeleportProc), SW_SHOW);
}

VOID onObjectListButton(HWND hWnd)
{
	if (WoW::InGame())
		ShowWindow(CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN_LIST), hWnd, (DLGPROC)ObjectListProc), SW_SHOW);
}

VOID onObjectWatchButton(HWND hWnd)
{
	if (WoW::InGame())
		ShowWindow(CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OBJECTWATCH), hWnd, (DLGPROC)ObjectWatchProc), SW_SHOW);
}

VOID onHackerWatchButton()
{
	if (WoW::InGame() && GetCurrentTargetGUID().low != NULL) 
	{
		LogAppend("Monitoring target for speed hacks...");
		//DCreateThread(HackerWatch, NULL, NULL, NULL);
	}
}

VOID onFollowTargetButton()
{
	Thread *follow = Thread::FindType(Thread::eType::Navigation);

	if (WoW::InGame() && !follow)
	{
		CObject &target = LocalPlayer.getTarget();
		target.update(NameInfo);

		vlog("Following %s", target.name());
		Thread::Create(FollowObject, NULL, Thread::eType::Navigation);
	}
	else
		LogAppend("Unable to follow unit");
}

VOID onChangeProcessButton(HWND hWnd)
{
	CHAR szBuffer[256];

	// Only one instance is allowed.
	if (pListWindow) 
	{
		SetForegroundWindow(pListWindow);
		return;
	}

	// Stop any wow info loop thread.
	Thread *t = Thread::FindType(Thread::eType::InfoLoop);
	if (t) t->stop();

	DinnerChat::Destroy();

	CleanPatch(Memory::Endscene.getAddress());
	CleanAllPatches(); // This is placed here so it still has a valid handle.

	LocalPlayer.clear();
	//memset(&LocalPlayer, 0, sizeof(LocalPlayer));
			
	sprintf_s(szBuffer, "%s unloaded", Dinner::getDankName().c_str());
	AddChatMessage(szBuffer, SystemMessage); // If we were previously attached to a different session, unload and notify.
			
	// Clear WoW's module cache.
	Memory::ClearCache();

	WoW::handle = NULL; // Handle is now null.
	g_hwWoW = NULL;

	g_relogger = -1;
	log("Detached");

	// Pont mode
	if (g_updateWoWChat && g_rosterMode && g_chatLog)
		LogChat("Pont is offline.");

	if (g_chatLog) LogStatus(false);
	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_PROCESS), NULL, (DLGPROC)ProcessListProc);

	GetLocalPlayerName(szBuffer);
	SetDlgItemText(hWnd, IDC_MAIN_TAB_GROUPBOX_PLAYER, szBuffer);
}

VOID onNameFilterCheck(HWND hWnd, WPARAM wParam)
{
	DWORD checked;

	if(HIWORD(wParam) == BN_CLICKED)
	{
		checked = SendDlgItemMessage(hWnd, IDC_MAIN_HACKS_CHECK_NAMEFILTER, BM_GETCHECK, 0, 0);
        if(checked == BST_CHECKED) 
		{
			if (Hack::NameFilter::Patch())
			{
				vlog("Disabled name filter");
			}
			else
				vlog("Hack::NameFilter::Patch error");
		}
		else if(checked == BST_UNCHECKED) 
		{
			if (Hack::NameFilter::Restore())
			{
				vlog("Restored name filter");
			}
			else
				vlog("Hack::NameFilter::Patch error");
		}
	}
}

VOID onTopmostCheck(HWND hWnd, WPARAM wParam)
{
	HandleTopmostCheck(hWnd, SendDlgItemMessage(hWnd, IDC_MAIN_TAB_MAIN_CHECK_TOPMOST, BM_GETCHECK, 0, 0), wParam);
}

VOID onAntiAFKCheck(HWND hWnd, WPARAM wParam)
{
	DWORD dwCheckPos;

	if(HIWORD(wParam) == BN_CLICKED)
    {
		dwCheckPos = SendDlgItemMessage(hWnd, IDC_MAIN_TAB_MAIN_CHECK_ANTIAFK, BM_GETCHECK, 0, 0);
		if(dwCheckPos == BST_CHECKED) 
		{
			g_AntiAFK = TRUE;
			log("Anti-AFK started");
		} 
		else if(dwCheckPos == BST_UNCHECKED) 
		{
			g_AntiAFK = FALSE;
			log("Anti-AFK stopped");
		}
	}
}

VOID onRenderWireframeCheck()
{
	AddChatMessage("Wireframe rendering toggled", "DMagic", LightPurpleMessage);
	log("Wireframe rendering toggled");
	Hack::Render::Wireframe(FALSE);
}

VOID onRenderHorizonCheck()
{
	AddChatMessage("Horizon rendering toggled", "DMagic", LightPurpleMessage);
	LogAppend("Horizon mountains rendering toggled");
	Hack::Render::HorizonMountains(FALSE);
}

VOID onRenderTerrainCheck()
{
	AddChatMessage("Terrain rendering toggled", "DMagic", LightPurpleMessage);
	LogAppend("Terrain rendering toggled");
	Hack::Render::Terrain(FALSE);
}

VOID onRenderWaterCheck()
{
	AddChatMessage("Water rendering toggled", "DMagic", LightPurpleMessage);
	LogAppend("Water rendering toggled");
	Hack::Render::Water(FALSE);
}

VOID onRenderWMOCheck()
{
	AddChatMessage("WMO rendering toggled", "DMagic", LightPurpleMessage);
	LogAppend("WMO rendering toggled");
	Hack::Render::WMO(FALSE);
}
 
VOID onRenderM2Check()
{
	AddChatMessage("M2 rendering toggled", "DMagic", LightPurpleMessage);
	LogAppend("M2 rendering toggled");
	Hack::Render::M2(FALSE);
}

VOID OffsetWindowSize(HWND window, int cx, int cy)
{
	RECT size;

	int width, height;

	GetWindowRect(window, &size);
	
	width = size.right - size.left;
	height = size.bottom - size.top;
	SetWindowPos(window, HWND_TOP, 0, 0, width + cx, height + cy, SWP_NOMOVE);
}

VOID OffsetWindowPos(HWND window, int cx, int cy)
{
	RECT pos;

	GetWindowRect(window, &pos);
	SetWindowPos(window, HWND_TOP, pos.left + cx, pos.top + cy, 0, 0, SWP_NOSIZE);
}

int GetWindowWidth(HWND window)
{
	RECT rect;

	GetWindowRect(window, &rect);
	return rect.right - rect.left;
}

WNDPROC pEditProc;

LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN) 
	{
        if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A') 
		{
            SendMessage(hwnd, EM_SETSEL, 0, -1);
        }
    }

    return CallWindowProc(pEditProc, hwnd, msg, wParam, lParam);
}

VOID onRunLuaButton(HWND parent)
{
	HWND runLua, edit;

	runLua = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LUA), parent, (DLGPROC)LuaScriptProc);
	OffsetWindowPos(runLua, GetWindowWidth(parent) + 10, 0);

	edit = GetDlgItem(runLua, IDC_LUA_EDIT);
	pEditProc = (WNDPROC)SetWindowLong(edit, GWL_WNDPROC, (LONG)&EditProc);
	ShowWindow(runLua, SW_SHOW);
}

VOID onCleanHooks()
{
	CleanPatch(Memory::Endscene.getAddress());
	CleanAllPatches();
	UncheckMagic();
}

VOID onLanguageHackCheck()
{
	DWORD dwCheckPos;

	dwCheckPos = GetMagicCheck(IDC_MAIN_HACKS_CHECK_LANGUAGEHACK);
	if (dwCheckPos == BST_CHECKED) 
	{
		if (!IsSubroutineHooked(Offsets::CGGameChat__AddChatMessage))
		{
			if (LanguageHack())
			{
				log("Language translation enabled");
				AddChatMessage("Language translation enabled", "DMagic", LightPurpleMessage);
			}
		}
	} 
	else if(dwCheckPos == BST_UNCHECKED) 
	{
		CleanPatch(Offsets::CGGameChat__AddChatMessage);
		LogAppend("Language translation disabled");	
		AddChatMessage("Language translation disabled", "DMagic", LightPurpleMessage);
	}
}

VOID onHideWoW()
{
	if (IsWindowVisible(g_hwWoW))
		ShowWindow(g_hwWoW, SW_HIDE);
	else
		ShowWindow(g_hwWoW, SW_SHOW);
}

VOID onInteractTarget()
{
	Interact(GetCurrentTarget(NULL).BaseAddress);
}

VOID onRadarButton()
{
	if (WoW::InGame())
		Thread::Create(Radar::Create, NULL, Thread::eType::GUI);
}

VOID onVFlyCheck(HWND hWnd, WPARAM wParam)
{
	HandleVFlyCheck(SendDlgItemMessage(hWnd, IDC_MAIN_TAB_HACKS_CHECK_VFLY, BM_GETCHECK, 0, 0), wParam);
}

VOID onCTTCheck(HWND hWnd, WPARAM wParam)
{
	HandleCTTCheck(SendDlgItemMessage(hWnd, IDC_MAIN_TAB_HACKS_CHECK_CTT, BM_GETCHECK, 0, 0), wParam);
}

VOID onMapTeleportCheck()
{
	DinnerChat::FlagOff(TOGGLE_NEWMAP);
	if (GetMagicCheck(IDC_MAIN_HACKS_CHECK_MAPTELEPORT))
	{
		g_mapTeleport = 2;
		Lua::DoString(
			"if Nx.Map ~= nil then "
				"hooksecurefunc(Nx.Map, \"SeT3\", "
					"function(self,typ,x1,y1,x2,y2,tex2,id,nam,kee,maI) "
						"dMapX, dMapY = self:GZP(self.MaI, x1, y1) "
						"dMapZ = self.MWI[self.MaI].Zon "
						"DinnerDank.FlagOff(1) "
						"DinnerDank.FlagOff(2) "
						
						// Set new map flag.
						"DinnerDank.FlagOn(2) "
						"SetConsoleKey(g_dconsolekey) "
					"end"
				") "
			"end");

		log("Map teleporting enabled");
	}
	else
	{
		g_mapTeleport = 0;
		log("Map teleporting disabled");
	}
}

VOID onFlyEdit(HWND hWnd)
{
	CHAR szBuffer[256];
	float speed = 7;

	GetDlgItemTextA(hWnd, IDC_MAIN_HACKS_EDIT_FLY, szBuffer, 64);
	if (isfloat(szBuffer))
	{
		speed = (FLOAT)atof(szBuffer);
		sprintf_s(szBuffer, "%d%%", (INT)CalculateSpeedPercent(speed));
		SetDlgItemTextA(hWnd, IDC_MAIN_HACKS_STATIC_FLY, szBuffer);
		Hack::Movement::Fly(g_bFlyHack, speed);
	}
}

VOID onTrackHumanoidsCheck(HWND hWnd)
{
	DWORD check;

	check = SendDlgItemMessage(hWnd, IDC_MAIN_HACKS_CHECK_TRACKHUMANOIDS, BM_GETCHECK, 0, 0);

	if (check) log("Tracking humanoids enabled");
	else	   log("Tracking humanoids disabled");

	Hack::Tracking::Humanoids(check);
}

VOID onFishRenderCheck()
{
	log("Toggled rendering");

	Hack::Render::M2(FALSE);
	Hack::Render::WMO(FALSE);
	Hack::Render::Water(FALSE);
	Hack::Render::Terrain(FALSE);
	Hack::Render::HorizonMountains(FALSE);
}

VOID onIngamePlayerCountCheck(HWND hWnd)
{
	DWORD check;

	CHAR szBuffer[256];

	check = SendDlgItemMessage(hWnd, IDC_MAIN_TOOLS_CHECK_INGAMEPLAYERCOUNT, BM_GETCHECK, 0, 0);
	if (!check)
	{
		sprintf_s(szBuffer, "%s:Hide()", DinnerChat::frameName.c_str());
		Lua::DoString(szBuffer);
	}
	else 
	{
		sprintf_s(szBuffer, "%s:Show()", DinnerChat::frameName.c_str());
		Lua::DoString(szBuffer);
	}
}

VOID onAnimatePatchCheck(HWND hWnd)
{
	DWORD check;

	check = SendDlgItemMessage(hWnd, IDC_MAIN_HACKS_CHECK_ANIMATEPATCH, BM_GETCHECK, 0, 0);
	if(check == BST_CHECKED) 
	{
		g_bPreAnimate = true;
		PreAnimatePatch(true);
		LogAppend("PreAnimate patch enabled");
		AddChatMessage("PreAnimate patch enabled", "DMagic", LightPurpleMessage);
	} 
	else if(check == BST_UNCHECKED)
	{
		g_bPreAnimate = false;
		PreAnimatePatch(false);
		LogAppend("PreAnimate patch disabled");	
		AddChatMessage("PreAnimate patch disabled", "DMagic", LightPurpleMessage);
	}
}

VOID onFishStartButton(HWND hWnd)
{
	CHAR szBuffer[256];

	if (WoW::InGame())
	{
		// Get Dinnerfish options.
		DFishChecks(hWnd);

		// Get Dinnerfish run time.
		GetDlgItemTextA(hWnd, IDC_FISH_TAB_EDIT_TIME, szBuffer, 64);

		if (!DinnerFish::Start(atoi(szBuffer)))
			SetDlgItemText(hWnd, IDC_FISH_TAB_BUTTON_START, "Start Fishing");
		else
			SetDlgItemText(hWnd, IDC_FISH_TAB_BUTTON_START, "Stop Fishing");
	}
}

std::string getFishingProfileFromCombo()
{
	char buffer[256];

	int check = SendDlgItemMessageA(g_hwMainWindow, IDC_FISH_COMBO_PROFILES, CB_GETCURSEL, 0, 0);
	SendDlgItemMessageA(g_hwMainWindow, IDC_FISH_COMBO_PROFILES, CB_GETLBTEXT, (WPARAM)check, (LPARAM)buffer);
	return std::string(buffer);
}

VOID onFishingProfileCombo(HWND hWnd, WPARAM wParam)
{
	DWORD check;
	CHAR  text[256];

	if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		check = SendDlgItemMessageA(hWnd, IDC_FISH_COMBO_PROFILES, CB_GETCURSEL, 0, 0);
		SendDlgItemMessageA(hWnd, IDC_FISH_COMBO_PROFILES, CB_GETLBTEXT, (WPARAM)check, (LPARAM)text);

		vlog("Changed fishing profile to: %s", text);
	}
}

VOID onGatheringModeCombo(HWND hWnd, WPARAM wParam)
{
	DWORD check;

	CHAR szBuffer[256];
	CHAR text[256];

	if (HIWORD(wParam) == CBN_SELCHANGE )
	{
		check = SendDlgItemMessageA(hWnd, IDC_MAIN_GATHERING_COMBO_MODE, CB_GETCURSEL, 0, 0);
		SendDlgItemMessageA(hWnd, IDC_MAIN_GATHERING_COMBO_MODE, CB_GETLBTEXT, (WPARAM)check, (LPARAM)text);

		sprintf_s(szBuffer, "Changed gathering mode to: %s", text);
		log(szBuffer);
	}
}

VOID onGatheringStartButton(HWND hWnd)
{
	CHAR szBuffer[256];

	if (WoW::InGame())
	{
		GetDlgItemTextA(hWnd, IDC_MAIN_GATHERING_EDIT_TIME, szBuffer, 64);
		//GetDlgItemTextA(hWnd, IDC_MAIN_GATHERING_EDIT_WAITTIME, szBuffer, 64);

		if (!StartDinnerGather(atoi(szBuffer)))
			SetDlgItemText(hWnd, IDC_MAIN_GATHERING_BUTTON_START, "Start Gathering");
		else
			SetDlgItemText(hWnd, IDC_MAIN_GATHERING_BUTTON_START, "Stop Gathering");
	}
}

VOID onMainStartButton()
{
	if (ReleaseMode == false && !WoW::InGame())
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_PROCESS), NULL, (DLGPROC)LoginProc);

	Thread::Create(Debug::Test, 0, Thread::eType::GUI, Thread::ePriority::High);
}

VOID onKeyDown_Enter(HWND hWnd)
{
	CHAR buffer[4096];
	HWND hwWindow = GetDlgItem(hWnd, IDC_MAIN_EDIT_LOG_INPUT);

	if (hwWindow == GetFocus())
	{
		GetWindowText(GetDlgItem(hWnd, IDC_MAIN_EDIT_LOG_INPUT), buffer, 4096);
		Thread::Create(HandleCommandInput_Wrapper, &buffer, Thread::eType::Key, Thread::ePriority::Low);

		SetDlgItemText(hWnd, IDC_MAIN_EDIT_LOG_INPUT, "");
	}
}

VOID onKeyDown_Back(HWND hWnd)
{
	HWND hwWindow = GetDlgItem(hWnd, IDC_MAIN_EDIT_LOG_INPUT);

	if (hwWindow == GetFocus())
	{
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_BACK))
		{
			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, WM_KEYDOWN, (WPARAM)VK_CONTROL, (LPARAM)VK_CONTROL);

			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, WM_KEYDOWN, (WPARAM)VK_LEFT, (LPARAM)VK_LEFT);
			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, WM_KEYUP, (WPARAM)VK_LEFT, (LPARAM)VK_LEFT);

			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, WM_KEYDOWN, (WPARAM)VK_BACK, (LPARAM)VK_BACK);
			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, WM_KEYUP, (WPARAM)VK_BACK, (LPARAM)VK_BACK);

			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, WM_KEYUP, (WPARAM)VK_CONTROL, (LPARAM)VK_CONTROL);
		}
	}
}

VOID onFishProfileButton(HWND hWnd)
{
	HWND profile;

	profile = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROFILE_SETTINGS), hWnd, (DLGPROC)Settings::ProfileSettingsProc);
	ShowWindow(profile, SW_SHOW);
}

LRESULT CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Main dialog callback function.

	int wmId, wmEvent;

	static HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));

	DWORD dwCheckPos = NULL;

	CHAR szBuffer[256];

    HDC hdcStatic;
	
	switch (message)
	{
	case WM_CTLCOLORSTATIC: // Make static controls transparent to get rid of the grey background.
		hdcStatic = (HDC) wParam;
		SetTextColor(hdcStatic, RGB(0,0,0));
	    SetBkColor(hdcStatic, RGB(255, 255, 255));
		return (INT_PTR)hBrush;

	case WM_INITDIALOG: 
		InitializeOtherControls(hWnd);
		InitTabControl(hWnd);
		
		g_LogVisible = TRUE;
		sprintf_s(szBuffer, "Welcome to %s", Dinner::getDankName().c_str());
		LogAppend(szBuffer);

		SetWindowText(g_hwMainWindow, WoW::GetAccountName().c_str());
		if (!WoW::InGame())
		{
			if (g_chatLog) LogStatus(false);
			LogAppend("Some features will be disabled until player is in game");
		}

		else if (WoW::GetGameState() == WoW::eGameState::InQueue)
			LogAppend("In Queue");

		break;

	case WM_CLOSE:
		g_hwMainWindow = NULL;
		KillTimer(hWnd, IDT_TIMER);

		DestroyWindow(hWnd);
        PostQuitMessage(0);
		//EndDialog(hWnd, LOWORD(wParam));
		return false;

	case WM_NOTIFY: // Tab selection notifier.
		if ((((LPNMHDR)lParam)->code) == TCN_SELCHANGE)
			HandleTabChange(hWnd);

		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
			case VK_DOWN:	onKeyDown_Down(hWnd);	break;
			case VK_UP:		onKeyDown_Up(hWnd);		break;
			//case VK_BACK:	onKeyDown_Back(hWnd);	break;
		}

		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch(wmId)
		{ // CreateThread for HandleCommand.
		case IDOK:										onKeyDown_Enter(hWnd);				break;

		case IDC_MAIN_TAB_HACKS_CHECK_CTT:				onCTTCheck(hWnd, wParam);			break;

		case IDC_MAIN_TAB_HACKS_CHECK_VFLY:				onVFlyCheck(hWnd, wParam);			break;

		case IDC_MAIN_TAB_HACKS_BUTTON_FOG_DENSITY:		onDensityChangeButton(hWnd);		break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_DUMPBASEOBJECT:	onDumpBaseObjectButton();			break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_DUMPLOCATION:	onDumpLocationButton();				break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_RESUME_WOW:		ResumeWoW();						break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_SUSPEND_WOW:		SuspendWoW();						break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_AURAS:			onDumpAurasButton();				break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_DUMPUNITFIELD:	onDumpUnitFieldButton();			break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_ADDRESS_DUMP:	Debug::DumpAddresses();				break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_PLAYER_DUMP:		onDebugPlayerDumpButton();			break;

		case IDD_MAIN_TAB_DEBUG_BUTTON_REFRESH_PLAYER:	onRefreshPlayerButton();			break;

		case IDC_MAIN_TOOLS_BUTTON_TELEPORT:			onTeleportButton(hWnd);				break;

		case IDC_MAIN_TOOLS_BUTTON_RADAR:				onObjectListButton(hWnd);			break;
			
		case IDC_MAIN_TOOLS_BUTTON_OBJECT_WATCH:		onObjectWatchButton(hWnd);			break;

		case IDC_MAIN_TOOLS_BUTTON_HACK_WATCH:			onHackerWatchButton();				break;

		case IDC_MAIN_TOOLS_BUTTON_HIDE_WOW:			onHideWoW();						break;

		case IDC_MAIN_TOOLS_BUTTON_INTERACT_TARGET:		onInteractTarget();					break;

		case IDC_MAIN_TOOLS_BUTTON_TURN_TO:				TurnToTarget();						break;

		case IDC_MAIN_TOOLS_BUTTON_TRACKHIDDEN:			onRadarButton();					break;

		case IDC_MAIN_TOOLS_BUTTON_FOLLOW_TARGET:		onFollowTargetButton();				break;

		case IDC_MAIN_TAB_BUTTON_CHANGEPROCESS:			onChangeProcessButton(hWnd);		break;

		case IDC_MAIN_TAB_MAIN_CHECK_TOPMOST:			onTopmostCheck(hWnd, wParam);		break;

		case IDC_MAIN_HACKS_CHECK_NAMEFILTER:			onNameFilterCheck(hWnd, wParam);	break;

		case IDC_MAIN_TAB_MAIN_CHECK_ANTIAFK:			onAntiAFKCheck(hWnd, wParam);		break;

		case IDC_MAIN_HACKS_CHECK_WIREFRAME:			onRenderWireframeCheck();			break;

		case IDC_MAIN_HACKS_CHECK_HORIZON:				onRenderHorizonCheck();				break;

		case IDC_MAIN_HACKS_CHECK_TERRAIN:				onRenderTerrainCheck();				break;

		case IDC_MAIN_HACKS_CHECK_WATER:				onRenderWaterCheck();				break;

		case IDC_MAIN_HACKS_CHECK_WMO:					onRenderWMOCheck();					break;
			
		case IDC_MAIN_HACKS_CHECK_M2:					onRenderM2Check();					break;

		case IDC_MAIN_TOOLS_BUTTON_RUN_LUA:				onRunLuaButton(hWnd);				break;

		case IDC_MAIN_TAB_DEBUG_BUTTON_CLEANHOOKS:		onCleanHooks();						break;

		case IDC_MAIN_HACKS_CHECK_LANGUAGEHACK:			onLanguageHackCheck();				break;

		case IDC_MAIN_HACKS_CHECK_MAPTELEPORT:			onMapTeleportCheck();				break;

		case IDC_MAIN_HACKS_CHECK_FLY:					MagicChecks(hWnd);					break;

		case IDC_MAIN_HACKS_EDIT_FLY:					onFlyEdit(hWnd);					break;

		case IDC_MAIN_HACKS_CHECK_TRACKHUMANOIDS:		onTrackHumanoidsCheck(hWnd);		break;

		case IDC_MAIN_FISH_CHECK_RENDER:				onFishRenderCheck();				break;

		case IDC_MAIN_TOOLS_CHECK_INGAMEPLAYERCOUNT:	onIngamePlayerCountCheck(hWnd);		break;

		case IDC_MAIN_HACKS_CHECK_ANIMATEPATCH:			onAnimatePatchCheck(hWnd);			break;

		case IDC_FISH_TAB_BUTTON_START:					onFishStartButton(hWnd);			break;

		case IDC_FISH_BUTTON_PROFILE:					onFishProfileButton(hWnd);			break;

		case IDC_MAIN_GATHERING_COMBO_MODE:				onGatheringModeCombo(hWnd, wParam);	break;

		case IDC_MAIN_GATHERING_BUTTON_START:			onGatheringStartButton(hWnd);		break;

		case IDC_MAIN_TAB_BUTTON_START:					onMainStartButton();				break;

		case IDC_FISH_COMBO_PROFILES:					onFishingProfileCombo(hWnd, wParam);break;
		}

		break;

		default:
			return false;
	}

	return true;
}