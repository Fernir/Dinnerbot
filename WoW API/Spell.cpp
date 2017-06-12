#include "Spell.h"
#include <stdio.h>
#include <string>

#include "Lua.h"
#include "Utilities\Utilities.h"

#include <Memory\Memory.h>
#include <Main\Constants.h>
#include <Game\Game.h>
#include <Game\Variable.h>
#include <Game\Pointer.h>

void CSpell::cast()
{
	Lua::vDo("CastSpellByName(\"%s\")", this->toString().c_str());
}

void CSpell::stopCasting()
{
	Lua::DoString("SpellStopCasting()");
}

int CSpell::getCooldown()
{
	std::string get;

	Lua::vDo("_, d_gspcd, _ = GetSpellCooldown(%d)");
	get = Lua::GetText("d_gspcd");

	if (isinteger(get.c_str()))
		return atoi(get.c_str());
}

bool CSpell::isValid()
{
	return this->spellId > 0;
}

bool CSpell::canCast()
{
	std::string &var = Lua::GenerateLocalVariable(10);

	Lua::vDo
	(
		"local start, duration, enabled = GetSpellCooldown(\'%s\') "
		"local usable, _ = IsUsableSpell(\'%s\') "
		//"print(usable) print(start) print(duration) print(enabled) "
		"%s = (start == 0 and duration == 0 and usable == 1) and 1 or 0 ",
		this->name.c_str(), this->name.c_str(), var.c_str()
	);

	return Lua::GetText(var) == "1";
}

bool CSpell::targetInRange()
{
	std::string &var = Lua::GenerateLocalVariable(10);

	Lua::vDo("%s = (IsSpellInRange(\"%s\", \"target\") == 1) and 1 or 0", var.c_str(), this->name.c_str());
	return Lua::GetText(var) == "1";
}

/*
BOOL CastSpellByID(DWORD spellId)
{
	DWORD  castSpellAddress = Offsets::Spell_C__CastSpell;
	DWORD  allocSubAddress = NULL;
	DWORD  totalSize = NULL;
	HANDLE thread = NULL;

	if (!WoW::InGame())
		return false;

	BYTE Sub_CallCastSpellByID[] = {
		// Updates TLS 
		0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
		0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
		0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
		0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
		0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of function code
		0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwCastSpellByIDAddress	
		0x6A, 0x00,							// PUSH 0
		0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH GUID.low
		0x6A, 0x00,							// PUSH GUID.high (null for local player)
		0x6A, 0x00,							// PUSH 0
		0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH dwSpellID
		0xFF, 0xD7,							// CALL EDI
		0x83, 0xC4, 0x14,					// ADD ESP, 14
		0xC3								// RETN
	};

	totalSize = sizeof(Sub_CallCastSpellByID);
	allocSubAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, totalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (allocSubAddress == NULL)
		return FALSE;

	memcpy(&Sub_CallCastSpellByID[24], &castSpellAddress, sizeof(castSpellAddress));
	memcpy(&Sub_CallCastSpellByID[31], &LocalPlayer.Obj.GUID.low, sizeof(LocalPlayer.Obj.GUID.low));
	memcpy(&Sub_CallCastSpellByID[40], &spellId, sizeof(spellId));

	if (!wpm(allocSubAddress, &Sub_CallCastSpellByID, sizeof(Sub_CallCastSpellByID)))
		return false;

	SuspendWoW();
	thread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)allocSubAddress, NULL, NULL, NULL);
	if (thread == NULL)
		return false;

	WaitForSingleObject(thread, 1000);
	CloseHandle(thread);
	ResumeWoW();

	VirtualFreeEx(WoW::handle, (LPVOID)allocSubAddress, totalSize, MEM_RELEASE);
	return true;
}*/

