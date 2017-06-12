#include "CGLoot.h"
#include "Lua.h"

#include "Game\Game.h"
#include "Main\Constants.h"
#include "Memory\Memory.h"

#include <Utilities\Wait.h>
#include <Utilities\Utilities.h>

WGUID Loot::getCurrentObject()
{
	WGUID guid;

	memset(&guid, 0, sizeof(guid));
	rpm(Offsets::ObjectBeingLooted, &guid, sizeof(guid));
	return guid;
}

bool Loot::isWindowOpen()
{
	if (ValidGUID(Loot::getCurrentObject()))
		return true;

	return false;
}

bool Loot::isWindowClosed()
{
	return !Loot::isWindowOpen();
}

std::string Loot::getSlotName(int slot)
{
	char buffer[256];

	Loot::getSlotName(slot, buffer, 256);
	return std::string(buffer);
}

bool Loot::getSlotName(int slot, CHAR *nameResult, size_t maxSize)
{
	DWORD buffer = NULL;
	DWORD totalSize = NULL;
	DWORD stubAddress = NULL;
	DWORD nameLocation = NULL;
	DWORD executeAddress = Offsets::CGLootWindow__GetLootSlotName;

	HANDLE remoteThread = NULL;

	CHAR szBuffer[256];

	if (!WoW::InGame()) return FALSE;
	//if (IsWardenLoaded()) return false;

	LocalPlayer.update(LocationInfo | UnitFieldInfo);
	if (!LocalPlayer.isValid()) AttachWoW();

	BYTE callFunction[] = {
		/* Update TLS */
		0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
		0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
		0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
		0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
		0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

		/** Call function segment. **/
		0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, executeAddress
		0x68, 0x00, 0x00, 0x00, 0x00,		// push slot - 1
		0x68, 0x00, 0x00, 0x00, 0x00,		// push maxSize
		0x68, 0x00, 0x00, 0x00, 0x00,		// push nameLocation
		0xFF, 0xD7,							// call edi
		0x83, 0xC4, 0x0C,					// add esp, 0C
		0xC3								// retn

	};

	totalSize = sizeof(callFunction)+maxSize;

	stubAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, totalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (stubAddress == NULL)
		return FALSE;

	nameLocation = stubAddress + sizeof(callFunction);

	slot--;
	memcpy(&callFunction[24], &executeAddress, sizeof(executeAddress));
	memcpy(&callFunction[29], &slot, sizeof(slot));
	memcpy(&callFunction[34], &maxSize, sizeof(maxSize));
	memcpy(&callFunction[39], &nameLocation, sizeof(nameLocation));

	if (!wpm(stubAddress, &callFunction, sizeof(callFunction)))
		return FALSE;

	SuspendWoW();
	remoteThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)stubAddress, NULL, NULL, NULL);
	if (remoteThread == NULL)
		return FALSE;

	WaitForSingleObject(remoteThread, 1000);
	CloseHandle(remoteThread);

	Memory::Read(nameLocation, &szBuffer, 256);
	strncpy(nameResult, szBuffer, strlen(szBuffer) + 1);

	VirtualFreeEx(WoW::handle, (LPVOID)stubAddress, totalSize, MEM_RELEASE);
	ResumeWoW();
	return TRUE;
}

bool Loot::close()
{
	/*DWORD buffer = NULL;
	DWORD totalSize = NULL;
	DWORD stubAddress = NULL;
	DWORD scriptLocation = NULL;
	DWORD CloseLootAddress = CGLootWindow__CloseLoot;

	HANDLE remoteThread;

	if (!IsIngame()) return FALSE;
	if (IsWardenLoaded()) return false;
	if (!RefreshLocalPlayerStruct()) AttachWoW();

	BYTE callFunction[] = {
	/* Update TLS *//*
	0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]
	0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
	0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
	0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
	0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

	/* Call function segment. *//*
	0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, executeAddress
	0x6A, 0x00,							// push 00
	0x6A, 0x00,							// push 00
	0x6A, 0x00,							// push 00
	0xFF, 0xD7,							// call edi
	0x83, 0xC4, 0x0C,					// add esp, 0C
	0xC3								// retn

	};

	totalSize = sizeof(callFunction);

	stubAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, totalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (stubAddress == NULL)
	return FALSE;

	scriptLocation = stubAddress + sizeof(callFunction);

	memcpy(&callFunction[24], &CloseLootAddress, sizeof(CloseLootAddress));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(stubAddress), &callFunction, sizeof(callFunction), 0)))
	return FALSE;

	SuspendWoW();
	remoteThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)stubAddress, NULL, NULL, NULL);
	if (remoteThread == NULL)
	return FALSE;

	WaitForSingleObject(remoteThread, 1000);
	CloseHandle(remoteThread);

	VirtualFreeEx(WoW::handle, (LPVOID)stubAddress, totalSize, MEM_DECOMMIT);
	ResumeWoW();
	return TRUE;*/

	return Lua::DoString("CloseLoot(0, 0, 0)");
}

