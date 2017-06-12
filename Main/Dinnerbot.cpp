#include "Dinnerbot.h"
#include "Command.h"

#include <time.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <initializer_list>

#include "Common\Common.h"
#include "Common\Objects.h"

#include "Geometry\Net.h"
#include "Geometry\triangle.h"
#include "Geometry\Coordinate.h"
#include "Engine\Navigation\Pathfinding.h"
#include "Engine\Navigation\Walker.h"

#include "Memory\Hacks.h"
#include "Memory\Hooks.h"
#include "Memory\Endscene.h"

#include "WoW API\Lua.h"
#include "WoW API\Chat.h"
#include "WoW API\Guild.h"
#include "WoW API\Movement.h"
#include "Wow API\Environment.h"
#include "Game\Interactions.h"
#include "WoW API\WorldBorders.h"

#include "Radar\NetEditor.h"

#include "File\NetIO.h"

#include "Pont\HTML.h"

#include "Dialog\Dialog.h"
#include "Dialog\ObjectWatch.h"
#include <CommCtrl.h>

BOOL ReleaseMode = false;

using namespace std;

// Demon (Global) Variables
HWND g_hPInfo = NULL;
HWND g_hwMainWindow = NULL;

__int64 m_ProgramStartTimer = NULL;

#ifndef RELEASE
BOOL g_VerboseInGameChat = false;
#else
BOOL g_VerboseInGameChat = false;
#endif

BOOL g_updateWoWChat = FALSE;
BOOL g_bPreAnimate = FALSE;
BOOL g_LogVisible = FALSE;
BOOL g_InGameChat = TRUE;
BOOL g_VFly = FALSE;

BOOL g_DFish_Logout = TRUE;
BOOL g_rosterMode = FALSE;
BOOL g_DFish_Hide = TRUE;
BOOL g_bFlyHack = FALSE;
BOOL g_chatLog = FALSE;
BOOL g_AntiAFK = TRUE;

WOWPOS g_TeleportPos;

INT g_relogger = -1;
INT g_mapTeleport = 0;

HINSTANCE g_hInst;

// Modular variables
WNDPROC m_wFogDensityEditProc;	// m_wFogDensityEditProc

BOOL m_GeneralEdit = FALSE; // Unreferenced
BOOL m_FDensity = FALSE;	// Unreferenced
BOOL g_LuaCVar = TRUE;

// [Multiphase Spectrographic Goggles]

VOID WoWInfoLoop();
VOID UpdateWoWChat();
VOID onHandleLuaCVar();

LRESULT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CommandProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ProcessListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

namespace Dinner
{
	std::vector<std::string> names { "Danks Never Die", "Mobile-Shrekmachine", "Frenchjesusmachine", "Mushmouthmachine",
		"Tasteman", "Dankbot", "Crunchbot", "Spacemanmachine", "Uptowndankmachine", "Multidankmachine", "Bigdank",
		"Frenchmachine", "Geldplsfortreinbot", "Kappapridebot", "Horskebreed", "Borskebleed", "Horsebead", "Horsetoe",
		"Nonstraight", "GodIhateblackguys", "Dinnerbone", "Dinnerface", "Dinnerboy", "Dinnerbot", "Dineman", "Dinecraft",
		"Horsehouse", "Horsemouse", "Horsefeet", "Dinnershrek", "MorelikeactuallyDinnerbot", "Trainface", "Multishekprogram",
		"Guythathacksyourwowforyou", "Therearesomenicestuffinhereprogram", "Dinnerdank", "Dinnerfrank", "Gospelmachine", "Immobilenutsmachine", "Itsmedankmobile",
		"Horselyshet-Machine", "Tongue Jesus", "Swearerman", "Watermaronbot", "Dinnerboat",
	};

	std::string adviceFilePath = "advice.txt";
	std::string versionName = "";

	std::string &getDankName()
	{
		return names[rand() % Dinner::names.size()];
	}

	std::string getVersionName()
	{
		return string_format("%d.%d.%d%s", RELEASE_VERSION, MAJOR_VERSION, MINOR_VERSION, (ReleaseMode) ? "" : " (Debug)");
	}

