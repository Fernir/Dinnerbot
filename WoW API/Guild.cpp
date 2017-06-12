#include "Guild.h"

#include "..\Common\Common.h"
#include "Lua.h"

// Updates the roster in-game.
bool Guild::UpdateGuildRoster()
{
	DWORD  functionAddress = Offsets::sub_GuildRoster;
	DWORD  subAddress = NULL;
	DWORD  size = NULL;

	HANDLE thread = NULL;

	//if (!IsIngame() || dwAddress == NULL) return FALSE;
	BYTE Sub[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of function code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress		
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	size = sizeof(Sub);
	subAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	memcpy(&Sub[24], &functionAddress, sizeof(functionAddress));
	if (subAddress == NULL)
		return FALSE;

	if (!wpm(subAddress, &Sub, sizeof(Sub)))
		return FALSE;

	thread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)subAddress, NULL, NULL, NULL);
	if (thread == NULL)
		return FALSE;

	WaitForSingleObject(thread, 1000);
	CloseHandle(thread);

	return VirtualFreeEx(WoW::handle, (LPVOID)subAddress, size, MEM_DECOMMIT);
}

int Guild::getNumGuildMembers()
{
	int num = 0;

	rpm(Offsets::numGuildMembers, &num);
	return num;
}

int Guild::getNumOnlineGuildMembers()
{
	int num = 0;

	rpm(Offsets::numOnlineGuildMembers, &num);
	return num;
}

// Requires ingame roster update.
void Guild::updateInfo()
{
	total = getNumGuildMembers();
	online = getNumOnlineGuildMembers();
}

// Requires update info
void Guild::buildRoster()
{
	roster.clear();
	for (int x = 0; x < online; x++)
		addEntry(x);
}

// Requires update info
bool Guild::CompareIngameRoster()
{
	GuildEntry buffer(-1);

	// If our roster is a different size than the ingame one, then it is outdated.
	if (roster.size() != online) 
		return false;

	for (int x = 0; x < roster.size(); x++)
	{
		buffer.entryIndex = x;
		buffer.updateZone();
		buffer.updateName();

		// Hide Pont from the roster.
		if (!_stricmp(buffer.name.c_str(), "pont")) continue;

		// If we encounter a different guild member name, our roster is outdated.
		if (_stricmp(roster[x].name.c_str(), buffer.name.c_str())) 
			return false;

		// If there is a zone difference between a guild member's current zone and their old zone, then our roster must be outdated.
		if (_stricmp(roster[x].zone.c_str(), buffer.zone.c_str())) 
			return false;
	}

	return true;
}

void Guild::addEntry(int index)
{
	GuildEntry entry(index);
	if (index >= online) return;

	entry.updateZone();
	entry.updateName();
	entry.updateClass();
	roster.push_back(entry);
}

// Unfinished.
void Guild::addMember(GuildEntry member)
{
	char buffer[100];

	sprintf_s(buffer, "GuildInvite(\"%s\")", member.name.c_str());
	Lua::DoString(buffer);
}

Guild::Guild(string guildName)
{
	total = 0;
	online = 0;
	this->name = guildName;
	roster.reserve(10);
}

void Guild::clear()
{
	roster.clear();
	online = 0;
	total = 0;
}

Guild::~Guild(void)
{
	roster.clear();
}