bool Loot::isSlotCoin(int slot)
{
	DWORD buffer;
	DWORD guidAddress = Offsets::LootWindow;
	DWORD unknownAddress1 = 0xBFA8D0;

	WGUID guid;


	rpm(unknownAddress1, &buffer, sizeof(buffer));
	rpm(Offsets::ObjectBeingLooted, &guid, sizeof(guid));

	if (guid.low && buffer > 0 && (unsigned int)(signed __int64)slot == 1)
		return TRUE;

	return FALSE;
}

int Loot::local()
{
	CObjectManager<CUnit> manager;

	std::vector<CUnit> lootTable;

	const float lootDistance = 7;

	// Get nearby lootable units
	for (auto &obj : manager)
	{
		if (!obj.isLocalPlayer())
		{
			obj.update(LocationInfo);
			if (obj.distance() < lootDistance)
			{
				obj.update(UnitFieldInfo);

				if (obj.isDead() && obj.hasLoot())
					lootTable.push_back(obj);
			}
		}
	}

	// Go through each unit and loot it
	for (auto &unit : lootTable)
	{
		unit.interact();

		// While loot window is closed, wait.
		Utilities::Wait lootWindow(Loot::isWindowClosed, 50, 500);
		if (lootWindow.wait())
		{
			Sleep(200);
			Loot::close();
		}
	}

	return true;
}

bool Loot::slots(std::vector<int> &vec)
{
	std::string build = "";

	if (vec.size() == 0) return true;
	for (const auto &slot : vec)
	{
		build += "LootSlot(";
		build += std::to_string(slot);
		build += ") ";
	}

	return Lua::DoString(build);
}

bool Loot::all()
{
	return Lua::vDo("for i = 1, %d do LootSlot(i) end", Loot::getItemCount());
}

bool Loot::slot(int slot)
{
	CHAR szBuffer[256];

	sprintf_s(szBuffer, "LootSlot(%d)", slot);
	return Lua::DoString(szBuffer);
}

UINT Loot::getItemCount()
{
	DWORD unknownAddress2 = 0xBFA8D0;
	DWORD unknownAddress = 0xBFA6B4;
	DWORD buffer = NULL;

	WGUID guid;

	DWORD result = NULL; // eax@1
	signed int v1 = 0; // ecx@2
	DWORD v2 = NULL; // edx@2
	signed int v3 = 0; // esi@2

	memset(&guid, 0, sizeof(guid));
	rpm(Offsets::ObjectBeingLooted, &guid, sizeof(guid));
	result = guid.high | guid.low;

	if (guid.low)
	{
		result = 0;
		v1 = 2;
		v2 = 0xBFA6B4;
		v3 = 3;
		do
		{
			// if ( *(_DWORD *)(v2 - 32) > 0 )
			//    result = v1 - 1;
			rpm(v2 - 32, &buffer, sizeof(buffer));
			if (buffer > 0)
				result = v1 - 1;

			// if ( *(_DWORD *)v2 > 0 )
			//     result = v1;
			rpm(v2, &buffer, sizeof(buffer));
			if (buffer > 0)
				result = v1;


			// if ( *(_DWORD *)(v2 + 32) > 0 )
			//     result = v1 + 1;
			rpm(v2 + 32, &buffer, sizeof(buffer));
			if (buffer > 0)
				result = v1 + 1;

			// if ( *(_DWORD *)(v2 + 64) > 0 )
			//     result = v1 + 2;
			rpm(v2 + 64, &buffer, sizeof(buffer));
			if (buffer > 0)
				result = v1 + 2;

			// if ( *(_DWORD *)(v2 + 96) > 0 )
			//     result = v1 + 3;
			rpm(v2 + 96, &buffer, sizeof(buffer));
			if (buffer > 0)
				result = v1 + 3;

			// if ( *(_DWORD *)(v2 + 128) > 0 )
			//     result = v1 + 4;
			rpm(v2 + 128, &buffer, sizeof(buffer));
			if (buffer > 0)
				result = v1 + 4;

			v1 += 6;
			v2 += 192;
			--v3;
		} while (v3);

		rpm(unknownAddress2, &buffer, sizeof(buffer));
		if (buffer)
			++result;
	}

	return result;
}