	VOID Initialize()
	{
		CHAR		szBuffer[MAX_PATH];
		SYSTEMTIME	st;

		GetSystemTime(&st);
		EnableDebugPrivileges();

		srand((uint)time(0));
		m_ProgramStartTimer = GetTickCount64();
		Dinner::versionName = Dinner::getVersionName();
		sprintf_s(szBuffer, "%s session started on %d/%d/%d", Dinner::getDankName().c_str(), st.wDay, st.wMonth, st.wYear);
		LogFile(szBuffer);
	}

	VOID Exit()
	{
		CHAR szBuffer[MAX_PATH];
		UINT time = 0;

		DinnerChat::Destroy();

		GetWoWWindow();
		CleanAllPatches();
		CleanPatch(Memory::Endscene.getAddress());
		BuildRosterMembersOnlineHTML(NULL);

		sprintf_s(szBuffer, "%s unloaded", Dinner::getDankName().c_str());
		AddChatMessage(szBuffer, SystemMessage);
		if (g_chatLog) LogStatus(false);

		time = GetTickCount64();
		time = (time - m_ProgramStartTimer) / 1000;

		// (playerCount>1)?"are":"is"
		sprintf_s(szBuffer, "%s ran for %d %s", Dinner::getDankName().c_str(), ((time / 60) < 1) ? time : time / 60, ((time / 60) < 1) ? "seconds" : "minutes");
		LogFile(szBuffer);
		LogFile("");
	}
};

VOID OnKeyPress()
{
	Object Target; 
	WOWPOS Buff;

	static WOWPOS LastPos;

	static BOOL  nUsed;
	static BOOL  pressed = false;
	static INT   thread = -1;

	static ULONGLONG timeout = 0;
	static ULONGLONG lastPath = 0;

	int len = 0;

	// Make sure everything is valid.
	if (!LocalPlayer.isValid())
	{
		LocalPlayer.clear();
		GetCurrentManager();

		LocalPlayer.setBase(WoW::GetPlayerBase());
		LocalPlayer.update(LocationInfo | UnitFieldInfo);
		if (!LocalPlayer.isValid())
		{
			AttachWoW();
			return;
		}
	}


	if (!WoW::InGame()) return;

	LocalPlayer.update(LocationInfo | UnitFieldInfo);

	// Wow keys.
	if (GetForegroundWindow() == g_hwWoW)
	{
		/*if ((GetAsyncKeyState(VK_F4)) && !pressed && !m_VFly && !ReleaseMode)
		{
			if (thread == -1)
			{
				thread = DCreateThread((LPTHREAD_START_ROUTINE)GlitchRun, NULL, NULL, NULL);
			}
			else if (thread != -1)
			{
				SetThreadTask(thread, STOPPED);
				thread = -1;
			}

			pressed = TRUE;
		}*/
		if (GetAsyncKeyState(VK_F4) == 0 && !g_VFly && !ReleaseMode)
		{
			pressed = FALSE;
		}

		if (GetAsyncKeyState(VK_F1) && !g_VFly)
		{
			SendKey('W', WM_KEYDOWN);
			SendKey('W', WM_KEYUP);
			Sleep(50);
			SendKey(0x20, WM_KEYDOWN);
			SendKey(0x20, WM_KEYUP);
			Sleep(100);
		}
		
		else if (GetAsyncKeyState(VkKeyScan('=')) && !g_VFly && ReleaseMode)
		{
			CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)QuickJump, NULL, NULL, NULL));
		}

		/*else if (GetAsyncKeyState(VK_F3) && !g_VFly && !ReleaseMode)
		{
			QuickBJump();
		}*/

		else if (GetAsyncKeyState(VK_F2) && !g_VFly && !ReleaseMode && GetTickCount64() - lastPath > 500)
		{
			// Given a navmesh, create a 
			// mode to define hole in mesh
			lastPath = GetTickCount64();
			NetEditor::AddPoint(new p2t::Point(LocalPlayer.Obj.X, LocalPlayer.Obj.Y));
		}

		else if (GetAsyncKeyState(VK_F1) && g_VFly)
		{
			// "Fly" downwards

			if (ValidCoord(LocalPlayer.pos()))
			{
				Buff.Z = LocalPlayer.Obj.Z - 1.0f;
				if (Buff.Z - LocalPlayer.Obj.Z < 5)
				{
					Hack::Movement::StopFall();
					wpm(LocalPlayer.base() + OBJECT_Z, &Buff.Z, sizeof(FLOAT));
				}
			}
			else
				AttachWoW();
		}

		else if (GetAsyncKeyState(VK_F2) && g_VFly)
		{
			// "Fly" upwards

			if (ValidCoord(LocalPlayer.pos()))
			{
				Buff.Z = LocalPlayer.Obj.Z + 1.0f;
				if (Buff.Z - LocalPlayer.Obj.Z < 5)
				{
					Hack::Movement::StopFall();
					wpm(LocalPlayer.base() + OBJECT_Z, &Buff.Z, sizeof(FLOAT));
				}
			}
			else
				AttachWoW();
		}
		else if (GetAsyncKeyState(VK_F3) && g_VFly)
		{
			// Teleport to target
			if (ValidCoord(LocalPlayer.pos()))
			{
				Target = GetCurrentTarget(LocationInfo);
				if (Target.BaseAddress != NULL && ValidCoord(Target.Pos))
				{
					Hack::Movement::Teleport(Target.Pos);
				}
			}
			else
				AttachWoW();
		}
		else if (GetAsyncKeyState(VK_F4) && g_VFly)
		{
			// Teleport to corpse.
			if (ValidCoord(LocalPlayer.pos()))
			{
				if (ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::eCorpse::X), &Buff, sizeof(Buff), 0))
				{
					if (!ValidCoord(Buff)) return; // To teleporting to invalid locations.
					Hack::Movement::Teleport(Buff);
				}
			}
			else
				AttachWoW();

		}
		else if (GetAsyncKeyState(VK_F5) && g_VFly)
		{	
			// * Add map id check.
			// Player structure refresh has returned with error.
			// Save location.
			if (ValidCoord(LocalPlayer.pos()))
			{
				LastPos = LocalPlayer.pos();
			}
		}
		else if (GetAsyncKeyState(VK_F6) && g_VFly)
		{
			if (ValidCoord(LastPos))
			{
				Hack::Movement::StopFall();			// Set local player's movement flags to 0x8 (Idle).
				Hack::Movement::Teleport(LastPos);  // Teleport to saved location.
				WriteProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.base() + OBJECT_ROT), &LastPos.Rotation, sizeof(LastPos.Rotation), NULL);
			}
		}
		else if (GetAsyncKeyState(0xc0) && !ReleaseMode)
		{
			CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)QuickJump, NULL, NULL, NULL));
		}
	}

}

