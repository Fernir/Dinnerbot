#include "Unit.h"
#include "Game\Game.h"
#include "Memory\Memory.h"
#include "Memory\Endscene.h"
#include "Utilities\Utilities.h"

#include <WoW API\Spell.h>

#include <WinInet.h>

using namespace Memory;

UINT GetAuraCount(Object Unit)
{
	UINT count = 0;

	ReadProcessMemory(WoW::handle, (LPVOID)(Unit.BaseAddress + 3536), &count, sizeof(count), NULL);
	if (count == -1)
		ReadProcessMemory(WoW::handle, (LPVOID)(Unit.BaseAddress + 3156), &count, sizeof(count), NULL);

  return count;
}

// 0C790000

bool ChangeStandState(DWORD baseAddress)
{
	DWORD executeAddress = Offsets::CGPlayer_C__TryChangeStandState;
	DWORD result = NULL;

	if (!WoW::InGame()) return false; 
	//if (Warden::IsLoaded()) return false;
	if (!Endscene.CanExecute() || !Endscene.IsHandlerStarted()) return false;

	// thiscall => pass this into ecx
	BYTE code[] = {
		/** Call function segment. **/
			0xBF, 0x00, 0x00, 0x00, 0x00,		// mov edi, executeAddress
			0xB9, 0x00, 0x00, 0x00, 0x00,		// mov ecx, baseAddress
			0x6A, 0x00,							// push 00
			0xFF, 0xD7,							// call edi
			0xC3								// retn
	};

	memcpy(&code[1], &executeAddress, sizeof(executeAddress));
	memcpy(&code[6], &baseAddress, sizeof(baseAddress));

	Stub stub(Endscene.getFunctionSpace(), code, sizeof(code));
	Endscene.ClearFunctionSpace();
	stub.inject();

	result = Endscene.Execute(stub); 
	return result != CEndscene::EXECUTE_ERROR;
}

bool HasAuraBySpellId(Object unit, INT spellId)
{
	int auraCount = GetAuraCount(unit); 
	int currentAuraCount = 0;

	DWORD currentAuraOffset = 0;
	DWORD currentAuraAddress = 0;
	DWORD buffer = NULL;
	DWORD buffer1 = NULL;

	if (auraCount)
	{
		while (currentAuraCount <= auraCount)
		{
			rpm(unit.BaseAddress + 884, &buffer, sizeof(buffer));
			rpm(unit.BaseAddress + 790, &buffer1, sizeof(buffer1));
			currentAuraAddress = (buffer == -1) ? (currentAuraOffset + buffer1) : (unit.BaseAddress + currentAuraOffset + 3152);
			rpm(currentAuraAddress + 8, &buffer1, sizeof(buffer1));
			
			currentAuraCount++;
			currentAuraOffset += 24;
			if (buffer1 == spellId)
				return TRUE;
		}
	}

	return FALSE;
}

