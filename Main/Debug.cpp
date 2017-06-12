#include "Debug.h"

#include "WoW API\Chat.h"
#include "WoW API\CGLoot.h"
#include "WoW API\Warden.h"
#include "WoW API\Object Classes\Unit.h"
#include <WoW API\Object Classes\Item.h>

#include "Memory\Endscene.h"
#include "Geometry\Coordinate.h"
#include "Radar\Drawing.h"
#include "Bot\Grinder.h"
#include "Engine\Combat\Rotation.h"
#include "Engine\Navigation\ClickToMove.h"
#include "WoW API\CInputControl.h"
#include "Engine\Navigation\Pathfinding.h"
#include "Engine\Navigation\Walker.h"
#include "Radar\NetEditor.h"

#include <Geometry\ConvexCone.h>

#include "Game\Interactions.h"

#include <Game\Variable.h>
#include <Game\Pointer.h>
#include <WoW API\ClientDB.h>
#include <WoW API\Guild.h>
#include <WoW API\Lua.h>

#include <File\NetIO.h>
#include <File\FishingProfile.h>

#include <Bot\Fishing.h>
#include <Memory\Hacks.h>

#include <TlHelp32.h>

using namespace std;
using namespace Memory;
using namespace Engine;
using namespace Geometry;

namespace Debug
{

	VOID te()
	{
		DWORD  address = Offsets::CGUnit_C__TryChangeStandState;
		DWORD  dwSubroutineAddress = NULL;
		DWORD  dwTotalSize = NULL;
		HANDLE hThread = NULL;

		if (!WoW::InGame())
			return;

		BYTE Sub_CallFunction[] = {
			// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

			// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, address
			0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ECX, playerBase
			0x6A, 0x00,							// PUSH 01
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
		};

		DWORD base = LocalPlayer.base();

		dwTotalSize = sizeof(Sub_CallFunction);
		dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		memcpy(&Sub_CallFunction[24], &address, sizeof(address));
		memcpy(&Sub_CallFunction[29], &base, sizeof(base));

		if (dwSubroutineAddress == NULL)
			return;

		if (!wpm(dwSubroutineAddress, &Sub_CallFunction, sizeof(Sub_CallFunction)))
			return;

		hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
		if (hThread == NULL)
			return;

		WaitForSingleObject(hThread, 1000);
		CloseHandle(hThread);

		vlog("TryChangeStandState");
		VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	}