VOID onEnteringWorld()
{
	// PLAYER_ENTERING_WORLD
	AttachWoW();
	EnableHacks();
	log("Attached");

	//if (!IsSubroutineHooked(g_dwEndScene))
	//	DCreateThread((LPTHREAD_START_ROUTINE)DinnerChat::Setup, NULL, NULL, NULL);
}

VOID onLeavingWorld()
{
	// PLAYER_LEAVING_WORLD

	// Stop any WoW info loop threads.
	//Thread::FindType(TYPE_INFOLOOP)->stop();

	LogAppend("Not ingame");
	if (WoW::isRunning())
	{
		// We need to destroy the LUA UI first before removing the endscene hook
		DinnerChat::Destroy();

		CleanPatch(Memory::Endscene.getAddress());
		CleanAllPatches();
		GetWoWWindow();
	}

	if (g_chatLog) LogStatus(false);

	// Pont mode
	if (g_updateWoWChat && g_rosterMode && g_chatLog)
		LogChat("Pont offline.");

	BuildRosterMembersOnlineHTML(NULL);
	SetDlgItemText(g_hwMainWindow, IDC_MAIN_TAB_GROUPBOX_PLAYER, "Not Ingame");
}

VOID HandleGamestateChange()
{
	using namespace Memory;
	DWORD startTimer = NULL;
	static BOOL MQ = FALSE;

	if (WoW::InGame())
	{
		if (!MQ) // Player entering world event
		{
			onEnteringWorld();
			MQ = TRUE;
		}

		// Handle in-game /drun scripts.
		if (Endscene.CanExecute())
		{
			if (g_LuaCVar) onHandleLuaCVar();
		}
	}
	else
	{
		if (MQ) // Player leaving world event
		{
			onLeavingWorld();
			MQ = FALSE;
		}
	}
}