DWORD CSpell::getSpellMask()
{
	unsigned int v3; // edx@1
	char *v4; // ecx@1
	int v5; // esi@1
	int v6; // edi@1
	int v7; // ebx@1
	int v8; // edi@3
	unsigned int v9; // esi@3
	int v10; // eax@3
	int v11; // edi@3
	int v12; // esi@3
	int v13; // eax@3
	int v14; // edi@3
	int v15; // esi@3
	unsigned int v16; // esi@5
	int v17; // ebx@17
	int v18; // edi@17
	int v19; // esi@17
	int v20; // ebx@17
	int v21; // edi@17
	int v22; // esi@17
	int v23; // ebx@17
	unsigned int v25; // [sp+Ch] [bp-4h]@1
	unsigned int zero_a; // [sp+20h] [bp+10h]@2
	int zero = 0;
	int len;

	char upper[512];

	strncpy(upper, name.c_str(), 512);
	toUpper(upper);

	len = name.size();
	v3 = len;
	v4 = upper;
	v5 = zero;
	v6 = -1640531527;
	v25 = len;
	v7 = -1640531527;
	if (len >= 0xC)
	{
		zero_a = len / 0xC;
		do
		{
			v8 = v6 + (unsigned __int8)v4[4] + (((unsigned __int8)v4[5] + (((unsigned __int8)v4[6] + ((unsigned __int8)v4[7] << 8)) << 8)) << 8);
			v9 = v5 + (unsigned __int8)v4[8] + (((unsigned __int8)v4[9] + (((unsigned __int8)v4[10] + ((unsigned __int8)v4[11] << 8)) << 8)) << 8);
			v10 = (v9 >> 13) ^ (v7
				+ (unsigned __int8)*v4
				+ (((unsigned __int8)v4[1] + (((unsigned __int8)v4[2] + ((unsigned __int8)v4[3] << 8)) << 8)) << 8)
				- v9
				- v8);
			v11 = (v10 << 8) ^ (v8 - v9 - v10);
			v12 = ((unsigned int)v11 >> 13) ^ (v9 - v11 - v10);
			v13 = ((unsigned int)v12 >> 12) ^ (v10 - v12 - v11);
			v14 = (v13 << 16) ^ (v11 - v12 - v13);
			v25 -= 12;
			v15 = ((unsigned int)v14 >> 5) ^ (v12 - v14 - v13);
			v7 = ((unsigned int)v15 >> 3) ^ (v13 - v15 - v14);
			v6 = (v7 << 10) ^ (v14 - v15 - v7);
			v5 = ((unsigned int)v6 >> 15) ^ (v15 - v6 - v7);
			v4 += 12;
			--zero_a;
		} while (zero_a);
		v3 = len;
	}
	v16 = v3 + v5;
	switch (v25)
	{
	case 0xBu:
		v16 += (unsigned __int8)v4[10] << 24;
		goto LABEL_7;
	case 0xAu:
	LABEL_7 :
		v16 += (unsigned __int8)v4[9] << 16;
			goto LABEL_8;
	case 9u:
	LABEL_8 :
		v16 += (unsigned __int8)v4[8] << 8;
			goto LABEL_9;
	case 8u:
	LABEL_9 :
		v6 += (unsigned __int8)v4[7] << 24;
			goto LABEL_10;
	case 7u:
	LABEL_10 :
		v6 += (unsigned __int8)v4[6] << 16;
			 goto LABEL_11;
	case 6u:
	LABEL_11 :
		v6 += (unsigned __int8)v4[5] << 8;
			 goto LABEL_12;
	case 5u:
	LABEL_12 :
		v6 += (unsigned __int8)v4[4];
			 goto LABEL_13;
	case 4u:
	LABEL_13 :
		v7 += (unsigned __int8)v4[3] << 24;
			 goto LABEL_14;
	case 3u:
	LABEL_14 :
		v7 += (unsigned __int8)v4[2] << 16;
			 goto LABEL_15;
	case 2u:
	LABEL_15 :
		v7 += (unsigned __int8)v4[1] << 8;
			 goto LABEL_16;
	case 1u:
	LABEL_16 :
		v7 += (unsigned __int8)*v4;
			 break;
	default:
		break;
	}
	v17 = (v16 >> 13) ^ (v7 - v16 - v6);
	v18 = (v17 << 8) ^ (v6 - v16 - v17);
	v19 = ((unsigned int)v18 >> 13) ^ (v16 - v18 - v17);
	v20 = ((unsigned int)v19 >> 12) ^ (v17 - v19 - v18);
	v21 = (v20 << 16) ^ (v18 - v19 - v20);
	v22 = ((unsigned int)v21 >> 5) ^ (v19 - v21 - v20);
	v23 = ((unsigned int)v22 >> 3) ^ (v20 - v22 - v21);
	return (((v23 << 10) ^ (unsigned int)(v21 - v22 - v23)) >> 15) ^ (v22 - ((v23 << 10) ^ (v21 - v22 - v23)) - v23);
}