	VOID Test()
	{
		Thread *thread = Thread::GetCurrent();

		if (!thread) return;
		if (ReleaseMode)
		{
#ifndef RELEASE
			thread->exit("Debug::Test");
#else
			thread->exit();
#endif
			return;
		}
		
		HMODULE d3d9 = LoadLibraryEx("D3D9.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
		DWORD endscene = (DWORD)GetProcAddress(d3d9, "Direct3DCreate9Ex") + 0x13BDE;
		//vlog("Address: 0x%x", endscene);

		if (Endscene.IsHooked() && !Endscene.CanExecute())
		{
			//wpm(Endscene.GetPtrAddress(), &endscene, sizeof(endscene));
			Endscene.Fix();
		}
		else
		{
			//NetEditor::Undo();
		}

		CObjectManager<CItem> manager;

		for (auto &item : manager)
		{
			vlog("Item Address: 0x%x", item.base());
			vlog("Item Field: 0x%x", item.fieldBase());
			vlog("Item Id: %d", item.textId());
		}

		// Grinder tests
		if (0)
		{
			// Set combat distance to be 5
			static Engine::Grinder grinder(5);

			grinder.setGCD(100);
			grinder.addLoot("Chunk of Boar Meat");
			grinder.addLoot(Grinder::LootType::Gold);

			// Boar faction
			grinder.addTarget(44);
			grinder.addTarget(22);
			grinder.addTarget(189);

			// Sinister Strike
			// Energy >= 45
			Engine::Condition ss_energy(Engine::CONDITION_POWER);
			ss_energy.addOption(Engine::OPTION_VALUE_BY_NUMBER, 44);
			ss_energy.addOption(Engine::OPTION_CHECK_PLAYER);
			ss_energy.addOption(Engine::OPTION_ORDER_MORE);
			ss_energy.addOption(Engine::OPTION_TYPE_POWER);

			Engine::Condition distance(Engine::CONDITION_DISTANCE_TO_TARGET);
			distance.addOption(Engine::OPTION_VALUE_BY_NUMBER, 5);
			distance.addOption(Engine::OPTION_CHECK_PLAYER);
			distance.addOption(Engine::OPTION_ORDER_LESS);

			Engine::Action ss("Sinister Strike", Action::SpellName);
			ss.addCondition(ss_energy);
			ss.addCondition(distance);

			// Eviscerate
			// Combo points >= 2
			Engine::Condition cbs(Engine::CONDITION_COMBO_POINTS);
			cbs.addOption(Engine::OPTION_VALUE_BY_NUMBER, 1);
			cbs.addOption(Engine::OPTION_ORDER_MORE);

			Engine::Condition evis_energy(Engine::CONDITION_POWER);
			evis_energy.addOption(Engine::OPTION_VALUE_BY_NUMBER, 34);
			evis_energy.addOption(Engine::OPTION_CHECK_PLAYER);
			evis_energy.addOption(Engine::OPTION_ORDER_MORE);
			evis_energy.addOption(Engine::OPTION_TYPE_POWER);

			Engine::Action evis("Eviscerate", Action::SpellName);
			evis.addCondition(evis_energy);
			evis.addCondition(distance);
			evis.addCondition(cbs);

			grinder.combatRotation.addAction(evis);
			grinder.combatRotation.addAction(ss);
			//CastSpellByID(2479);

			if (WoW::InGame())
			{
				NetEditor::LoadNet("loch10-16.xml");

				if (NetEditor::net.order() > 0)
				{
					//Navigation::setTolerance(0.01);
					grinder.setNet(&NetEditor::net);

					while (grinder.fsm())
					{
						Sleep(1000);
					}
				}
			}


			/*Guild g("danks");

			g.updateInfo();
			g.buildRoster();

			GuildEntry *member = NULL;

			int online = g.getNumOnlineGuildMembers();
			for (int x = 0; x < online; x++)
			{
				member = &g.roster[x];
				if (!member) continue;

				vlog("Guild member %d: %s %s %s", x, member->name.c_str(), member->zone.c_str(), member->className.c_str());
			}*/

			//DWORD movementPtr = LocalPlayer.getCMovementPtr();
			// CMovement::OnMoveStop(movementPtr, tickCount, someChar);

			//Engine::Navigation::face(4307.93, -3225.03, 308.35);
			//CInputControl::updatePlayer();
			//ChangeStandState(LocalPlayer.base());

			//te();

			if (WoW::InGame())
			{
				// Start radar
				//Radar::Create();
			}
			else
			{

			}
		}

#ifndef RELEASE
		thread->exit("Debug::Test");
#else
		thread->exit();
#endif
		return;
	}

	VOID DumpAuras(Object &Unit)
	{
		int auraCount = GetAuraCount(Unit); 
		int currentAuraCount = 0;

		DWORD dwCurrentAuraOffset = 0; 
		DWORD dwCurrentAuraAddress = 0;
		DWORD dwBuffer = NULL;
		DWORD dwBuffer1 = NULL;

		CHAR szBuffer[256];
		CHAR szName[64];

		DGetObjectName(Unit, Unit.Name);
		sprintf_s(szBuffer, "Auras active on %s:", Unit.Name);
		AddChatMessage(szBuffer, "DDebug", TurqoiseMessage);
		log(szBuffer);

		if (auraCount)
		{
			while (currentAuraCount <= auraCount)
			{
				rpm(Unit.BaseAddress + 884, &dwBuffer, sizeof(dwBuffer));
				rpm(Unit.BaseAddress + 790, &dwBuffer1, sizeof(dwBuffer1));
				dwCurrentAuraAddress = (dwBuffer == -1)?(dwCurrentAuraOffset + dwBuffer1):(Unit.BaseAddress + dwCurrentAuraOffset + 3152); 

				rpm(dwCurrentAuraAddress + 8, &dwBuffer1, sizeof(dwBuffer1));

				dwCurrentAuraOffset += 24;
				if (dwBuffer1 == 0 || dwBuffer1 > 65535) 
				{
					currentAuraCount++;
					continue;
				}

				memset(szName, 0, 64);
				GetAuraName(dwBuffer1, szName);
				sprintf_s(szBuffer, "Aura[%d]: %d - %s", currentAuraCount, dwBuffer1, szName);
				AddChatMessage(szBuffer, "DDebug", TurqoiseMessage);

				LogAppend(szBuffer);
				currentAuraCount++;
			}
		}
}

	VOID DumpUnitField(const DUnitField &UnitField)
	{
		if (!WoW::InGame()) return;
		vlog("**************Unit Field Dump**************\n");
		vlog("Unit Field Struct Address: 0x%p\n", UnitField.BaseAddress);
		vlog("Attack Power: %d\n", UnitField.Attack_Power);
		vlog("Channel Object: 0x%p\n", UnitField.ChannelObject);
		vlog("Created By: 0x%lx\n", UnitField.CreatedBy);
		vlog("Display ID: %d\n", UnitField.DisplayId);
		vlog("Mount Display ID: %d\n", UnitField.MountDisplayId);
		vlog("Combat Reach: %f\n", UnitField.CombatReach);
		vlog("Bounding radius: %f\n", UnitField.Bounding_Radius);
		vlog("Level: %d\n", UnitField.Level);
		vlog("Faction: %d\n", UnitField.Faction);
		vlog("Channel SpellID: %d\n", UnitField.ChannelSpell);
		vlog("Aura State: 0x%x\n", UnitField.Aura_State);
		//vlog("Casting SpellID: %d\n", UnitField.CastingSpell);
		vlog("Health: %d/%d\n", UnitField.Health, UnitField.Max_Health);
		vlog("PowerType: %d\n", UnitField.PowerType);
		vlog("Power: %d/%d\n", UnitField.Power, UnitField.Max_Power);
		vlog("Damage: %f-%f\n", UnitField.Min_Damage, UnitField.Max_Damage);
		vlog("Target: 0x%p\n", UnitField.Target);
		vlog("Unit Flags: 0x%p\n", UnitField.Unit_Flags);
		vlog("Dynamic Unit Flags: 0x%p\n", UnitField.Unit_Dynamic_Flags);
		vlog("Summoned By: 0x%p\n", UnitField.SummonedBy);
		vlog("Movement State: 0x%p\n", UnitField.MovementState);
		vlog("Current Speed: %0.2f\n", UnitField.CurrentSpeed);
		vlog("Active Speed: %0.2f\n", UnitField.ActiveSpeed);
		vlog("Walk Speed: %0.2f\n", UnitField.WalkSpeed);
		vlog("Swim Speed: %0.2f\n", UnitField.SwimSpeed);
		vlog("Flight Speed: %0.2f\n", UnitField.FlightSpeed);
		vlog("*********************************************\n");
	}

	VOID DumpGameObjectField(const DGameObjectField &GameObject)
	{
		CHAR szBuffer[216];
	
		if (!WoW::InGame()) return;
		vlog("**************GameObject Field Dump**************\n");
		LogAppend(szBuffer);
		vlog("GameObject Field Struct: 0x%p\n", GameObject.BaseAddress);
		LogAppend(szBuffer);
		vlog("Created By: 0x%p\n", GameObject.CreatedBy);
		LogAppend(szBuffer);
		vlog("Display ID: 0x%p\n", GameObject.DisplayId);
		LogAppend(szBuffer);
		vlog("Flags: 0x%p\n", GameObject.Flags);
		LogAppend(szBuffer);
		sprintf(szBuffer,"********************************************************\n");
		LogAppend(szBuffer);
	}

	VOID DumpLocation(const Object &Dump)
	{
		CHAR szBuffer[216];
	
		if (!WoW::InGame()) return;
		vlog("**************Object Location Dump**************\n");
		vlog("Object X: %f", Dump.X);
		vlog("Object Y: %f", Dump.Y);
		vlog("Object Z: %f", Dump.Z);
		vlog("Waypoint: {%0.2f, %0.2f, %0.2f}", Dump.X, Dump.Y, Dump.Z);
		vlog("Object Rotation: %f", Dump.Rotation);
		vlog("MapID, ZoneID: (%d, %d)", Dump.MapID, Dump.ZoneID);
		vlog("Distance to player: %f", GetDistance(Dump));
		vlog("**************************************************\n");
	}

	VOID DumpBaseObject(const Object &Dump)
	{
		CHAR szBuffer[256];
	
		//Dump = DGetObjectEx(Dump.BaseAddress, BaseObjectInfo | NameInfo);
	
		if (!WoW::InGame()) return;
		sprintf(szBuffer, "**************Base Object Dump**************\n");
		LogAppend(szBuffer);
		sprintf(szBuffer, "Object Name: %s", Dump.Name);
		LogAppend(szBuffer);
		sprintf(szBuffer, "Object Base: 0x%p", Dump.BaseAddress);
		LogAppend(szBuffer);
		sprintf(szBuffer, "Object Type: %d", Dump.Type_ID);
		LogAppend(szBuffer);
		sprintf(szBuffer, "Object GUID: 0x%p%p", Dump.GUID.high, Dump.GUID.low);
		LogAppend(szBuffer);
		sprintf(szBuffer, "Next Object Pointer: 0x%p", Dump.Next_Ptr);
		LogAppend(szBuffer);
		sprintf(szBuffer, "**************************************************\n");
		LogAppend(szBuffer);
	}

	VOID DumpAddresses()
	{
		DWORD buffer = NULL;
		DWORD fBuffer = 0;

		vlog("******************Object Manager*****************\n");
		vlog("Client Connection: 0x%p\n", Offsets::ClientConnection);
		vlog("Current Manager: 0x%p\n", g_dwCurrentManager);
		vlog("*************************************************\n");
	
	
		/*vlog("********************Click To Move*****************\n");
	

		vlog("CTM_Base: 0x%x", CTM_Base);
	
	
		ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_X), &fBuffer, sizeof(fBuffer), NULL);
		vlog("CTM_X: %0.2f", fBuffer);
	
	
		ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_Y), &fBuffer, sizeof(fBuffer), NULL);
		vlog("CTM_Y: %0.2f", fBuffer);
	
	
		ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_Z), &fBuffer, sizeof(fBuffer), NULL);
		vlog("CTM_Z: %0.2f", fBuffer);
	
	
		ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_TurnSpeed), &fBuffer, sizeof(fBuffer), NULL);
		vlog("CTM_TurnSpeed: %0.2f", fBuffer);
	
	
		ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_Distance), &fBuffer, sizeof(fBuffer), NULL);
		vlog("CTM_Distance: %0.2f", fBuffer);
	
	
		ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_Action), &buffer, sizeof(buffer), NULL);
		vlog("CTM_Action: 0x%x", CTM_Base + CTM_Action);
	
	
		ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_GUID), &buffer, sizeof(buffer), NULL);
		vlog("CTM_GUID: 0x%x", CTM_Base + CTM_GUID);
	
	
		vlog("*************************************************\n");
		*/

		vlog("********************Globals*****************\n");
		vlog("WoW window handle: 0x%p",  g_hwWoW);
		vlog("WoW process handle: 0x%p", WoW::handle);
		vlog("WoW process id: %d",		 GetProcessId(WoW::handle));

		vlog("********************************************\n");
	

		vlog("********************WoW Globals*****************\n");
		vlog("Current Loot Object: 0x%x%x", Loot::getCurrentObject().high, Loot::getCurrentObject().low);
		vlog("Wow memory usage: %0.2f mb", (float)GetWoWMemoryUsage() / 1024 / 1024);
		vlog("************************************************\n");
	

		vlog("******************Player Related***********\n");
		vlog("Player Base: 0x%p",			  WoW::GetPlayerBase());
		vlog("Local Player Movement: 0x%p", LocalPlayer.base() + Offsets::MovementState);
		vlog("[Local Player Movement]: 0x%p", LocalPlayer.unitField().MovementState);
		vlog("Current Manager: 0x%p",		  g_dwCurrentManager);
		vlog("*******************************************\n");
	
		vlog("******************Anti-Afk*****************\n");
		vlog("LastHardwareAction: 0x%p", GetLastHardwareAction());
		buffer = (DWORD)GetTimestamp();
		fBuffer = (DWORD)OsGetAsyncTimeMs();

		vlog("Timestamp: %d",	GetTimestamp());
		vlog("Tickcount: %d",	fBuffer);
		vlog("Hangtime(): %d",	abs((INT)buffer - (INT)fBuffer));
		vlog("********************************************\n");
	

		vlog("***************Warden Anti-Cheat**********************\n");
		vlog("Warden Base: 0x%p",		 Warden::GetBase());
		vlog("Warden Scan Module: 0x%p", Warden::GetScanModule());
		vlog("Warden Class Ptr: 0x%p",	 Warden::GetClass());
		vlog("Is Warden Loaded: %s",	 (Warden::IsLoaded() > 0)? "Yes" : "No");
		vlog("*******************************************************\n");
	

		vlog("********************Threads*****************\n");
		vlog("Number of Threads: %d", Thread::Threads.size() + 1);
		vlog("Infoloop thread: 0x%x", Thread::FindType(Thread::eType::InfoLoop));
		vlog("********************************************\n");

		vlog("***********************EndScene Related**********************\n");
		vlog("Endscene address: 0x%x",			CEndscene::Get());
		vlog("Endscene ptr address: 0x%x",		CEndscene::GetPtrAddress());
		vlog("Module cache size: %d",			Memory::cache.size());
		vlog("Is EndScene hooked: %d",			Endscene.IsHooked());
		vlog("getFunctionSpace: 0x%x",			Endscene.getFunctionSpace());
		vlog("ReadFunctionAddress: 0x%x",		Endscene.ReadFunctionAddress());
		vlog("ReadFunctionReturn: 0x%x",		Endscene.ReadFunctionReturn());
		vlog("CanExecuteMainThread: %d",		Endscene.CanExecute());
		vlog("LastESTime: %d",					Endscene.getLastRuntime());
		vlog("************************************************************\n");
	}
};