bool GetAuraName(UINT nAuraID, CHAR *szAuraName)
{
	HINTERNET hSession;  
	HINTERNET hURL;
	DWORD dwBytesRead;

	CHAR szUrl[256];
	CHAR *szBuffer = (CHAR *)malloc(sizeof(CHAR) * 50000);

	memset(szBuffer, 0, 50000);
	sprintf_s(szUrl, "http://wotlk.openwow.com/spell=%d", nAuraID);
	
	do
	{
		hSession = InternetOpen("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession)
		{
			hURL = InternetOpenUrl(hSession, szUrl, NULL, 0, 0, 0);
			if (hURL)
			{
				InternetReadFile(hURL, (LPSTR)szBuffer, (DWORD)50000, &dwBytesRead);
				InternetCloseHandle(hURL);
			}
			else 
				break;
		}
		else
			break;
	}
	while(szBuffer == NULL || strlen(szBuffer) < 3);
	InternetCloseHandle(hSession);
	
	//LogAppend(szBuffer);
	ParseBetween(szBuffer, "<title>", " - Spells -", szAuraName);
	free(szBuffer);
	return TRUE;
}

/******** CUnit class functions start ********/

// Returns most auras active for some reason.
// For a proper aura count, return for i = 1, getAuraCount() do if getAuraSpellId(i) != 0 then count++; end
uint CUnit::auraCount()
{
	UINT count = 0;

	rpm(this->base() + 3536, &count);

	if (count == -1)
		rpm(this->base() + 3156, &count);

  return count;
}

void CUnit::_readName()
{
	DWORD namePtr;

	if (!rpm(this->base() + Offsets::UnitName1, &namePtr))
		return;

	if (!rpm(namePtr + Offsets::UnitName2, &namePtr))
		return;

	if (!rpm(namePtr, &this->Obj.Name, sizeof(this->Obj.Name)))
		return;
}

// UnitData = 0xD0,
// PowerType = 0x47

std::string CUnit::getPowerString()
{
	switch (this->powerType())
	{
		case POWER_TYPE_MANA:
			return "Mana";
			break;

		case POWER_TYPE_RAGE:
			return "Rage";
			break;

		case POWER_TYPE_ENERGY:
			return "Energy";
			break;

		case POWER_TYPE_RUNE:
			return "Runic Power";
			break;
	}

	return "Unknown Power Type";
}

uint CUnit::auraSpellId(UINT auraIndex)
{
	INT buff, buffer;

	rpm(this->base() + 3536, &buff);
	if (buff == -1)
	{
		rpm(this->base() + 3160, &buff);
		rpm(buff + 24 * auraIndex + 8, &buffer);
		return buffer;
	}

	rpm(this->base() + 24 * auraIndex + 3160, &buff);
	return buff;
}

bool CUnit::hasAura(UINT spellId)
{
	for (int x = 0; x < this->auraCount(); x++)
	{
		if (this->auraSpellId(x) == spellId)
			return true;
	}

	return false;
}

bool CUnit::hasAura(std::string spellName)
{
	// Attempt to retrieve spellid from name.
	CSpell spell(spellName);
	uint spellId = spell.getId();

	return this->hasAura(spellId);
}

bool CUnit::hasAura(CSpell &spell)
{
	uint spellId = spell.getId();

	return this->hasAura(spellId);
}

bool CUnit::hasBuff(UINT spellId)
{
	for (int x = 0; x < this->auraCount(); x++)
	{
		if (this->auraSpellId(x) == spellId)
			return true;
	}

	return false;
}

bool CUnit::hasBuff(std::string spellName)
{
	// Attempt to retrieve spellid from name.
	CSpell spell(spellName);
	uint spellId = spell.getId();

	return this->hasAura(spellId);
}

bool CUnit::hasBuff(CSpell &spell)
{
	uint spellId = spell.getId();

	return this->hasAura(spellId);
}

void CUnit::UpdateDisplayInfo()
{
	byte stub[] = {
		0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
		0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
		0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
		0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
		0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

		0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, subroutineAddress
		0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ECX, baseAddress
		0x6A, 0x01,							// PUSH 00
		0xFF, 0xD7,							// CALL EDI
		0xC3								// RETN
	};

	DWORD address = this->base();
	DWORD updateAddress = Offsets::CGUnit_C__UpdateDisplayInfo;

	memcpy(&stub[24], &updateAddress, sizeof(updateAddress));
	memcpy(&stub[29], &address, sizeof(address));

	Stub call(stub, sizeof(stub));

	call.allocate();
	if (call.allocated())
	{
		call.inject();
		call.execute();
		call.free();
	}

	if (!call.isSafe())
		velog("Stub error (%s)", call.getErrorString().c_str());
}

void CUnit::UpdateMountDisplayInfo()
{
	byte stub[] = {
		0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
		0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
		0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
		0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
		0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

		0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, subroutineAddress
		0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ECX, baseAddress
		0x6A, 0x01,							// PUSH 00
		0xFF, 0xD7,							// CALL EDI
		0xC3								// RETN
	};

	DWORD address = this->base();
	DWORD updateAddress = Offsets::CGUnit_C__OnMountDisplayChanged;

	memcpy(&stub[24], &updateAddress, sizeof(updateAddress));
	memcpy(&stub[29], &address, sizeof(address));

	Stub call(stub, sizeof(stub));

	call.allocate();
	if (call.allocated())
	{
		call.inject();
		call.execute();
		call.free();
	}

	if (!call.isSafe())
		velog("Stub error (%s)", call.getErrorString().c_str());

}

void CUnit::setDisplayId(uint id)
{
	DWORD address = this->unitField().BaseAddress + Offsets::UNIT_FIELD_DISPLAYID;
	uint displayBuffer = id;

	if (!wpm(address, &displayBuffer, sizeof(displayBuffer)))
	{
		vlog("Unable to write display Id");
		return;
	}
}

void CUnit::setMountDisplayId(uint id)
{
	DWORD address = this->unitField().BaseAddress + Offsets::UNIT_FIELD_MOUNTDISPLAYID;
	uint displayBuffer = id;

	if (!wpm(address, &displayBuffer, sizeof(displayBuffer)))
	{
		velog("Unable to write display Id");
		return;
	}
}


// [19:43:32] Warden disabled
// [19:53:56] disconnected 

#define ALLIANCE 1
#define HORDE	 0

#define HUMAN	1
#define DWARF	3
#define NELF	4
#define GNOME	115
#define DRAENAI 1629

#define ORC    2
#define UNDEAD 5
#define TAUREN 6
#define TROLL  116
#define BELF   1610


int CUnit::getFactionGroup()
{
	switch (this->faction())
	{
	case HUMAN:
	case DWARF:
	case NELF:
	case GNOME:
	case DRAENAI:
		return ALLIANCE;

	case ORC:
	case UNDEAD:
	case TAUREN:
	case TROLL:
	case BELF:
		return HORDE;
	}

	return -1;
}


// ** IDK if this works
bool CUnit::isFriendly(CUnit *other)
{
	// 1 - human
	// 3 - dwarf
	// 4 - nelf
	// 115 - gnome
	// 1629 - draenai

	// 2 - orc
	// 5 - undead
	// 6 - tauren
	// 116 - troll
	// 1610 - belf

	if (other->getFactionGroup() == this->getFactionGroup()) return true;
	return false;

	int buff1, buff2;

	rpm(this->base() + 2496, &buff1);
	rpm(this->base() + 2608, &buff2);

	return buff1 > 0 && !(buff2 & 0x10000000);
}

bool CUnit::isStealthed()
{
	DWORD buffer;
	
	rpm(this->base() + 0xD0, &buffer, sizeof(buffer));
	rpm(buffer + 0x112, &buffer, sizeof(buffer));
	return buffer & 2;
}

bool CUnit::isFlying()
{
	DWORD buffer;

	rpm(this->unitField().BaseAddress + 0x36, &buffer, sizeof(buffer));
	rpm(buffer + 0x44, &buffer, sizeof(buffer));

	return buffer & 0x2000000;
}

bool CUnit::isDead()
{
	return this->health() == 0;
}

bool CUnit::hasLoot()
{
	return this->dynamicFlags() == 0xD;
}

bool CUnit::isAlive()
{
	return !this->isDead();
}

bool CUnit::isMounted()
{
	return (this->unitFlags() & 0x08000000) > 0;
}
	
bool CUnit::isCasting()
{
	DWORD buffer = 0;

	rpm(this->base() + UNIT_CASTING_SPELL, &buffer, sizeof(buffer));
	return buffer;
}

bool CUnit::isChanneling()
{
	return this->unitField().ChannelSpell;
}

bool CUnit::isFishing()
{
	return this->channelSpell() == FISHING_CASTING_SPELL;
}

bool CUnit::inCombat()
{
	return (this->unitField().Unit_Flags & Offsets::UNIT_FLAG_IN_COMBAT) == Offsets::UNIT_FLAG_IN_COMBAT;
}

bool CUnit::owns(CUnit *o)
{
	return o->createdBy() == this->guid();
}

bool CUnit::owns(CGameObject *o)
{
	return o->createdBy() == this->guid();
}
bool CUnit::owns(CGameObject &o)
{
	return o.createdBy() == this->guid();
}

DWORD CUnit::updateMovementPtr()
{
	DWORD buffer;

	rpm(this->base() + 0x788, &buffer);
	return buffer;
}

// Get unit's base address in WoW's memory.
DWORD CUnit::updateFieldBase()
{
	DWORD buffer;
	rpm(this->base() + OBJECT_UNIT_FIELD, &buffer, sizeof(buffer));
	
	this->Obj.UnitField.BaseAddress = buffer;
	return buffer;
}

DWORD CUnit::updateHealth()
{
	rpm(this->baseField() + Offsets::UNIT_FIELD_HEALTH, &Obj.UnitField.Health, sizeof(Obj.UnitField.Health));
	rpm(this->baseField() + Offsets::UNIT_FIELD_HEALTH, &Obj.UnitField.Max_Health, sizeof(Obj.UnitField.Max_Health));

	return this->health();
}

DWORD CUnit::updateFaction()
{
	DWORD buffer;
	rpm(this->baseField() + Offsets::UNIT_FIELD_FACTIONTEMPLATE, &buffer, sizeof(buffer));

	this->Obj.UnitField.Faction = buffer;
	return buffer;
}

// Return guid of the unit's target.
CUnit CUnit::getTarget()
{
	WGUID guid;
	CUnit target;

	rpm(this->baseField() + Offsets::UNIT_FIELD_TARGET, &guid, sizeof(guid));
	this->Obj.UnitField.Target = guid;
	
	target.update(guid);
	return target;
}

bool CUnit::hasNoTarget()
{
	return this->unitField().Target == 0;
}

bool CUnit::hasTarget(WGUID t)
{
	return this->unitField().Target == t;
}

bool CUnit::hasTarget(Object const &t)
{
	return this->unitField().Target == t.GUID;
}

bool CUnit::hasTarget(CObject &t)
{
	return this->unitField().Target == t.guid();
}

/******** CUnit class functions end ********/