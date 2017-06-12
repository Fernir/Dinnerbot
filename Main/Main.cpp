#include "Main.h"

#include "Memory\Endscene.h"
#include "Common\Common.h"
#include "Common\Objects.h"

#include "Radar\Drawing.h"

#include "Main\Debug.h"

#include <vector>

using namespace std;
using namespace Memory;

//#include "Translate.h"

HWND g_hwWoW = NULL;

DWORD g_dwCurrentManager;

//Object LocalPlayer;

WNDENUMPROC CALLBACK WoWWindowProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwPID;

	GetWindowThreadProcessId(hWnd, &dwPID);
	if (dwPID == GetProcessId(WoW::handle))
	{
		WoW::window = hWnd;
		g_hwWoW = hWnd;

		return (WNDENUMPROC)FALSE;
	}

	return (WNDENUMPROC)TRUE;
}

BOOL GetWoWWindow()
{
	return EnumWindows((WNDENUMPROC)WoWWindowProc, NULL);
}

BOOL InitializeBot()
{
	GetWoWWindow();
	Endscene.reset(1000);
	
	AttachWoW();
	if (WoW::GetGameState() < 2)
	{
		if (!rpm(PLAYER_NAME, &LocalPlayer.Obj.Name, sizeof(LocalPlayer.Obj.Name))) return FALSE;
		if (!(GetCurrentManager()))	return FALSE;
	}
	
	return TRUE;
}

namespace Test
{

	VOID Debug()
	{
		return;
	}
	

		/*while (go)
		{
			go = false;
			portal.update(GetObjectByDisplayID(0x0000052F));
			portal.update(BaseObjectInfo | GameObjectInfo | LocationInfo);

			if (portal.isValid() && portal.distance() < 5)
			{
				log("Click");
				portal.interact();
			}

			Sleep(100);
		} */

