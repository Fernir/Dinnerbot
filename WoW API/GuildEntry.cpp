#include "GuildEntry.h"
#include "ClientDB.h"

#include <Game\Variable.h>

DWORD GuildEntry::GetRosterAddress(void)
{
	DWORD rosterPtr = NULL;

	rpm(Offsets::rosterList, &rosterPtr, sizeof(DWORD));
	return rosterPtr;
}

DWORD GuildEntry::getEntryAddress()
{
	DWORD entryAddress = NULL;

	if (entryIndex == -1) return NULL;

	rpm(GetRosterAddress() + this->entryIndex * 4, &entryAddress);
	return entryAddress;
}

void GuildEntry::updateZone(void)
{
	ClientDB areaTable(Offsets::AreaTableDB);

	if (this->entryIndex == -1) return; // error
	
	DWORD zoneAddress;
	DWORD zonePtr;

	DWORD recordId;
	DWORD row;

	char zoneText[52];

	WoW::Variable<DWORD> rowPtr;

	rpm(getEntryAddress() + 0x44, &recordId, sizeof(DWORD));
	row = areaTable.GetRow(recordId);

	zonePtr = row + 0x2C;
	rpm(zonePtr, &zoneAddress, sizeof(DWORD));
	
	rpm(zoneAddress, &zoneText, 52);
	zone = zoneText;
}

void GuildEntry::updateClass(void)
{
	ClientDB classes(Offsets::ChrClassesDB);

	DWORD recordId, buffer;
	DWORD entryPtr, row;

	char name[16];

	if (this->entryIndex == -1) return; // error
	
	// Returns the (entryIndex)th roster entry.
	// Then returns the DBC recordId associated with the class name;
	rpm(GetRosterAddress() + this->entryIndex * 4, &entryPtr, sizeof(DWORD));
	rpm(entryPtr + 0x3C, &recordId, sizeof(DWORD));
	row = classes.GetRow(recordId);

	rpm(entryPtr + 0x40, &buffer, sizeof(DWORD));

	// Get entry pointer of class in ClientDB.
	entryPtr = classes.LookupRow(row, buffer, 0);
	rpm(entryPtr, &name, 16);

	className = name;
}

void GuildEntry::updateName(void)
{
	/*GuildRoster Player name: 
	eax = index;
	mov edx,[g_rosterNameList]
	mov esi,[edx+eax*4]
	lea eax,[esi+08]*/

	char buffer[16];

	DWORD esi, edx;
	DWORD namePtr;

	if (this->entryIndex == -1) return; // error

	edx = GetRosterAddress();

	// Returns pointer to roster entry.
	// The character name pointer is at entryAddress + 0x8.
	rpm(edx + this->entryIndex * 4, &esi, sizeof(DWORD));
	namePtr = esi + 8;

	rpm(namePtr, &buffer, 16); // Max character name length is 12, add 4 for safety (fear of the unknown).
	name = buffer;
}

GuildEntry::GuildEntry(std::string entryText)
{
	entryIndex = -1;
	rankIndex = 10;
	status = -1;
	level = 0;
	
	name = entryText;
	this->zone = "";
	this->className = "";
}

GuildEntry::GuildEntry(int entry)
{
	entryIndex = entry;
	rankIndex = 10;
	status = -1;
	level = 0;

	this->name = "";
	this->zone = "";
	this->className = "";
}

GuildEntry::~GuildEntry(void)
{
}