VOID WoWInfoLoop()
{
	Thread *thread = Thread::GetCurrent();

	while (thread && thread->running())
	{
		Sleep(30);

		// Retrieve any new in-game messages.
		UpdateWoWChat();

		// Game state logic and hook changes.
		HandleGamestateChange();

		// If our wow window is in the foreground, allow hotkeys to work.
		if (WoW::InGame())
		{
			// Handle key presses.
			OnKeyPress(); 
		}

		//if (g_bFlyHack && LocalPlayer.UnitField.FlightSpeed != g_fFlyHack)
			//SetFlightSpeed(g_fFlyHack);
	}

	// Notify thread handler that the current thread has exited.
#ifndef RELEASE
	thread->exit("WoWInfoLoop");
#else
	thread->exit();
#endif
}

VOID BGQueueProc()
{
	if (GetBGQueue() == BG_QUEUE_POP)
	{
		if (GetForegroundWindow() != g_hwWoW)
			SetForegroundWindow(g_hwWoW);  // If the BG Queue popped, set WoW to highest z-order.
	}
}


VOID WardenProc()
{
	Object	ObjectWatch;
	CHAR	szBuffer[MAX_PATH];
	CHAR	strBuffer[MAX_PATH];
	CHAR	*strPtr				= NULL;

	static bool enabledWarden = true;


	// On 04/03/2015 Molten turned on the anticheat server checks. This is my attempt at bypassing it.
	// The warden timeout is 3 minutes, so if we reload warden before that timeout, Warden will send a keep alive packet to the server, signifying it is still enabled.
	// Update: ofc this doesn't work, Warden::Load does not contain the possibly unique code that is sent by the server.

	/*if (GetTickCount64() - g_wardenDisableTimer > WARDEN_TIMEOUT - 30000 && !IsWardenLoaded())
	{
		CleanAllPatches();
		WardenLoad();
		if (!IsWardenLoaded())
		{
			log("Unable to load Warden, expect to be disconnected in approximately 30 seconds.");
		}
		else
		{
			log("Warden loaded for server keep alive.");
			enabledWarden = true;
			Sleep(1500);
		}

	}*/

	/*if (ObjectWatch.BaseAddress = IsWardenLoaded()) // Here we use ObjectWatch's base address as a buffer.
	{
		sprintf(szBuffer, "Warden found at 0x%p, attempting to disable.", ObjectWatch.BaseAddress);
		LogAppend(szBuffer);
		if (WardenUnload())
		{
			LogAppend("Warden Anti-Cheat unloaded.");

			if (enabledWarden)
			{
				EnableHacks();
				enabledWarden = false;
			}
		}
		else
			LogAppend("Error unloading Warden.");

	}*/


	if (g_Searching)
	{   // An incidental object search section.]
		strncpy(szBuffer, g_szSearch, strlen(g_szSearch) + 1);
		strPtr = strtok(szBuffer, ",.");
		while (strPtr != NULL)
		{
			ObjectWatch = GetObjectByName(strPtr);
			if (ValidObject(ObjectWatch))
			{
				ObjectWatch = DGetObjectEx(ObjectWatch.BaseAddress, BaseObjectInfo | LocationInfo | NameInfo);
				memcpy(&g_TeleportPos, &ObjectWatch.Pos, sizeof(WOWPOS));
				sprintf(strBuffer, "Found %s at (%0.2f, %0.2f, %0.2f); %0.2f yds away", ObjectWatch.Name, ObjectWatch.X, ObjectWatch.Y, ObjectWatch.Z, LocalPlayer.distance(ObjectWatch.Pos));
				if (g_OWReport) AddChatMessage(strBuffer, SystemMessage);
					LogAppend(strBuffer);

				g_Searching = FALSE;
			}		

			strPtr = strtok(NULL, ",.");
		}	

		if (!g_Searching)
			SetDlgItemText(g_hwObjectWatch, IDC_OBJECTWATCH_BUTTON_SEARCH, "Search");
	}

	/* TODO: Transport monitor feature
	if (g_Transport)
	{
	} */
}

VOID EnvironmentProc()
{
	if (GetFogDensity() != g_fFogDensity && IsFogColorPatched())
	{
		RestoreFogColorProtection();
		if (!IsFogColorPatched())
			PatchFogColorProtection();
		
		SetFogDensity(g_fFogDensity);
	}
}