DWORD CSpell::getSpellContainerByName()
{
	int this2; 
	unsigned int mask; 
	int currentContainer; 

	char compare[512];

	int buffer, buffer1, buffer2;

	// ContainerDB
	this2 = 0x00BE8E64;
	rpm(this2 + 36, &buffer, sizeof(buffer));

	if (buffer == -1)
		return 0;

	mask = this->getSpellMask();                       // some string to address function involving toUpper

	rpm(this2 + 28, &buffer, sizeof(buffer));
	rpm(this2 + 36, &buffer1, sizeof(buffer1));
	rpm(buffer + 12 * (buffer1 & mask) + 8, &currentContainer, sizeof(currentContainer));

	if (currentContainer & 1 || !currentContainer)
		currentContainer = 0;

	while (!(currentContainer & 1) && currentContainer)
	{
		rpm(currentContainer, &buffer, sizeof(buffer));
		rpm(currentContainer + 20, &buffer1, sizeof(buffer1));
		rpm(buffer1, compare, name.size() + 1);

		if (buffer == mask && !_stricmp(name.c_str(), compare))
			return currentContainer;

		rpm(this2 + 28, &buffer, sizeof(buffer));
		rpm(this2 + 36, &buffer1, sizeof(buffer1));
		rpm(buffer + 12 * (buffer1 & mask), &buffer2, sizeof(buffer2));
		rpm(currentContainer + buffer2 + 4, &buffer, sizeof(buffer));

		currentContainer = buffer;
	}

	return 0;
}

uint CSpell::retrieveSpellId()
{
	DWORD container = NULL;
	DWORD buffer = NULL;
	DWORD result = 0;

	container = this->getSpellContainerByName();
	rpm(container + 32, &buffer, sizeof(buffer));
	rpm(buffer + 8, &result, sizeof(result));
	return result;
}

std::string &CSpell::toString()
{
	return this->name;
}

uint CSpell::getId()
{
	return this->spellId;
}

void CSpell::invalidate()
{
	this->spellId = 0;
}

CSpell::CSpell(std::string spellName)
{
	this->name = spellName;
	this->spellId = this->retrieveSpellId();
}

CSpell::CSpell(uint spellId)
{
	this->name = "";
	this->spellId = spellId;
}

CSpell::CSpell()
{
	this->name = "";
	this->spellId = 0;
}

CSpell::~CSpell()
{

}

namespace Spell
{
	BOOL Cast(const char *spellName)
	{
		return Lua::vDo("CastSpellByName(\"%s\")", spellName);
	}

	BOOL Cast(const uint spellId)
	{
		return Lua::vDo("CastSpellByID(%d)", spellId);
	}

	BOOL StopCasting()
	{	
		return Lua::DoString("SpellStopCasting()");
	}
	
	BOOL isOnCooldown(const int spellId)
	{
		CHAR text[256];

		Lua::vDo("deewrck, rwerfdfd, dfdsfrgdftger = GetSpellCooldown(%d)", spellId);

		//Lua::DoString(text);
		Lua::GetText("rwerfdfd", text);

		if (isinteger(text))
			if (atoi(text) == 0)
				return false;

		return true;
	}

};