	/*VOID SpawnKhadgarServant()
	{
		INT state = 1;

		while (state > 0)
		{
			Sleep(20);
		}

		state = 1;
		while (1)
		{	
			Sleep(300);
			SendKey('3', WM_KEYDOWN);
			SendKey('3', WM_KEYUP);
			Sleep(500);
			SendKey('3', WM_KEYDOWN);
			SendKey('3', WM_KEYUP);
			Sleep(500);
			SendKey('4', WM_KEYDOWN);
			SendKey('4', WM_KEYUP);
			Sleep(100);
			SendKey('5', WM_KEYDOWN);
			SendKey('5', WM_KEYUP);
			SendKey('D', WM_KEYDOWN);
			Sleep(50);
			SendKey('D', WM_KEYUP);
			Sleep(2000);
		}

		ToggleCTM(TRUE);
		Sleep(10000);
		GetWoWWindow();
		CleanAllPatches();
	}

	BOOL Move(WOWPOS pos)
	{
		while (GetDistance(pos) > 0.5f && LocalPlayer.isAlive()) 
		{
			if (GetCTMState() != CTM_Moving)
				MoveToPos(pos);

			Sleep(50);
		}
	
		return TRUE;
	}

	VOID PathToGoldShire()
	{
		CHAR szBuffer[64];
		WOWPOS Starting = {-8949.950195, -132.492996, 83.531197};
		WOWPOS PathToGoldShire[] = {{-8964.990234, -120.878181, 83.217705}, 
		{-9003.118164, -89.549004, 86.099777}, 
		{-9036.206055, -98.777351, 87.756058},
		{-9046.488281, -90.633873, 88.046806},
		{-9052.085938, -48.725754, 88.351685},
		{-9072.718750, -45.259205, 87.929977},
		{-9166.379883, -110.561760, 72.193245},
		{-9228.950195, -106.208191, 71.134232},
		{-9298.322266, -58.777988, 67.223503},
		{-9334.170898, -50.897675, 65.930618},
		{-9370.490234, -20.750200, 62.800690},
		{-9445.342773, 61.616367, 56.113052}};
	
		if (GetDistance3D(LocalPlayer.pos(), Starting) > 1)
		{
			AddChatMessage("We are nowhere near the beginning. We can't start until we know we're near the beginning!", "#Goldshire2014", TurqoiseMessage);
			return;
		}
	
		AddChatMessage("HERE WE GOOO!", "Pussy Willow", TurqoiseMessage);
		for (int x = 0; x < 12; x++)
		{
			sprintf_s(szBuffer, "Next node: (%0.2f, %0.2f, %0.2f)", PathToGoldShire[x].X, PathToGoldShire[x].Y, PathToGoldShire[x].Z);
			AddChatMessage(szBuffer, "#Goldshire2014", TurqoiseMessage);
			LogAppend(szBuffer);
			Move(PathToGoldShire[x]);
		}

		AddChatMessage("We are at the end of the willow", "#Goldshire2014", TurqoiseMessage);
	}

	VOID LedgeJump(WOWPOS *nodes)
	{
		Move(nodes[0]); // Safety node.
		Move(nodes[2]); // In front of ledge.
		NudgeJump();    // Nudge jump up.
		while (GetPlayerMovementState() != 0x80000000) // Wait to fall.
			Sleep(100);
	
		while (GetDistance3D(LocalPlayer.pos(), nodes[3]) > 0.5f)
			Move(nodes[3]); // Adjust ledge position (CTM is a dick).
	
		Move(nodes[4]); // Move to edge of ledge.
		TurnToPos(nodes[5]); // Turn to mini-roof spot.
		QuickJump(); // Quick jump towards spot.
		Sleep(500); 

		while (GetPlayerMovementState() != 0x80000000)
		{   // Failure analysis.
			Sleep(100);
			AddChatMessage("Waiting", "#GoldshireParkour", TurqoiseMessage);
		}
	}

	VOID MiniRoofJump(WOWPOS *nodes)
	{
		NudgeJump();
		Sleep(500);

		Move(nodes[7]);
		Move(nodes[8]);

		TurnToPos(nodes[9]);
		QuickJump();
		Sleep(500);
		while (GetPlayerMovementState() != 0x80000000)
		{   // Failure analysis.
			Sleep(100);
			AddChatMessage("Waiting", "#GoldshireParkour", TurqoiseMessage);
		}

		NudgeJump();
		Sleep(500);
	}

	VOID JumpAroundGoldShire()
	{
		WOWPOS nodes[] = {{-9465.875977, 49.512707, 56.971348}, // Safety node for window ledge.
		{-9452.779297, 48.042229, 57.005680}, // Safety node for roof jump.
		{-9467.588867, 42.136837, 56.660099}, // In front of window ledge, ready to jump up.
		{-9469.455078, 41.241840, 58.048553}, // Adjust ledge position.
		{-9466.927734, 40.954956, 58.048985}, // On window ledge, ready to attempt jump.
		{-9463.855469, 40.240414, 59.508591}, // In mini-roof spot, need to nudge jump forward.
		{-9463.785156, 40.219982, 61.148262}, // Result after nudge jumping.
		{-9463.217773, 42.022556, 61.582386}, // Move out of resulting spot.
		{-9459.035156, 43.068314, 64.182236}, // Middle of mini-roof.
		{-9456.848633, 40.580795, 65.721664}, // Roof spot.
		{-9466.335938, 27.172396, 74.559464}, // Middle of Top roof.
		{-9471.164063, 43.966415, 75.378998}, // Endbb
		};

		do
		{
			do
			{
				LedgeJump(nodes);
				Sleep(1000);
			} while (LocalPlayer.Obj.Z < floor(nodes[4].Z));

			// We are on the mini-roof.
			MiniRoofJump(nodes);
			if (LocalPlayer.Obj.Z < floor(nodes[9].Z))
			{
				Move(nodes[1]);
				continue;
			}

			// We are on the roof!
		} while (LocalPlayer.Obj.Z < floor(nodes[11].Z));
	}

	VOID FlyCraze()
	{	// This function must be very responsive, so it may use a lot of cpu.
		WOWPOS waypoints[] = {
			{5628.67, -659.16, 352.68},		// Start
			{5465.90, -1130.85, 279.55},
			{5510.28, -1213.09, 262.45},
			{5501.80, -1236.56, 257.83},
			{5493.88, -1247.91, 258.20},
			{5422.48, -1190.77, 256.31},
			{5391.85, -1181.63, 263.19},
			{5376.94, -1190.43, 273.69},
			{5370.06, -1249.01, 282.96},
			{5365.38, -1276.91, 248.64},
			{5366.31, -1278.43, 244.96},
			{5362.27, -1297.75, 243.47},
			{5336.56, -1302.58, 245.10},
			{5306.44, -1325.23, 244.78},
			{5233.35, -1543.38, 299.90},
			{5195.97, -1620.09, 359.87},
			{5149.42, -1632.92, 417.43},
			{5165.75, -1656.20, 418.20},
			{5171.89, -1666.37, 421.24},
			{5180.39, -1681.64, 418.71},

		};

		UINT index = 1;
		float ayy = 555555;

		if (LocalPlayer.mapId() != 571)
			return;

		FlyHack(true, 200);
		if (GetDistance(waypoints[0]) > 4)
			Teleport(waypoints[0]);

		while (index < (sizeof(waypoints) / sizeof(WOWPOS)))
		{
			// Move to next waypoint.

			// Wait until we reach that waypoint.
			while (GetDistance3D(waypoints[index], LocalPlayer.pos()) > 5)
			{
				if (GetCTMState() != CTM_Moving)
				{
					ClickToMove(waypoints[index]);
					WriteProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_TurnSpeed), &ayy, sizeof(ayy), 0);
				}

				LocalPlayer.update(LocationInfo);
			}

			log("go ayy");
			// Set next waypoint.
			index++;
		}

		log("we done");
		FlyHack(false, 0);
	}

	// Lua unlocker, poate.
	DWORD SetBreakpoint(HANDLE thread, DWORD address)
	{
		INT bpReg = 0;
		SuspendWoW();

		CONTEXT context;
	
		if (GetThreadContext(thread, &context))
		{
			DWORD enable = 0x1 << (bpReg *2);
			DWORD bpType = 0x00F0000 << (bpReg * 4);
			DWORD mask = enable | bpType;

			switch (bpReg)
			{
			case 0: context.Dr0 = address; log("Found a guy1"); break;
			case 1: context.Dr1 = address; log("Found a guy2"); break;
			case 2: context.Dr2 = address; log("Found a guy3"); break;
			case 3: context.Dr3 = address; log("Found a guy4"); break;
			}

			context.Dr7 |= mask;
			SetThreadContext(thread, &context);
		}

		ResumeWoW();

		return true;
	}

	Object PlayerTargetted_Callback(Object CurrentObject)
	{
		CHAR szBuffer[4000];
	
		CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | UnitFieldInfo);

		if (CompareWGUIDS(CurrentObject.GUID, LocalPlayer.guid()))
			return CurrentObject;

		if (CurrentObject.UnitField.Target == LocalPlayer.guid().low)
		{
			DGetObjectName(CurrentObject, CurrentObject.Name);

			sprintf_s(szBuffer, "0x%x vs 0x%x", CurrentObject.GUID.low, LocalPlayer.guid().low);
			log(szBuffer);

			CurrentObject.BYTES_1 = 0xFA;
		}

		return CurrentObject;
	}

	#define DG_IDLE			0x0
	#define DG_PREFIGHT		0x1
	#define DG_FIGHTMOVE	0x2
	#define DG_FIGHT		0x3
	#define DG_LOOT			0x4
	#define DG_TRAVEL		0x5

	#define PI 3.141592653589793238462643383279502884

	Object FindCat()
	{
		Object cat;
		Object CurrentObject, NextObject;

		memset(&cat, 0, sizeof(cat));
		if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + OBJECT_MANAGER_FIRST), &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress), NULL)))
		{
			memset(&CurrentObject, 0, sizeof(CurrentObject));
			return cat;
		}

		CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo);
		while (ValidObject(CurrentObject))
		{
			if (CurrentObject.Type_ID == OT_PLAYER)
			{ //0x000072DF
				CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | UnitFieldInfo);
				if (CurrentObject.UnitField.DisplayId == 892 || CurrentObject.UnitField.DisplayId == 8571 || CurrentObject.UnitField.DisplayId == 29407)
				{
					return cat;
				}
			}

			NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo);
			if (NextObject.BaseAddress == CurrentObject.BaseAddress)
				break;
			else
				CurrentObject = NextObject;
		}

		return cat;
	}

	/*
	VOID PortalBot()
	{
		char wordMatches[][] = {"port", "dal"};
		char playerName[256];
		int words = 2;
		while (1)
		{
			if (SearchChat(wordMatches, words, "WHISPER", playerName))
			{
				InviteParty(playerName);
				if player is nearby, cast
					CastSpellByName("Portal: Dalaran");
				otherwise, wait a minute, if not then kick them.
				- wait on cast
					- while waiting, accept any tips from player

			}

		}
	}*/