VOID onHandleLuaCVar()
{
	CHAR buffer[4096];
	CHAR buffers[4097];
	
	// If the in-game flag was set, then handle it.
	if (DinnerChat::MaskHasFlag(TOGGLE_PARSE))
	{	
		// Set in-game flag to off to indicate that we've handled it.
		DinnerChat::FlagOff(TOGGLE_PARSE);

		Lua::DoString("DE_string = GetCVar(\"DE\")");
		Lua::GetText("DE_string", buffers);
		sprintf_s(buffer, "!%s", buffers);

		Commands::handler(buffer);
	}
}

using namespace Engine::Navigation;
using namespace Geometry;

VOID netMove(WOWPOS *end)
{
	// Pathfind to the destination.
	Pathfinding pf(&NetEditor::net);
	Thread *thread = Thread::GetCurrent();

	vector<WoWPos> path;

	//Engine::Navigation::move(-67.42, 2171.82, 30.65);
	pf.findPath(LocalPlayer.pos(), *end);
	if (pf.pathFound)
	{
		vlog("Path found along %d nodes", pf.getPathNodeSize());
		NetEditor::ClearPath();
		path = pf.getPath();
		thread->setTask(Thread::eTask::Moving);
		NetEditor::ShowPath(path);

		// Walk path
		/*Walker walker(path, thread);
		walker.setTolerance(5);
		walker.setWait(10);
		walker.walk();

		NetEditor::ClearPath();*/
	}

	delete end;
#ifndef RELEASE
	thread->exit("netMove");
#else
	thread->exit();
#endif
}

VOID onHandleMapTeleport()
{
	CHAR buffer[256];
	WOWPOS world, map;
	WOWPOS *end = new WOWPOS;

	if (DinnerChat::MaskHasFlag(TOGGLE_NEWMAP))
	{
		// Set newmap toggle bit to off to indicate we have handled the latest map goto.
		DinnerChat::FlagOff(TOGGLE_NEWMAP); 

		// Get the map coordinates from the lua stack.
		Lua::GetText("dMapX", buffer);
		map.X = atof(buffer);

		Lua::GetText("dMapY", buffer);
		map.Y = atof(buffer);

		// Get the map zone id.
		Lua::GetText("dMapZ", buffer);

		// Convert the map coordinates to world coordinates.
		world = ConvertMapToWorld(ConvertGameToDBC(atoi(buffer), LocalPlayer.mapId()), map);
		end->X = world.X; 
		end->Y = world.Y;

		if (g_mapTeleport == 2)		 // teleport
		{
			Hack::Movement::Teleport(world.X, world.Y, LocalPlayer.Obj.Z);
			delete end;
		}

		else if (g_mapTeleport == 1) // move
		{
			ClickToMove(world.X, world.Y, LocalPlayer.Obj.Z);
			delete end;
		}

		else if (g_mapTeleport == 3) // net move
		{
			Thread *thread = NULL;
			if ((thread = Thread::FindType(Thread::eType::Navigation)))
				thread->stop();

			Thread::Create(netMove, end, Thread::eType::Navigation);
		}
	}	
}

VOID RestartWoW(char *wowPath)
{
	PROCESS_INFORMATION ProcessInfo; 

	STARTUPINFO StartupInfo; //This is an [in] parameter

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo); //Only compulsory field

	if (CreateProcess(wowPath, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &StartupInfo, &ProcessInfo))
	{ 
		WaitForSingleObject(ProcessInfo.hProcess, 0);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}

	WoW::handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInfo.dwProcessId);

	if (WoW::handle != NULL)
	{
		GetWoWWindow();

		ShowWindow(g_hwWoW, FALSE);
		//InitializeBot(); // Set up fetch threads, start main dialog, and initialize global variables.
		SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_HACKS_CHECK_LANGUAGEHACK, BM_SETCHECK, 0, 0);
		SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_TAB_MAIN_CHECK_ANTIAFK, BM_SETCHECK, 1, 0);
		SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_FISH_CHECK_LOGOUT, BM_SETCHECK, 0, 0);
		SendDlgItemMessage(g_hwMainWindow, IDC_MAIN_FISH_CHECK_HIDE, BM_SETCHECK, 0, 0);
		SetDlgItemText(g_hwMainWindow, IDC_MAIN_TAB_HACKS_EDIT_FOG_DENSITY, "1.00");
	}
	
#ifndef RELEASE
	Thread::DeleteCurrent("RestartWoW");
#else
	Thread::DeleteCurrent();
#endif
}

VOID ReloggerProc()
{
	static ULONGLONG start = 0;
	static ULONGLONG timeout = 10000;
	static int loginTries = 0;
	static bool restarting = false;

	// Determine if pont mode is enabled.
	bool pont = g_updateWoWChat && g_rosterMode && g_chatLog;

	string buffer;

	CHAR *wowPath = "C:\\Program Files (x86)\\Apache Software Foundation\\Apache2.2\\Pont\\Client\\Wow.exe";

	int diff = 0;

	if (GetTickCount64() - start >= timeout && restarting)
	{
		log("WoW restarted");
		restarting = false;
	}

	else if (restarting)
	{
		return;
	}

	if (loginTries > 50)
	{
		log("Too many logins have been attempted. The client may possibly be bugged");
		log("Restarting wow");

		TerminateProcess(WoW::handle, 0);

		Thread::Create((RestartWoW), (LPVOID)(wowPath));
		loginTries = 0;
		restarting = true;
	}

	// If we're at the login screen, we're not in queue, and past the login timeout, let's log in.
	if (WoW::IsLoginScreen() && (WoW::GetGameState() != WoW::eGameState::InQueue) && (GetTickCount64() - start >= timeout))
	{
		log("Logging back in");

		/*"if RealmListOkButton:IsVisible() then "
			" "
		"end ";


		"GetNumRealms(1) "
			"getglobal(\"RealmList\") ";*/

		/*
		function GlueBox(text)
	GlueDialog_Show("OKAY", text, "")
end

function GetRealmName(index)
	return GetRealmInfo(1, index);
end

function IsRealmDown(index)
	name, numCharacters, invalidRealm, realmDown, currentRealm, pvp, rp, load, locked, major, 
	minor, revision, build = GetRealmInfo(1, index);

	if realmDown then 
		return true
	else 
		return false
	end
end

	name, numCharacters, invalidRealm, realmDown, currentRealm, pvp, rp, load, locked, major, minor,
	revision, build = GetRealmInfo(1, 1);

		*/

		if (0)//CanExecuteMainThread())
		{
			buffer = string_format("DefaultServerLogin(\"%s\", \"%s\")", g_lastAccount, g_lastPass);
			Lua::Run(buffer.c_str());
		}
		else
		{
			Login(g_lastAccount, g_lastPass);
		}

		start = GetTickCount();
		loginTries++;
	}
	else if (WoW::IsCharacterSelectScreen() && pont)
	{
		loginTries = 0;
		// Assuming we can hook endscene properly,
		if (0)//CanExecuteMainThread())
		{
			// Lua version
			Lua::Run(
			"for i = 1, GetNumCharacters() do "
				"if GetCharacterInfo(i) == \"Pont\" then "
					"CharacterSelect_SelectCharacter(i) "
					"CharacterSelect_EnterWorld() "
				"end "
			"end"
			);
		}
		else
		{
			// Keyboard version
			if (diff == 0)
			{
				SendKey(0x0D, WM_KEYDOWN);	// 0x0D == EnterKey
				SendKey(0x0D, WM_KEYUP);
				log("Pressed enter");
			}

			for (int x = 0; x < abs(diff); x++)
			{
				if (diff > 0)
				{
					SendKey(VK_UP, WM_KEYDOWN);
					SendKey(VK_UP, WM_KEYUP);
				}
				else
				{
					SendKey(VK_DOWN, WM_KEYDOWN);
					SendKey(VK_DOWN, WM_KEYUP);
				}
			}
		}

	}
}

//Morphs:
//25209 - drum set
//25517 - val'kyr
//27403 - taller val'kyr
//25071 - apple