	/*VOID MouseOverTarget()
	{
		CObject target(GetObjectByGUIDEx(GetMouseOverGUID(), BaseObjectInfo | GameObjectInfo | UnitFieldInfo | LocationInfo | NameInfo));

		vlog("Mouse Over Target:");
		DumpBaseObject(target.object());
		switch(target.type())
		{
		case OT_UNIT:

		case OT_PLAYER:
			DumpUnitField(target.unitField());
			break;

		case OT_GAMEOBJ:
			DumpGameObjectField(target.gameObjectField());
			break;
		}
	
		DumpLocation(target.object());
	}

	BOOL Ayys()
	{
		DWORD_PTR eax = 0;
		DWORD_PTR address = 0x00B499A4; // address of the instruction after the call for hardware breakpoint

		vlog("Debug: %d", DebugActiveProcess(GetProcessId(WoW::handle)));

		// Avoid killing app on exit
		DebugSetProcessKillOnExit(false);

		// get thread ID of the main thread in process
		DWORD_PTR dwThreadID = GetProcessThreadID(GetProcessId(WoW::handle));

		// gain access to the thread
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadID);

		CONTEXT Context; 
		Context.ContextFlags = CONTEXT_DEBUG_REGISTERS; 
		GetThreadContext(hThread, &Context); 

		// Dr0 -> Dr3 are usable debug registers. 
		Context.Dr0 = 0x0060EC45; 
		Context.Dr7 |= 1; 

		SetThreadContext(hThread, &Context);

		DEBUG_EVENT debugEv;
		BOOL handled = false;

		for (;;)
		{
			WaitForDebugEvent(&debugEv, INFINITE);

			switch (debugEv.dwDebugEventCode)
			{
			case EXCEPTION_DEBUG_EVENT:
				switch (debugEv.u.Exception.ExceptionRecord.ExceptionCode)
				{
					case EXCEPTION_SINGLE_STEP:
						if (debugEv.u.Exception.ExceptionRecord.ExceptionAddress == (LPVOID)address)
						{
							Context.ContextFlags = CONTEXT_FULL; 
							GetThreadContext(hThread, &Context); 
							vlog("Eax: 0x%x", Context.Eax);
							
							//SetThreadContext(hThread, &Context); 
						}
						
						ContinueDebugEvent(debugEv.dwProcessId, debugEv.dwThreadId, DBG_EXCEPTION_NOT_HANDLED); 
						break;
				}

				break;
			  default: 
				 ContinueDebugEvent(debugEv.dwProcessId, debugEv.dwThreadId, DBG_EXCEPTION_NOT_HANDLED); 
			}
		}

		DebugActiveProcessStop(GetProcessId(WoW::handle));
		return 0;
	}

	CObject Items(CObject *obj)
	{
		static int count = 0;

		if (obj->type() == OT_ITEM)
		{
			vlog("Item name: %s", GetObjectName(obj->base()));
			obj->setReturnBytes(); // stop callback
		}

		return *obj;
	}
	
	//
	CObject IteratePlayersGuildSignature(CObject *CurrentObject)
	{
		if (CurrentObject->isPlayer() && !CompareWGUIDS(LocalPlayer.guid(), CurrentObject->guid()))
		{
			CurrentObject->update(BaseObjectInfo | UnitFieldInfo | LocationInfo);
			CurrentObject->target(); 

			Lua::DoString("local g = GetGuildInfo(\"target\") if g == nil then PetitionFrameRequestButton:Click() end");
		}

		return *CurrentObject;
	}

	

		/*carpe.update();
		Sleep(500); // Wow's roster update is slow.

		vlog("%d / %d members online", carpe.online, carpe.total);
		for (int x = 0; x < carpe.online; x++)
		{
			member.entryIndex = x; // set entry index to the current iterate.
			member.updateName(); // update the name of member at entry x 
			
			vlog("Member %d: %s", x+1, member.name.c_str());
		}*/

		//ObjectListCallback(IteratePlayersGuildSignature);
		/*
		vlog(
	"function DString(name, code, target) "
	   "DSend(name .. ' = \\'' .. ('%%q'):format(code) .. '\\'', target) "
	"end ");*/

		/*vlog("Bits: %d", toBin(toggle));
		vlog("Map: %d", ToggleHasFlag(TOGGLE_NEWMAP));
		vlog("Parse: %d", ToggleHasFlag(TOGGLE_PARSE));

		ToggleOff(TOGGLE_NEWMAP);
		toggle = GetToggleValue();
		vlog("Bits: %d", toBin(toggle));
		vlog("Map: %d", ToggleHasFlag(TOGGLE_NEWMAP));
		vlog("Parse: %d", ToggleHasFlag(TOGGLE_PARSE));*/
		//ObjectListCallback(IteratePlayersGuildSignature);
		//ObjectListCallback(Items);
		//Ayys();
		/*	unit.setBase(0x1EB7C6B0);
			unit.update(NameInfo);

			DumpBaseObject(unit.object());*/
		/*}
		else
		{
			log("Invalid unit");
		}*/



// Random old tests:

		//ObjectListCallback(IteratePlayersGuildSignature); 

		// For ulduar camera scene{}
		//ClickToMove(1845.514, 119.561, 340.0);
		/*GetChatMessage(GetCurrentChatMessagePointer(), message, 256, name, 256);
		vlog("[%s]: %s", name, message);	

		HandleCommandInput(message);*/
		// For dal sewer jump.
		/*ClickToMove(5666.26, 896.30, 597.55);
		vlog("GameState: %d", IsLoginScreen());*/