VOID PontSafeCheck()
{
	static bool found = false;

	if (PONT_MODE)
	{
		if (!found && PlayersInProximity(10) > 0)
		{
			std::vector<std::string> &playerNames = GetPlayersInProximity(20);
			std::string message = "Pont has been discovered by: ";

			// Build discovered string
			for (auto &name : playerNames)
			{
				message += name;
				message += ", ";
			}

			message += "Pont";
			// Send message to each of the Pont commanders
			for (auto &each : DinnerChat::grandmothers)
			{
				Lua::sendWhisper(message, each);
				DinnerChat::SendWhisper(message, each);
			}

			vlog("[WARNING]: %s", message.c_str());
			Lua::vDo("SendChatMessage(\"%s\", \"SAY\", nil, nil)", PONT_DANGER_MESSAGE);
			found = true;
		}
	}
}

VOID GuildRosterProc(Guild *carpe)
{
	if (carpe == NULL || !WoW::InGame()) 
	{
		Thread::DeleteCurrent();
//#ifndef RELEASE
//		Thread::DeleteCurrent("GuildRosterProc");
//#else
//		Thread::DeleteCurrent();
//#endif
		return; 
	}

	// In-game roster update.
	carpe->UpdateGuildRoster();

	// WoW's weird guild roster update requires a sleep or two.
	Sleep(1000);

	// Check if Pont has been discovered
	PontSafeCheck();

	// Pull stats from the recent update.
	carpe->updateInfo();
	if (!carpe->CompareIngameRoster())
	{
		log("Building new roster...");

		// Retrieve member names, class, locations.
		carpe->buildRoster();

		// Build HTML from guild roster.
		BuildRosterHTML(carpe);
		BuildRosterMembersOnlineHTML(carpe);
		log("Guild roster has been updated.");
	}

	Thread::DeleteCurrent();
	//#ifndef RELEASE
//	Thread::DeleteCurrent("GuildRosterProc");
//#else
//	Thread::DeleteCurrent();
//#endif
}

VOID GuildRosterProc_Wrapper()
{
	static ULONGLONG lastCall = GetTickCount64();
	static Guild carpe("Carpe Noctem");
	static bool net = false;

	if (!g_rosterMode) return;

	// If we have not realized we aren't in game with roster mode enabled, clear the roster.
	// This is so when we get back in game we are able to fix the number of members online.
	if (!WoW::InGame()) 
	{
		if (!net)
		{
			carpe.clear();
			net = true;
		}

		return; 
	}

	else if (GetTickCount64() - lastCall > 10000)
	{
		net = false;
		lastCall = GetTickCount64();
		Thread::Create(GuildRosterProc, &carpe);
	}
}

/* WinAPI timer function called every 500 milliseconds.*/
VOID CALLBACK MainTimerProc(HWND hWnd, UINT uMessage, UINT_PTR uEventId, DWORD dwTime)
{
	CHAR szBuffer[512];

	//static __int64	memoryUsageTimer = GetTickCount64();

	static BOOL	WasInGame, WasInQueue, TimerStart;
	static INT	timerCount;
	
	DWORD processId;

	GetWindowThreadProcessId(g_hwWoW, &processId); // Get Id of process that is using wow's window handle.

	if (g_AntiAFK) AntiAFK();
	if (WoW::handle == NULL) WasInQueue = FALSE;

	// Compare with our current handle of wow.
	if (processId != GetProcessId(WoW::handle) || !IsWindow(g_hwWoW))
	{
		GetWoWWindow(); 
	}

	GuildRosterProc_Wrapper();
	
	if (!WoW::InGame() && g_relogger == 1)
	{
		ReloggerProc();
	}

	if (WoW::InGame())
	{
		// Update the local player
		LocalPlayer.setBase(WoW::GetPlayerBase());

		if (Memory::Endscene.CanExecute())
		{
			if (g_mapTeleport != 0) onHandleMapTeleport();
		}
		
		if (!Memory::Endscene.IsHooked())
		{
			if (Memory::Endscene.Start())
			{
				// Pont mode
				if (g_updateWoWChat && g_rosterMode && g_chatLog)
				{
					SYSTEMTIME	st;

					GetSystemTime(&st);
					sprintf_s(szBuffer, "Pont logged in on %d/%d/%d (local).", st.wDay, st.wMonth, st.wYear);
					LogChat(szBuffer);
				}
			}

		}

		if (!DinnerChat::MaskHasFlag(TOGGLE_CHAT))
		{
			//DinnerChat::Setup();
			if (!Thread::FindType(Thread::eType::Chat))
				Thread::Create(DinnerChat::Setup, NULL, Thread::eType::Chat, Thread::ePriority::Medium);
		}

		WardenProc();
		BGQueueProc();
		EnvironmentProc();
		UpdateMainDisplay(hWnd);
		DinnerChat::PlayerCount();
	}	

	else if (WoW::GetGameState() == WoW::InQueue && WoW::GetQueuePosition() > 0)
	{   
		// Checks game queue state.
		if (!WasInQueue) WasInQueue = TRUE;

		sprintf_s(szBuffer, "%s - %d", WoW::GetRealmName().c_str(), WoW::GetQueuePosition());
		SetDlgItemText(g_hwMainWindow, IDC_MAIN_TAB_GROUPBOX_PLAYER, szBuffer);
	}

	else if (WasInQueue && !WoW::IsLoginScreen())
	{
		timerCount = 0;
		TimerStart = TRUE;
		WasInQueue = FALSE;
		SetForegroundWindow(g_hwWoW);
		SetDlgItemText(g_hwMainWindow, IDC_MAIN_TAB_GROUPBOX_PLAYER, "Character Select");
	}

	if (TimerStart && timerCount < 2)
	{
		if (GetForegroundWindow() != g_hwWoW) SetForegroundWindow(g_hwWoW);
		timerCount++;
	}
}