		/*WOWPOS list[] = {{-3479.30, -14102.78, 2.15},
		{-3482.12, -14103.08, 2.50},
		{-3485.34, -14101.80, 2.79},
		{-3487.70, -14098.93, 3.10},
		{-3488.07, -14094.79, 3.46},
		{-3485.92, -14090.98, 3.71},
		{-3482.54, -14089.48, 4.11},
		{-3479.49, -14089.90, 4.43},
		{-3476.34, -14092.01, 4.76},
		{-3475.02, -14095.99, 5.14},
		{-3476.26, -14099.75, 5.51},
		{-3478.57, -14101.57, 5.80},
		{-3481.84, -14101.99, 6.11},
		{-3484.99, -14100.49, 6.47},
		{-3486.69, -14097.31, 6.86},
		{-3486.02, -14094.08, 7.22},
		{-3484.08, -14092.05, 7.55},
		{-3481.56, -14091.53, 7.86},
		{-3479.13, -14092.43, 8.21},
		{-3477.41, -14094.77, 8.60},
		{-3477.30, -14096.69, 8.81},
		{-3478.47, -14098.68, 9.02},
		{-3480.53, -14099.76, 9.56},
		{-3483.54, -14098.84, 10.05},
		{-3484.45, -14096.62, 10.50},
		{-3483.67, -14094.32, 10.97},
		{-3481.68, -14093.51, 11.43},
		{-3479.83, -14094.31, 11.87},
		{-3478.98, -14096.10, 12.39},
		{-3479.84, -14097.72, 12.86},
		{-3481.18, -14098.18, 13.19},
		{-3482.85, -14097.65, 13.71},
		{-3483.17, -14095.45, 14.39},
		{-3481.55, -14094.42, 15.03},
		{-3480.14, -14094.98, 15.55},
		{-3479.85, -14096.81, 15.93},

		};

		WOWPOS lists[] = {
			{-7398.57, 285.61, 59.46},
			{-7396.71, 284.31, 59.46},
			{-7395.26, 283.29, 59.46},
			{-7395.01, 282.36, 59.46},
			{-7394.79, 281.11, 59.46},
			{-7395.97, 280.83, 59.46},
			{-7397.08, 280.63, 59.46},
			{-7397.89, 280.31, 59.46},
			{-7398.69, 280.87, 59.46},
			{-7399.73, 281.60, 59.46}

		};

		FLOAT rot = 5.671991;
		int x = 0; 
		WOWPOS world, map;*/