CHAR *ParseNameFromLBBuffer(CHAR *szLBBuffer)
{
	return strtok(szLBBuffer, " - ");
}

// Keyboard shortcut for last command.
VOID onKeyDown_Up(HWND hWnd)
{
	CHAR buffer[256];

	int len = 0;
	
	HWND hwWindow = GetDlgItem(hWnd, IDC_MAIN_EDIT_LOG_INPUT);

	// If the log has focus, handle any input.
	if (hwWindow == GetFocus())
	{
		if (Commands::commandHistory.size() > 0)
		{
			// Get the last entered command and display it.
			Commands::commandIndex = (Commands::commandIndex - 1) % Commands::commandHistory.size();
			sprintf_s(buffer, "%s", Commands::commandHistory.at(Commands::commandIndex).c_str());

			len = strlen(buffer);
			SetDlgItemText(hWnd, IDC_MAIN_EDIT_LOG_INPUT, buffer);
			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, EM_SETSEL, len, len);
		}
	}
}

// Keyboard shortcut for last command.
VOID onKeyDown_Down(HWND hWnd)
{
	CHAR buffer[256];

	int len = 0;
	
	HWND hwWindow = GetDlgItem(hWnd, IDC_MAIN_EDIT_LOG_INPUT);

	// If the log has focus, handle any input.
	if (hwWindow == GetFocus())
	{
		if (Commands::commandHistory.size() > 0)
		{
			// Get the last entered command and display it.
			Commands::commandIndex = (Commands::commandIndex + 1) % Commands::commandHistory.size();
			sprintf_s(buffer, "%s", Commands::commandHistory[Commands::commandIndex].c_str());
		
			len = strlen(buffer);
			SetDlgItemText(hWnd, IDC_MAIN_EDIT_LOG_INPUT, buffer);
			SendDlgItemMessage(hWnd, IDC_MAIN_EDIT_LOG_INPUT, EM_SETSEL, len, len);

		}
	}
}

VOID HandleCommandInput_Wrapper(CONST CHAR *msg)
{
	Commands::handler(msg);

#ifndef RELEASE
	Thread::DeleteCurrent("HandleCommandInput_Wrapper");
#else
	Thread::DeleteCurrent();
#endif
}

#define DINNER_CREATE_WINDOW WM_APP + 1

// Entry-point
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND	hDlg;
	MSG		msg;
	INT		ret;

	InitCommonControls();
	Dinner::Initialize();

	DialogBoxA(hInstance, MAKEINTRESOURCE(IDD_PROCESS), NULL, (DLGPROC)ProcessListProc);

	if (WoW::handle != NULL)
	{
		// Create main dialog.
		hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)DialogProc);
		ShowWindow(hDlg, SW_SHOW); 

		// Process dialog messages.
		while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if (!IsDialogMessage(hDlg, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			switch (msg.message)
			{
				case WM_KEYDOWN:
					SendMessage(hDlg, msg.message, msg.wParam, msg.lParam);
					break;
			}
		}
	}

	// Cleanup and shutdown.
	Dinner::Exit();

	return 0;
}