		//JumpAroundGoldShire();
		// /script DEFAULT_CHAT_FRAME:AddMessage("\124cffffffff\124Hachievement:1021:&quot;..UnitGUID(&quot;player&quot;)..&quot;:0:0:0:0:0:0:0:0\124h[A Gift for the Tree of Adidi]\124h\124r");
		// can change name to anything

		// map to world for zone = x
		// change zone = y
		// world to map for zone = y.

		/*Lua_GetText("dMapX", buff);
		map.X = atof(buff);

		Lua_GetText("dMapY", buff);
		map.Y = atof(buff);

		Lua_GetText("dMapZ", buff);
		world = ConvertMapToWorld(ConvertGameToDBC(atoi(buff), LocalPlayer.MapID), map);
		sprintf_s(szBuffer, "Z: %d, DBC: %d", atoi(buff), ConvertGameToDBC(atoi(buff), LocalPlayer.MapID));
		log(szBuffer);

		ConvertWorldToMap(ConvertGameToDBC(atoi(buff), 1), world);*/

		/*while (x < sizeof(lists) / sizeof(WOWPOS))
		{
			if (GetDistance(lists[x]) > 1 && (GetCTMState() == CTM_Stopped))
			{
				SetPlayerRotation(rot);
				Sleep(500);
				Lua_DoString("ActionButton1:Click()");
				Sleep(1000);
				SendKey('W', WM_KEYDOWN);
				Sleep(100);
				SendKey('W', WM_KEYUP);
				Teleport(lists[x]);
				x++;
			}
		}*/

		// Torches stairs
		/*int x = 0;
		while (x < sizeof(list) / sizeof(WOWPOS))
		{
			Sleep(100);
			ClickToMove(list[x]);
			if (GetDistance(list[x]) < 1)
				x++;
		}

		log("done");*/

		//UnloadMap(81);

		/*target = FindCat();

		GetUnitName(target.BaseAddress, name);
		if (ValidObject(target))
		{
			sprintf_s(szBuffer, "Cat %s found.", name);
			AddChatMessage(szBuffer, "DD", RaidWarningMessage);
			AddChatMessage(szBuffer, "DD", RedWarningMessage);
			log(szBuffer);

			TargetObject(target.GUID);
		}
		else
		{
			log("No cats found.");
		}*/

		//ObjectListCallback(IteratePlayersGuildSignature);
		//ObjectListCallback(TEP_Callback);

	/*
	VOID FarmBot()
	{

		CHAR szBuffer[2543];

		Object enemy;

		WOWPOS result; 
	
		FLOAT dif;
	
		DWORD state = DG_PREFIGHT;
		BOOL ranged = false;
		BOOL speller = true;

		//enemy = ObjectListCallback(PlayerTargetted_Callback);
		enemy = GetCurrentTarget(LocationInfo | UnitFieldInfo);

		do
		{
			switch (state)
			{
			case DG_PREFIGHT:
				state = DG_FIGHTMOVE;
				break;
			
			case DG_FIGHTMOVE:
				if (GetCTMState() != CTM_Moving)
				{
					dif = GetRotationToFace(enemy.Pos) - LocalPlayer.Rotation;
	
					while (dif < - 2 * PI) dif += 2 * PI;
					while (dif > 2 * PI) dif -= 2 * PI;

					dif = fabs(dif);
			
					if (dif > 0.3)
						SetPlayerRotation(GetRotationToFace(enemy.Pos));
				}

				if (speller)
				{
					if (GetCTMAction() == CTM_Moving && GetDistance(enemy.Pos) < 30)
					{
						log("We are moving and close to the enemy");
					}
				}

				if (GetCTMState() != CTM_Moving)
				{
					if (!ranged)
					{
						ClickToMove(enemy.Pos);
					}
				}

				state = DG_FIGHT;
				break;

			case DG_FIGHT:
				sprintf_s(szBuffer, "GUID: 0x%x", enemy.GUID.low);
				log(szBuffer);

				TargetObject(enemy.GUID);
				//Interact(enemy.BaseAddress);
				if (Traceline(LocalPlayer.Pos, enemy.Pos, &result, HitTestLOS) && GetCTMAction() != CTM_Moving && GetDistance(enemy.Pos) > 1.4)
				{
					log("Not moving and far away");
					if (ranged)
						ClickToMove(enemy.Pos, enemy.GUID, CTM_AttackGuid);
				}
			
				if (Traceline(LocalPlayer.Pos, enemy.Pos, &result, HitTestLOS))
					CastSpells();

				state = DG_FIGHTMOVE;
				break;

			case DG_LOOT:
				break;

			}

			Sleep(100);
			enemy = DGetObjectEx(enemy.BaseAddress, LocationInfo | UnitFieldInfo);

		} while (ValidObject(enemy) && ValidCoord(enemy.Pos) && enemy.UnitField.Health > 0);
	}*/
	/*
	BOOL CALLBACK EnumResourceNameCallback(HMODULE hModule, LPCTSTR lpType,
		LPSTR lpName, LONG_PTR lParam)
	{
		HRSRC hResInfo = FindResourceA(hModule, lpName, lpType);
		DWORD cbResource = SizeofResource(hModule, hResInfo);

		HGLOBAL hResData = LoadResource(hModule, hResInfo);
		const BYTE *pResource = (const BYTE *)LockResource(hResData);

		CHAR filename[MAX_PATH];
		if (IS_INTRESOURCE(lpName))
			sprintf_s(filename, "#%d.manifest", lpName);
		else
			sprintf_s(filename, "%s.manifest", lpName);

		FILE *f = fopen(filename, "wb");

		if (f)
		{
			fwrite(pResource, cbResource, 1, f);
			fclose(f);
		}

		UnlockResource(hResData);
		FreeResource(hResData);

		return TRUE;   // Keep going
	}*/

	// Pathfinding:
	// Classes: Map, Node, 

	
};
/*
		Logging in:
WoWLua("AccountLoginAccountEdit:SetText('" + MyUsername + "')");
WoWLua("AccountLoginPasswordEdit:SetText('" + MyPassword + "')");
WoWLua("AccountLogin_Login()");

Selecting a realm:
(This will automatically select the first realm it finds on which you have a character, you will need to tweak this if you use multiple realms and need different behaviour.)
WoWLua(
"for i = 1, select(\"#\", GetRealmCategories()), 1 do "
" local numRealms = GetNumRealms(i); "
" for j = 1, numRealms, 1 do "
" local _, numCharacters = GetRealmInfo(i, j); "
" if (numCharacters > 0) then "
" ChangeRealm(i, j); "
" end "
" end "
"end ");

Selecting a character:
WoWLua("CharacterSelect_SelectCharacter(" + CharId + ", 0)");

Entering the world:
WoWLua("CharacterSelect_EnterWorld()");
 public static class LoginHelpers
    {
        public static bool RealmFrameVisible { get { return Lua.GetReturnVal<bool>("RealmList:IsShown()", 0); } }
        public static bool CharSelectVisible { get { return Lua.GetReturnVal<bool>("CharacterSelectUI:IsShown()", 0); } }

        public static void Login(string account, string password)
        {
            Lua.DoString(string.Format("DefaultServerLogin('{0}', '{1}')", account, password));
        }

        public static void SelectRealm(string name)
        {
            if (RealmFrameVisible)
            {
                Lua.DoString(
                    string.Format(
                        "for i = 1, select('#', GetRealmCategories()), 1 do local numRealms = GetNumRealms(i);" +
                        "for j = 1, numRealms, 1 do local name, numCharacters = GetRealmInfo(i, j);" +
                        "if (name ~= nil and name == '{0}')ChangeRealm(i,j);end end end",
                        name));
            }
        }

        public static void SelectCharacter(string name)
        {
            if (CharSelectVisible)
            {
                Lua.DoString(
                    string.Format(
                        "for i=0,GetNumCharacters(),1 do local name = GetCharacterInfo(i);" +
                        "if (name ~= nil and name == '{0}')CharacterSelect_SelectCharacter(i);end end",
                        name));
            }
        }

        public static void EnterWorld()
        {
            Lua.DoString("EnterWorld()");
        }
    }

		*/
		

		// DO NOT remove. Or maybe do remove.
		//ExitCurrentThread();*/