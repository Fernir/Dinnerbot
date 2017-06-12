#include "Memory.h"

#include "Common\Common.h"
#include "Common\Objects.h"
#include "WoW API\Chat.h"

#include "Hooks.h"
#include "Hacks.h"
#include "Endscene.h"

#include <iostream>
#include <psapi.h>
#include <TlHelp32.h>

HANDLE Memory::WoWHandle = NULL;

BOOL g_bCurManager = FALSE;

using namespace Memory;
using namespace Offsets;

byte *Stub::getCode()
{
	return this->code;
}

DWORD Stub::getAddress()
{
	return this->address;
}

uint Stub::getSize()
{
	return this->size;
}

void Stub::setAddress(DWORD a)
{
	this->address = a;
}

void Stub::setSize(uint s)
{
	this->size = s;
}

void Stub::setCode(byte *c)
{
	this->code = c;
}

bool Stub::allocated()
{
	return this->_allocated;
}

std::string Stub::getErrorString()
{
	switch (this->error)
	{
	case Error::Allocate:
		return "Allocate";
		break;

	case Error::Write:
		return "Write";
		break;

	case Error::Read:
		return "Read";
		break;

	case Error::Free:
		return "Free";
		break;

	case Error::Unallocated:
		return "Unallocated";
		break;

	case Error::Safe:
		return "Safe";
		break;

	case Error::Execute:
		return "Execute";
		break;
	}

	return "Unknown";
}

Error Stub::getError()
{
	return this->error;
}

bool Stub::isSafe()
{
	return this->getError() == Error::Safe;
}

Error Stub::allocate()
{
	if (code == NULL || size == NULL) return Error::Unallocated;

	address = (DWORD)VirtualAllocEx(WoW::handle, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!address)
	{
		error = Error::Allocate;
		return error;
	}

	this->_allocated = true;
	error = Error::Safe;
	return error;
}

Error Stub::free()
{
	if (address == NULL || code == NULL || size == NULL) return Error::Unallocated;

	error = (VirtualFreeEx(WoW::handle, (LPVOID)this->address, this->size, MEM_DECOMMIT)) ? Error::Safe : Error::Free;
	return error;
}

Error Stub::execute()
{
	if (address == NULL || code == NULL || size == NULL) return Error::Unallocated;


	HANDLE thread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)this->address, NULL, NULL, NULL);
	if (!thread)
	{
		vlog("Unable to execute thread");
		error = Error::Execute;
		return error;
	}

	error = (WaitForSingleObject(thread, 1000)) ? Error::Safe : Error::Execute;
	CloseHandle(thread);
	return error;
}

Error Stub::inject()
{
	if (address == NULL || code == NULL || size == NULL) return Error::Unallocated;

	error = (wpm(this->getAddress(), this->getCode(), this->getSize())) ? Error::Safe : Error::Write;
	return error;
}

Stub::Stub(DWORD a, byte *c, uint s)
{
	this->address = a;
	this->code = c;
	this->size = s;
	this->_allocated = false;
	this->error = Error::Safe;
}

Stub::Stub(byte *c, uint s)
{
	this->address = NULL;
	this->code = c;
	this->size = s;
	this->_allocated = false;
	this->error = Error::Safe;
}

Stub::~Stub()
{
}

DWORD GetWoWMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS pmc;
	DWORD ret = NULL;

	if (WoW::handle == NULL) return -1;
	if (!GetProcessMemoryInfo(WoW::handle, &pmc, sizeof(pmc))) return -1;

	return pmc.WorkingSetSize;
}

FLOAT GetFramerate()
{ 
	// Reveresed from wow assembly.
	DWORD address = NULL;
	float buffer = NULL;
	double v0; // st7@1
    int v1; // eax@1
    signed int v2; // ecx@1
    double v3; // st7@5
    double result; // st7@6

    v0 = 0;
	v1 = 0;
	v2 = 0;
	v3 = 0;
	result = 0;

	rpm(0xCD7728, &v1, sizeof(v1));
    // v1 = dword_CD7728;
    v2 = 30;
    do
    {
		address = 0xCD76B0 + v1 * sizeof(float); // &flt_CD76B0[v1++];
		rpm(address, &buffer, sizeof(buffer)); // flt_CD76B0
		v0 += buffer; // v0 = v0 + flt_CD76B0[v1++]; 
		v1++;
        if ( v1 == 30 )
            v1 = 0;
        --v2;
    }
    while ( v2 );
    v3 = v0 * 0.033333335;
    if ( v3 >= 0.001 )
        result = 1.0 / v3;
    else
        result = 1000.0;
    return result;
}

BOOL IsWalking()
{
	BYTE bBuff = 255;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.base() + MovementState + 1), &bBuff, sizeof(bBuff), NULL))
		return FALSE;

	if (bBuff == 0) return FALSE;
	else if (bBuff == 1) return TRUE;
	return bBuff;
}

// Note: Rather buggy
BOOL ToggleWalk()
{
	BYTE bBuff;

	if (IsWalking())
	{
		bBuff = 0;
		if (!WriteProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.base() + MovementState + 1), &bBuff, sizeof(bBuff), NULL))
			return FALSE;
	}
	else
	{
		bBuff = 1;
		if (!WriteProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.base() + MovementState + 1), &bBuff, sizeof(bBuff), NULL))
			return FALSE;
	}

	return TRUE;
}

//DWORD dwBuff = NULL;
//DWORD EDX, EAX, EBX, ECX, ESI, EDI;

//ESI = CTM_Activate_Start;
//EAX = CTM_Constant;


//// Initialize EBX
//rpm(ESI + 0x1C, &EDX, sizeof(EDX)); // EDX 
//vlog("EDX: 0x%x, ESI: 0x%x", EDX, ESI);
//rpm(ESI + 0x24, &dwBuff, sizeof(dwBuff)); // ESI + 24

//EDI = EAX;
//EAX = EAX & dwBuff;
//ECX = EAX + EAX * 2;
//ReadProcessMemory(WoW::handle, (LPVOID)(EDX + ECX * 4 + 0x08), &EBX, sizeof(EBX), NULL);

//// Start address calculations
//while (dwBuff != EDI)
//{
//	EAX = EDI;
//	ReadProcessMemory(WoW::handle, (LPVOID)(ESI + 0x24), &dwBuff, sizeof(dwBuff), NULL); // ESI + 24
//	EAX = ESI & dwBuff;

//	EDX = EAX * 2 + EAX; // EDX - LEA EDX,[EAX*2+EAX]	
//	ReadProcessMemory(WoW::handle, (LPVOID)(ESI + 0x1C), &EAX, sizeof(EAX), NULL); // EAX - MOV EAX,DWORD PTR DS:[ESI+1C]

//	EAX = EDX * 4 + EAX; // EAX - LEA EAX,[EDX*4+EAX]
//	ReadProcessMemory(WoW::handle, (LPVOID)(EAX), &EAX, sizeof(EAX), NULL); // EAX - MOV EAX,DWORD PTR DS:[EAX]

//	EAX += EBX; // EAX - ADD EAX,EBX
//	ReadProcessMemory(WoW::handle, (LPVOID)(EAX + 0x4), &EBX, sizeof(EBX), NULL); //EBX - MOV EBX,DWORD PTR DS:[EAX+4] 

//	ReadProcessMemory(WoW::handle, (LPVOID)(EBX), &dwBuff, sizeof(dwBuff), NULL); //EBX - MOV EBX,DWORD PTR DS:[EAX+4] 
//}

//vlog("First answer: 0x%x", EBX + 0x30);

////return EBX + 0x30;

#include <Game\Variable.h>
#include <Game\Pointer.h>

// Reversed from disassembly
DWORD GetCTMSwitch()
{
	WoW::Pointer<uint> esi;
	WoW::Pointer<uint> eax;
	WoW::Pointer<uint> edx, ebx, ecx, edi, buff;

	esi = CTM_Activate_Start;
	eax = CTM_Constant;

	edx = *(esi + 0x1C);

	buff = *(esi + 0x24);

	edi = eax;
	eax = eax & buff;
	ecx = eax + eax * 2;
	
	ebx = *(edx + ecx * 4 + 0x08);

	while (buff != edi)
	{
		eax = edi;
		buff = *(esi + 0x24);
		eax = esi & buff;

		edx = eax * 2 + eax;
		eax = *(esi + 0x1C);

		eax = edx * 4 + eax;
		eax = *(eax);

		eax = eax + ebx;
		ebx = *(eax + 0x4);

		buff = *(ebx);
	}

	return &ebx + 0x30;
}	


// Get status of Battleground Queue.
DWORD GetBGQueue()
{
	DWORD dwBuff;

	ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::BattlegroundStatus), &dwBuff, sizeof(DWORD), NULL);
	return dwBuff;
}

typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);

BOOL ResumeWoW()
{
	NtSuspendProcess pfnNtResumeProcess = (NtSuspendProcess)GetProcAddress(GetModuleHandle("ntdll"), "NtResumeProcess");
	pfnNtResumeProcess(WoW::handle);
	return FALSE;
}

BOOL SuspendWoW()
{
	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(GetModuleHandle("ntdll"), "NtSuspendProcess");
	pfnNtSuspendProcess(WoW::handle);
	return FALSE;
}

VOID StartClickToTeleport(DWORD dwObjectBase)
{
	DWORD dwBuffer;

	if (!WoW::InGame()) return;
	//if (IsWardenLoaded()) return;
	if (IsSubroutineHooked(CGPlayer_C__ClickToMove)) return;

	LocalPlayer.update(LocationInfo | UnitFieldInfo);
	if (!LocalPlayer.isValid()) return;

	BYTE bClickToTeleport[] = {
		/** Parameter Modify Segment. **/
		0x89, 0x0D, 0x00, 0x00, 0x00, 0x00,						// MOV [Buffer1], ECX
		0x89, 0x3D, 0x00, 0x00, 0x00, 0x00,						// MOV [Buffer2], EDI
		0x5F,													// POP EDI
		0x59,													// POP ECX
		0x6A, 0xA,												// PUSH 13 - No movement
		0x57,													// PUSH EDI
		0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,						// MOV ECX, [Buffer1]
		0x8B, 0x3D, 0x00, 0x00, 0x00, 0x00,						// MOV EDI, [Buffer2]

		/** Original Code **/
		0x57,													// PUSH EDI
		0x8B, 0x7C, 0x24, 0x10,									// MOV EDI, [ESP+C]

		/** Invalid Location Segment **/
		0xF3, 0x0F, 0x10, 0x07,									// MOVSS XMM0, [EDI]
		0xF3, 0x0F, 0x10, 0x4F, 0x04,							// MOVSS XMM1, [EDI+4]
		0xF3, 0x0F, 0x10, 0x57, 0x08,							// MOVSS XMM2, [EDI+8]

		0x0F, 0x2E, 0x05, 0x00, 0x00, 0x00, 0x00,				// UCOMISS XMM0, [address of null]
		0x9F,													// LAHF
		0xF6, 0xC4, 0x44,										// TEST AH, 44
		0x7B, 0x38,												// JNP amount

		0x0F, 0x2E, 0x0D, 0x00, 0x00, 0x00, 0x00,				// UCOMISS XMM1, [address of null]
		0x9F,													// LAHF 
		0xF6, 0xC4, 0x44,										// TEST AH, 44
		0x7B, 0x2B,												// JNP amount

		0x0F, 0x2E, 0x15, 0x00, 0x00, 0x00, 0x00,				// UCOMISS XMM2, [address of null]
		0x9F,													// LAHF
		0xF6, 0xC4, 0x44,										// TEST AH, 44
		0x7B, 0x1E,												// JNP amount

		/** Teleport Segment **/
		0x60,													// PUSHAD
		0x9C,													// PUSHFD

		0xD9, 0x07,												// FLD [EDI]
		0xD9, 0x1D, 0x00, 0x00, 0x00, 0x00,						// FSTP [LocalPlayer.X]

		0xD9, 0x47, 0x04,										// FLD [EDI+4]
		0xD9, 0x1D, 0x00, 0x00, 0x00, 0x00,						// FSTP [LocalPlayer.Y]

		0xD9, 0x47, 0x08,										// FLD [EDI+8]
		0xD9, 0x1D, 0x00, 0x00, 0x00, 0x00,						// FSTP [LocalPlayer.Z]

		0x9D,													// POPFD
		0x61,													// POPAD
		0x5F,													// POP EDI
	};
	 
	// Create jmp patch at CTM function to a new cave.
	SuspendWoW();
	HookClickToMove();

	dwBuffer = wClickToMove.Cave + sizeof(bClickToTeleport) + 50;
	memcpy(&bClickToTeleport[2], &dwBuffer, sizeof(dwBuffer));		// &Buffer1
	memcpy(&bClickToTeleport[19], &dwBuffer, sizeof(dwBuffer));		// &Buffer1

	dwBuffer += 4;
	memcpy(&bClickToTeleport[51], &dwBuffer, sizeof(dwBuffer));		// Nullspace
	memcpy(&bClickToTeleport[64], &dwBuffer, sizeof(dwBuffer));		// Nullspace
	memcpy(&bClickToTeleport[77], &dwBuffer, sizeof(dwBuffer));		// Nullspace

	dwBuffer += 4;
	memcpy(&bClickToTeleport[8], &dwBuffer, sizeof(dwBuffer));		// &Buffer2
	memcpy(&bClickToTeleport[25], &dwBuffer, sizeof(dwBuffer));		// &Buffer2

	dwBuffer = dwObjectBase + OBJECT_X;
	memcpy(&bClickToTeleport[93], &dwBuffer, sizeof(dwBuffer));		// &LocalPlayer.X

	dwBuffer = dwObjectBase + OBJECT_Y;
	memcpy(&bClickToTeleport[102], &dwBuffer, sizeof(dwBuffer));	// &LocalPlayer.Y

	dwBuffer = dwObjectBase + OBJECT_Z;
	memcpy(&bClickToTeleport[111], &dwBuffer, sizeof(dwBuffer));	// &LocalPlayer.Z
	
	// Write our asm code to the CTM cave.
	ChangeHookFunction(CGPlayer_C__ClickToMove, wClickToMove.Cave, bClickToTeleport, sizeof(bClickToTeleport));
	ResumeWoW();
	
#ifndef RELEASE
	Thread::DeleteCurrent("StartClickToTeleport");
#else
	Thread::DeleteCurrent();
#endif
}

DWORD GetTimestamp()
{
	DWORD dwTimestamp = NULL;
	ReadProcessMemory(WoW::handle, (LPVOID)(Timestamp), &dwTimestamp, sizeof(dwTimestamp), 0);

	return dwTimestamp;
}

DWORD GetLastHardwareAction()
{
	DWORD dwTimestamp = NULL;
	ReadProcessMemory(WoW::handle, (LPVOID)(LastHardwareAction), &dwTimestamp, sizeof(dwTimestamp), 0);

	return dwTimestamp;
}

BOOL AntiAFK()
{
	DWORD timeStamp = 0;
	static int count = 6;

	if (count++ <= 5)
	{
		return true;
	}
	
	count = 0;
	timeStamp = OsGetAsyncTimeMs();

	//vlog("Timestamp: %d", timeStamp);
	return wpm(LastHardwareAction, &timeStamp, sizeof(timeStamp));
}

Object GetCurrentTarget(DWORD flags)
{
	return GetObjectByGUIDEx(GetCurrentTargetGUID(), flags);
}

// Uses Offsets::CGlueMgr__DefaultServerLogin hook to retrieve password after typed in.
// This is just a proof of concept, and is not currently used.
BOOL GetLoginPass(CHAR *stringReturn, size_t maxSize)
{
	CHAR buffer[MAX_PATH];

	ReadProcessMemory(WoW::handle, (LPVOID)g_passStringAddress, &buffer, maxSize, NULL);

	if (buffer != NULL)
		strncpy(stringReturn, buffer, maxSize);

	return TRUE;
}

bool ReadItemField(DItemField &itemField)
{
	return rpm(itemField.BaseAddress + ITEM_FIELD_OWNER, &itemField.Owner, sizeof(DItemField));
}

bool ReadPlayerField_Out(Object &CurrentObject)
{
	return rpm(CurrentObject.BaseAddress + ITEM_FIELD_OWNER, &CurrentObject.ItemField.Owner, sizeof(DItemField));
}

bool ReadGameObjectField(DGameObjectField &gameObjectField)
{
	return rpm(gameObjectField.BaseAddress + OBJECT_FIELD_CREATED_BY, &gameObjectField.CreatedBy, sizeof(gameObjectField))
		&& rpm(gameObjectField.BaseAddress + GAMEOBJECT_OTHER_FLAGS, &gameObjectField.OtherFlags, sizeof(gameObjectField.OtherFlags));
}

bool ReadUnitPowerStats(Object &obj)
{
	DWORD unitData = NULL;

	// Power type
	rpm(obj.BaseAddress + 0xD0, &unitData, sizeof(unitData));
	rpm(unitData + 0x47, &obj.UnitField.PowerType, sizeof(obj.UnitField.PowerType));

	// Power
	rpm(obj.BaseAddress + obj.UnitField.PowerType * 4 + 0xFB4, &obj.UnitField.Power, sizeof(obj.UnitField.Power));
	rpm(unitData + obj.UnitField.PowerType * 4 + 0x6C, &obj.UnitField.Max_Power, sizeof(obj.UnitField.Max_Power));
	if (obj.UnitField.PowerType == POWER_TYPE_RAGE)
	{
		obj.UnitField.Power /= 10;
		obj.UnitField.Max_Power /= 10;
	}

	return true;
}

bool ReadUnitMovementStats(Object &obj)
{
	rpm(obj.BaseAddress + MovementState, &obj.UnitField.MovementState, sizeof(obj.UnitField.MovementState));
	rpm(obj.BaseAddress + MovementSpeedCurrent, &obj.UnitField.CurrentSpeed, sizeof(obj.UnitField.CurrentSpeed));
	rpm(obj.BaseAddress + MovementSpeedWalking, &obj.UnitField.WalkSpeed, sizeof(obj.UnitField.WalkSpeed));
	rpm(obj.BaseAddress + MovementSpeedActive, &obj.UnitField.ActiveSpeed, sizeof(obj.UnitField.ActiveSpeed));
	rpm(obj.BaseAddress + MovementSpeedSwimming, &obj.UnitField.SwimSpeed, sizeof(obj.UnitField.SwimSpeed));
	return rpm(obj.BaseAddress + MovementSpeedFlying, &obj.UnitField.FlightSpeed, sizeof(obj.UnitField.FlightSpeed));
}

bool ReadUnitField(DUnitField &unitField)
{
	rpm(unitField.BaseAddress + UNIT_FIELD_CHARM, &unitField.Charm, (UNIT_FIELD_HEALTH + 0x4) - UNIT_FIELD_CHARM);
	rpm(unitField.BaseAddress + UNIT_FIELD_MAXHEALTH, &unitField.Max_Health);

	rpm(unitField.BaseAddress + UNIT_FIELD_LEVEL, &unitField.Level);
	rpm(unitField.BaseAddress + UNIT_FIELD_FACTIONTEMPLATE, &unitField.Faction);
	rpm(unitField.BaseAddress + UNIT_VIRTUAL_ITEM_SLOT_ID, &unitField.VirtualItemSlotId);

	rpm(unitField.BaseAddress + UNIT_FIELD_FLAGS, &unitField.Unit_Flags, (UNIT_FIELD_AURASTATE + 0x4) - UNIT_FIELD_FLAGS);

	rpm(unitField.BaseAddress + UNIT_FIELD_DISPLAYID, &unitField.DisplayId, (UNIT_FIELD_MAXDAMAGE + 0x4) - UNIT_FIELD_DISPLAYID);

	rpm(unitField.BaseAddress + UNIT_FIELD_BOUNDINGRADIUS, &unitField.Bounding_Radius, (UNIT_FIELD_COMBATREACH + 0x4) - UNIT_FIELD_BOUNDINGRADIUS);

	rpm(unitField.BaseAddress + UNIT_DYNAMIC_FLAGS, &unitField.Unit_Dynamic_Flags);
	return rpm(unitField.BaseAddress + UNIT_FIELD_BYTES_1, &unitField.Bytes_1);

	// Individual RPMs for now, full struct RPM later.
	//rpm(unitField.BaseAddress + UNIT_FIELD_HEALTH, &unitField.Health, sizeof(CurrentObject.UnitField.Health));
	//rpm(unitField.BaseAddress + UNIT_FIELD_ATTACK_POWER, &CurrentObject.UnitField.Attack_Power, sizeof(CurrentObject.UnitField.Attack_Power));
	//rpm(unitField.BaseAddress + UNIT_FIELD_TARGET, &CurrentObject.UnitField.Target, sizeof(CurrentObject.UnitField.Target));

	//rpm(unitField.BaseAddress + UNIT_FIELD_MAXHEALTH, &CurrentObject.UnitField.Max_Health, sizeof(CurrentObject.UnitField.Max_Health));
	//rpm(unitField.BaseAddress + UNIT_FIELD_MAXDAMAGE, &CurrentObject.UnitField.Max_Damage, sizeof(CurrentObject.UnitField.Max_Damage));
	//rpm(unitField.BaseAddress + UNIT_FIELD_MINDAMAGE, &CurrentObject.UnitField.Min_Damage, sizeof(CurrentObject.UnitField.Min_Damage));
	//rpm(unitField.BaseAddress + UNIT_FIELD_FLAGS, &CurrentObject.UnitField.Unit_Flags, sizeof(CurrentObject.UnitField.Unit_Flags));

	//rpm(unitField.BaseAddress + UNIT_DYNAMIC_FLAGS, &CurrentObject.UnitField.Unit_Dynamic_Flags, sizeof(CurrentObject.UnitField.Unit_Dynamic_Flags));
	//rpm(unitField.BaseAddress + UNIT_FIELD_CREATEDBY, &CurrentObject.UnitField.CreatedBy, sizeof(CurrentObject.UnitField.CreatedBy));
	//rpm(unitField.BaseAddress + UNIT_FIELD_SUMMONEDBY, &CurrentObject.UnitField.SummonedBy, sizeof(CurrentObject.UnitField.SummonedBy));
	//rpm(unitField.BaseAddress + UNIT_FIELD_CHANNEL_OBJECT, &CurrentObject.UnitField.ChannelObject, sizeof(CurrentObject.UnitField.ChannelObject));
	//rpm(unitField.BaseAddress + UNIT_FIELD_DISPLAYID, &CurrentObject.UnitField.DisplayId, sizeof(CurrentObject.UnitField.DisplayId));

	//rpm(unitField.BaseAddress + UNIT_FIELD_LEVEL, &unitField.Level, sizeof(unitField.Level));
	//rpm(unitField.BaseAddress + UNIT_FIELD_COMBATREACH, &unitField.CombatReach, sizeof(unitField.CombatReach));
	//rpm(unitField.BaseAddress + UNIT_CHANNEL_SPELL, &unitField.ChannelSpell, sizeof(unitField.ChannelSpell));
	//rpm(unitField.BaseAddress + UNIT_FIELD_FACTIONTEMPLATE, &unitField.Faction, sizeof(unitField.Faction));

	//rpm(unitField.BaseAddress + UNIT_FIELD_BOUNDINGRADIUS, &unitField.Bounding_Radius, sizeof(unitField.Bounding_Radius));
}

Object ReadGameObjectField(Object CurrentObject)
{
	rpm(CurrentObject.GameObjectField.BaseAddress + OBJECT_FIELD_CREATED_BY, &CurrentObject.GameObjectField.CreatedBy, sizeof(CurrentObject.GameObjectField));
	return CurrentObject;
}

Object ReadUnitField(Object CurrentObject)
{
	DWORD buffer = NULL;

	// Individual RPMs for now, full struct RPM later.
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_HEALTH, &CurrentObject.UnitField.Health, sizeof(CurrentObject.UnitField.Health));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_ATTACK_POWER, &CurrentObject.UnitField.Attack_Power, sizeof(CurrentObject.UnitField.Attack_Power));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_TARGET, &CurrentObject.UnitField.Target, sizeof(CurrentObject.UnitField.Target));

	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_MAXHEALTH, &CurrentObject.UnitField.Max_Health, sizeof(CurrentObject.UnitField.Max_Health));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_MAXDAMAGE, &CurrentObject.UnitField.Max_Damage, sizeof(CurrentObject.UnitField.Max_Damage));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_MINDAMAGE, &CurrentObject.UnitField.Min_Damage, sizeof(CurrentObject.UnitField.Min_Damage));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_FLAGS, &CurrentObject.UnitField.Unit_Flags, sizeof(CurrentObject.UnitField.Unit_Flags));

	rpm(CurrentObject.UnitField.BaseAddress + UNIT_DYNAMIC_FLAGS, &CurrentObject.UnitField.Unit_Dynamic_Flags, sizeof(CurrentObject.UnitField.Unit_Dynamic_Flags));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_CREATEDBY, &CurrentObject.UnitField.CreatedBy, sizeof(CurrentObject.UnitField.CreatedBy));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_SUMMONEDBY, &CurrentObject.UnitField.SummonedBy, sizeof(CurrentObject.UnitField.SummonedBy));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_CHANNEL_OBJECT, &CurrentObject.UnitField.ChannelObject, sizeof(CurrentObject.UnitField.ChannelObject));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_DISPLAYID, &CurrentObject.UnitField.DisplayId, sizeof(CurrentObject.UnitField.DisplayId));

	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_LEVEL, &CurrentObject.UnitField.Level, sizeof(CurrentObject.UnitField.Level));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_COMBATREACH, &CurrentObject.UnitField.CombatReach, sizeof(CurrentObject.UnitField.CombatReach));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_CHANNEL_SPELL, &CurrentObject.UnitField.ChannelSpell, sizeof(CurrentObject.UnitField.ChannelSpell));
	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_FACTIONTEMPLATE, &CurrentObject.UnitField.Faction, sizeof(CurrentObject.UnitField.Faction));

	rpm(CurrentObject.UnitField.BaseAddress + UNIT_FIELD_BOUNDINGRADIUS, &CurrentObject.UnitField.Bounding_Radius, sizeof(CurrentObject.UnitField.Bounding_Radius));
	
	// Power type
	rpm(CurrentObject.BaseAddress + 0xD0, &buffer, sizeof(buffer));
	rpm(buffer + 0x47, &CurrentObject.UnitField.PowerType, sizeof(CurrentObject.UnitField.PowerType));
	
	// Power
	rpm(CurrentObject.BaseAddress + CurrentObject.UnitField.PowerType*4 + 0xFB4, &CurrentObject.UnitField.Power, sizeof(CurrentObject.UnitField.Power));
	
	// ReadProcessMemory(WoW::handle, (LPVOID)(CurrentObject.BaseAddress + UNIT_CASTING_SPELL), &CurrentObject.UnitField.CastingSpell, sizeof(CurrentObject.UnitField.CastingSpell), NULL);
	
	rpm(CurrentObject.BaseAddress + MovementState, &CurrentObject.UnitField.MovementState, sizeof(CurrentObject.UnitField.MovementState));
	rpm(CurrentObject.BaseAddress + MovementSpeedCurrent, &CurrentObject.UnitField.CurrentSpeed, sizeof(CurrentObject.UnitField.CurrentSpeed));
	rpm(CurrentObject.BaseAddress + MovementSpeedWalking, &CurrentObject.UnitField.WalkSpeed, sizeof(CurrentObject.UnitField.WalkSpeed));
	rpm(CurrentObject.BaseAddress + MovementSpeedActive, &CurrentObject.UnitField.ActiveSpeed, sizeof(CurrentObject.UnitField.ActiveSpeed));
	rpm(CurrentObject.BaseAddress + MovementSpeedSwimming, &CurrentObject.UnitField.SwimSpeed, sizeof(CurrentObject.UnitField.SwimSpeed));
	rpm(CurrentObject.BaseAddress + MovementSpeedFlying, &CurrentObject.UnitField.FlightSpeed, sizeof(CurrentObject.UnitField.FlightSpeed));
	return CurrentObject;
}

BOOL IsLoading()
{
	BOOL bBuffer = TRUE;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(IsLoadingOrConnecting), &bBuffer, sizeof(bBuffer), 0))
		return TRUE;

	return bBuffer;
}
	
WGUID GetMouseOverGUID()
{
	WGUID targetGUID = NULL;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(MouseOverGUID), &targetGUID, 8, 0))
		return NULL;

	return targetGUID;
}

WGUID GetCurrentTargetGUID()
{
	WGUID targetGUID = NULL;

	if (!WoW::InGame()) return NULL;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(CURRENT_TARGET_GUID), &targetGUID, 8, 0))
		return NULL;

	return targetGUID;
}

WGUID GetLastTarget()
{
	WGUID targetGUID = NULL;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(LAST_TARGET_GUID), &targetGUID, 8, 0))
	{
		return NULL;
	}

	return targetGUID;
}

BOOL ToggleCTM(BOOL nVal)
{
	BOOL nBuff = nVal;

	return WriteProcessMemory(WoW::handle, (LPVOID)(GetCTMSwitch()), &nBuff, sizeof(nBuff), NULL);
}

BOOL SetPlayerRotation(FLOAT fRot)
{
	return WriteProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.base() + OBJECT_ROT), &fRot, sizeof(fRot), NULL);
}

UINT PlayersInArea()
{
	Object CurrentObject, NextObject;
	UINT count = 0;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return NULL; // RPM Error.
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | LocationInfo);
	while (ValidObject(CurrentObject))
	{
		if (CurrentObject.Type_ID == OT_PLAYER && !(CurrentObject.GUID == LocalPlayer.guid()))
			count++;

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}

	return count;
}

uint PlayersInProximity(float radius)
{
	/* Iterates through object list and adds local pools to the list. */
	CObject current, next;

	uint count = 0;

	if (!Memory::Read(g_dwCurrentManager + Offsets::FirstEntry, &current.Obj.BaseAddress, sizeof(DWORD)))
		return count;

	// Set the update flags to include gameobject info for both objects (next, current).
	next.setUpdateFlags(BaseObjectInfo);
	current.setUpdateFlags(BaseObjectInfo);
	current.update();

	while (current.isValid())
	{
		// Make sure the current object is a game object.
		if (current.isPlayer() && !current.isLocalPlayer())
		{
			current.update(LocationInfo);
			if (LocalPlayer.distance(current) < radius)
				count++;
		}

		// Set next.base to the next base address in the object manager.
		next.updateBase(current.nextPtr());
		if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
			break;
		else
			current.update(next.object()); // Otherwise, update the current object as the previous next object.
	}

	return count;
}

bool Memory::StoreTlsRetrieveStub(byte *stub, std::size_t size)
{
	DWORD clientConnection = Offsets::ClientConnection;
	DWORD currentManager = Offsets::CurrentManager;
	DWORD index = Offsets::TlsIndex;

	byte tlsStub[] = {
		/* Update TLS */
		0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
		0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
		0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
		0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
		0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	
	};

	if (size < sizeof(tlsStub)) return false;

	memcpy(tlsStub + 2, &clientConnection, sizeof(clientConnection));
	memcpy(tlsStub + 8, &currentManager, sizeof(currentManager));
	memcpy(tlsStub + 14, &index, sizeof(index));
	memcpy(stub, tlsStub, sizeof(tlsStub));

	return true;
}

std::vector<std::string> GetPlayersInProximity(float radius)
{
	/* Iterates through object list and adds local players to the list. */
	CObject current, next;

	std::vector<std::string> playerNames;

	if (!Memory::Read(g_dwCurrentManager + Offsets::FirstEntry, &current.Obj.BaseAddress, sizeof(DWORD)))
		return playerNames;

	// Set the update flags to include gameobject info for both objects (next, current).
	next.setUpdateFlags(BaseObjectInfo);
	current.setUpdateFlags(BaseObjectInfo);
	current.update();

	while (current.isValid())
	{
		// Make sure the current object is a game object.
		if (current.isPlayer() && !current.isLocalPlayer())
		{
			current.update(LocationInfo);
			if (LocalPlayer.distance(current) < radius)
			{
				current.getName();
				playerNames.push_back(current.name());
			}
		}

		// Set next.base to the next base address in the object manager.
		next.updateBase(current.nextPtr());
		if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
			break;
		else
			current.update(next.object()); // Otherwise, update the current object as the previous next object.
	}

	return playerNames;
}

UINT PlayersInArea(WGUID playerList[64])
{
	Object CurrentObject, NextObject;
	UINT count = 0;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return NULL; // RPM Error.
	}
	
	memset(playerList, 0, sizeof(playerList));
	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo);
	while (ValidObject(CurrentObject))
	{
		if (CurrentObject.Type_ID == OT_PLAYER && !CompareWGUIDS(CurrentObject.GUID, LocalPlayer.guid()))
		{
			playerList[count].low = CurrentObject.GUID.low;
			playerList[count].high= CurrentObject.GUID.high;
			count++;
		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}

	return count;
}

bool DGetObjectEx_Out(DWORD dwBaseAddress, DWORD dwInfoFlags, Object &out)
{
	out.BaseAddress = dwBaseAddress;
	if (!ValidObject(out))
		return false;

	// Full struct rpm.
	rpm(dwBaseAddress + Offsets::eObjectManager::ObjectType, &out.Type_ID, 44);

	if (CHECK_BIT(dwInfoFlags, 4)) // Location
	{
		if (out.Type_ID == OT_GAMEOBJ)
			rpm(dwBaseAddress + GAMEOBJECT_X, &out.Pos, sizeof(out.Pos) - sizeof(FLOAT));
		else
			rpm(dwBaseAddress + OBJECT_X, &out.Pos, sizeof(out.Pos));

		out.MapID = WoW::GetMapID();
		out.ZoneID = WoW::GetZoneID();

		out.X = out.Pos.X;
		out.Y = out.Pos.Y;
		out.Z = out.Pos.Z;
		out.Rotation = out.Pos.Rotation;
	}

	if (CHECK_BIT(dwInfoFlags, 8)) // UnitField
	{
		rpm(dwBaseAddress + OBJECT_UNIT_FIELD, &out.UnitField.BaseAddress, sizeof(out.UnitField.BaseAddress));

		ReadUnitField(out.UnitField);
		ReadUnitPowerStats(out);
		ReadUnitMovementStats(out);
	}

	if (CHECK_BIT(dwInfoFlags, 12)) // GameObjectField
	{
		rpm(dwBaseAddress + OBJECT_GAMEOBJECT_FIELD, &out.GameObjectField.BaseAddress, sizeof(out.GameObjectField.BaseAddress));
		ReadGameObjectField(out);
	}

	if (CHECK_BIT(dwInfoFlags, 16)) // NameInfo
	{
		memset(&out.Name, 0, sizeof(out.Name));
		DGetObjectName(out, out.Name);
	}

	if (CHECK_BIT(dwInfoFlags, 20)) // ItemField
	{
		rpm(dwBaseAddress + OBJECT_ITEM_FIELD, &out.ItemField.BaseAddress, sizeof(out.ItemField.BaseAddress));
		ReadItemField(out.ItemField);
	}

	if (CHECK_BIT(dwInfoFlags, 24)) // PlayerField
	{
		rpm(dwBaseAddress + OBJECT_PLAYER_FIELD, &out.ItemField.BaseAddress, sizeof(out.GameObjectField.BaseAddress));
		//ReadItemField(out);
	}

	return true;
}

Object DGetObjectEx(DWORD dwBaseAddress, DWORD dwInfoFlags)
{
	Object NewObject;

	memset(&NewObject, 0, sizeof(NewObject));
	NewObject.BaseAddress = dwBaseAddress;
	if (!ValidObject(NewObject))
		return NewObject;

	ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress + Offsets::eObjectManager::ObjectType), &NewObject.Type_ID, 44, NULL);

	if (CHECK_BIT(dwInfoFlags, 4)) // Location
	{
		if (NewObject.Type_ID == OT_GAMEOBJ)
			ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress + GAMEOBJECT_X), &NewObject.Pos, sizeof(NewObject.Pos) - sizeof(FLOAT), NULL);
		else
		{
			ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress + OBJECT_X), &NewObject.Pos, sizeof(NewObject.Pos), NULL);
		}

		NewObject.MapID = WoW::GetMapID();
		NewObject.ZoneID = WoW::GetZoneID();

		NewObject.X = NewObject.Pos.X;
		NewObject.Y = NewObject.Pos.Y;
		NewObject.Z = NewObject.Pos.Z;
		NewObject.Rotation = NewObject.Pos.Rotation;
	}

	if (CHECK_BIT(dwInfoFlags, 8)) // UnitField
	{
		ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress + OBJECT_UNIT_FIELD), &NewObject.UnitField.BaseAddress, sizeof(NewObject.UnitField.BaseAddress), NULL);
		NewObject = ReadUnitField(NewObject);
	}

	if (CHECK_BIT(dwInfoFlags, 12)) // GameObject
	{
		ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress + OBJECT_GAMEOBJECT_FIELD), &NewObject.GameObjectField.BaseAddress, sizeof(NewObject.GameObjectField.BaseAddress), NULL);
		NewObject = ReadGameObjectField(NewObject);
	}

	if (CHECK_BIT(dwInfoFlags, 16)) // NameInfo
	{
		memset(&NewObject.Name, 0, sizeof(NewObject.Name));
		DGetObjectName(NewObject, NewObject.Name);
	}
	
	return NewObject;
}

BOOL CompareWGUIDS(WGUID wGuid1, WGUID wGuid2)
{
	if (wGuid1.low == wGuid2.low && wGuid1.high == wGuid2.high)
		return TRUE;

	return FALSE;
}

BOOL Memory::Read(DWORD address, void *buffer, size_t size)
{
	return ReadProcessMemory(WoW::handle, (LPVOID)address, buffer, size, NULL);
}

// Default is DWORD read.
BOOL Memory::Read(DWORD address, void *buffer)
{
	return ReadProcessMemory(WoW::handle, (LPVOID)address, buffer, sizeof(DWORD), NULL);
}

BOOL Memory::CanRead(DWORD address)
{
	MEMORY_BASIC_INFORMATION info;

	VirtualQueryEx(WoW::handle, (LPVOID)address, &info, sizeof(info));
	return info.State == MEM_COMMIT;
}

BOOL Memory::Write(DWORD address, void *buffer, size_t size)
{
	return WriteProcessMemory(WoW::handle, (LPVOID)address, buffer, size, NULL);
}

// Default is DWORD write.
BOOL Memory::Write(DWORD address, void *buffer)
{
	return WriteProcessMemory(WoW::handle, (LPVOID)address, buffer, sizeof(DWORD), NULL);
}

std::vector<AddressSpace> Memory::cache;
bool Memory::cached;

VOID Memory::ClearCache()
{
	Memory::cache.clear();
	Memory::cached = false;
}

BOOL Memory::CacheModules()
{
	MODULEENTRY32 me32;
	HANDLE moduleSnap = NULL;
	DWORD base = NULL;
	
	AddressSpace m;

	if (WoW::handle == NULL) return false;

	Memory::cache.clear();

	// Take a snapshot of all modules loaded into WoW.
	moduleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(WoW::handle));
	if(moduleSnap == INVALID_HANDLE_VALUE)
	{
		LogFile("Memory::CacheModules: Module process snap");
		return(FALSE);
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);
	
	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Module32First(moduleSnap, &me32))
	{
		LogFile("Memory::CacheModules: Module32First failed");
		CloseHandle(moduleSnap);  // clean the snapshot object
		return(FALSE);
	}

	do
	{
		// Retrieve module information.
		m.base = (DWORD)me32.modBaseAddr;
		m.size = me32.modBaseSize;
		m.name = me32.szExePath;

		// Cache the current module.
		cache.push_back(m);
	} 
	while (Module32Next(moduleSnap, &me32));

	CloseHandle(moduleSnap);
	Memory::cached = true;
	return false;
}

BOOL Memory::AddressInD3D9(DWORD address)
{
	AddressSpace *current;
	
	// Traverse the module cache.
	for (int x = 0; x < Memory::cache.size(); x++)
	{
		current = &Memory::cache[x];
		if (current->base <= address)
		{
			// Address within address space.
			if (address <= current->base + current->size)
			{
				// Module has name d3d9.dll.
				if (strstr(current->name.c_str(), "d3d9.dll"))
				{
					return true;
				}
			}
		}
	}

	return false;
}

Object DGetObject(DWORD base)
{
	return DGetObjectEx(base, BaseObjectInfo);
}

bool GetPlayerName(WGUID GUID, char *name)
{
	DWORD mask, base, offset, current;
	DWORD shortGUID, testGUID;
	CHAR  buffer[256];

	rpm(NameCache + NameMask, &mask);
	rpm(NameCache + NameBase, &base);

	shortGUID = GUID.low & 0xFFFFFFFF;
	offset = 12 * (mask & shortGUID);

	rpm(base + offset + 8, &current);
	rpm(base + offset, &offset);
	if ((current & 0x1) == 0x1) return false;

	rpm(current, &testGUID);
	while (testGUID != shortGUID)
	{
		rpm(current + offset + 4, &current);

		if ((current & 0x1) == 0x1) return false;
		rpm(current, &testGUID);
	}

	rpm(current + NameString, &buffer, sizeof(buffer));
	strncpy(name, buffer, strlen(buffer) + 1);
	return true;
}

BOOL DGetObjectName(Object NewObject, CHAR *szString)
{
	if (!ValidObject(NewObject)) 
	{
		strcpy(szString, "<Error>");
		return FALSE;
	}
	switch(NewObject.Type_ID)
	{
	case OT_PLAYER:
		GetPlayerName(NewObject.GUID, szString);
		return TRUE;

	case OT_UNIT:
		GetUnitName(NewObject.BaseAddress, szString);
		return TRUE;

	case OT_GAMEOBJ:
		GetGameObjectName(NewObject.BaseAddress, szString);
		return TRUE;;

	default:
		strcpy(szString, "<Unknown>");
		return TRUE;
	}
}

BOOL Logout()
{
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwQuitGameAddress = Lua_Logout;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

	if (!WoW::InGame())
		return FALSE;

	LogAppend("Logging out...");
	BYTE Sub_CallQuitGame[] = {
			// TLS Overhead
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwInteractAddress
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallQuitGame);
	memcpy(&Sub_CallQuitGame[24], &dwQuitGameAddress, sizeof(dwQuitGameAddress));
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dwSubroutineAddress == NULL)
	{
		vlog("Error allocating memory");
		return FALSE;
	}


	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallQuitGame, sizeof(Sub_CallQuitGame), 0)))
		return FALSE;

	SuspendWoW();
	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	ResumeWoW();
	return TRUE;
}

CHAR *GetGameObjectName(DWORD dwBase)
{
	DWORD dwNamePtr;
	CHAR  szNameBuffer[256];
	CHAR  *szNamePtr;
	
	ReadProcessMemory(WoW::handle, (LPVOID)(dwBase + 0x1A4), &dwNamePtr, sizeof(dwNamePtr), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(dwNamePtr + 0x90), &dwNamePtr, sizeof(dwNamePtr), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(dwNamePtr), &szNameBuffer, sizeof(szNameBuffer), NULL);

	szNamePtr = (CHAR *)malloc(sizeof(CHAR)*strlen(szNameBuffer));
	strcpy(szNamePtr, szNameBuffer);
	return szNamePtr;
}

CHAR *GetGameObjectName(DWORD dwBase, CHAR *szString)
{
	DWORD dwNamePtr;
	CHAR  szNameBuffer[256];
	
	ReadProcessMemory(WoW::handle, (LPVOID)(dwBase + 0x1A4), &dwNamePtr, sizeof(dwNamePtr), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(dwNamePtr + 0x90), &dwNamePtr, sizeof(dwNamePtr), NULL);
	ReadProcessMemory(WoW::handle, (LPVOID)(dwNamePtr), &szNameBuffer, sizeof(szNameBuffer), NULL);

	strcpy(szString, szNameBuffer);
	return NULL;
}

BOOL IsChatboxOpen()
{
	DWORD dwBuff = NULL;

	ReadProcessMemory(WoW::handle, (LPVOID)(ChatboxIsOpen), &dwBuff, sizeof(dwBuff), NULL);
	return dwBuff;
}

Object GetObjectByGUIDEx(WGUID GUID, DWORD dwFlags)
{
	Object CurrentObject, NextObject;
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject, sizeof(DWORD), NULL)))
		return CurrentObject;

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		if (CurrentObject.GUID == GUID)
		{
			return DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | dwFlags);
		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	return CurrentObject;
}

Object GetObjectByGUID(WGUID GUID)
{
	return GetObjectByGUIDEx(GUID, BaseObjectInfo);
}

BOOL ValidObject(Object obj)
{
	return obj.BaseAddress != 0 && (obj.BaseAddress & 1) == 0;
}

BOOL TargetObject(WGUID ObjectGUID)
{
	if (!WoW::InGame()) return false;
	if (!Endscene.CanExecute()) return false;
	
	DWORD totalSize = NULL;
	DWORD stubAddress = NULL;
	DWORD valueLocation = NULL;
	DWORD executeAddress = CGGameUI__Target;

	BYTE callFunction[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress	
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH ObjectGUID.high		
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH ObjectGUID.low						
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x08,					// ADD ESP, 08
			0xC3								// RETN
	};
	
	totalSize = sizeof(callFunction) + FUNCTIONSPACE_MAX;

	stubAddress = Endscene.getFunctionSpace();
	if (stubAddress == NULL)
		return FALSE;

	valueLocation = stubAddress + sizeof(callFunction);
	
	memcpy(&callFunction[24], &executeAddress, sizeof(executeAddress));
	memcpy(&callFunction[29], &ObjectGUID.high, sizeof(ObjectGUID.high));
	memcpy(&callFunction[34], &ObjectGUID.low, sizeof(ObjectGUID.high));
	
	Endscene.ClearFunctionSpace();

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(stubAddress), &callFunction, sizeof(callFunction), 0)))
		return FALSE;

	return Endscene.Execute(stubAddress); // Buffer used for time to return.
}

// Replace with ObjectManager::FindByDisplayId or something
Object GetObjectByDisplayID(DWORD displayID)
{
	Object CurrentObject, NextObject;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject, sizeof(DWORD), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return CurrentObject;
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | UnitFieldInfo | GameObjectInfo);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		if (CurrentObject.Type_ID == OT_GAMEOBJ)
		{
			if (CurrentObject.GameObjectField.DisplayId == displayID)
				return CurrentObject;
		}
		else if (CurrentObject.Type_ID == OT_UNIT)
		{
			if (CurrentObject.UnitField.DisplayId == displayID)
				return CurrentObject;
		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo | UnitFieldInfo | GameObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	return CurrentObject;
}

Object GetObjectByName(CHAR *szName)
{
	Object CurrentObject, NextObject;
	std::string name;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject, sizeof(DWORD), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return CurrentObject;
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		if (CurrentObject.Type_ID == OT_PLAYER || CurrentObject.Type_ID == OT_UNIT || CurrentObject.Type_ID == OT_GAMEOBJ)
		{
			DGetObjectName(CurrentObject, CurrentObject.Name);

			toLower(szName);
			toLower(CurrentObject.Name);
			if (strstr(CurrentObject.Name, szName))
				return CurrentObject;
		}

		NextObject = DGetObject(CurrentObject.Next_Ptr);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	return CurrentObject;
}

BOOL TargetObjectByWrite(Object WObject)
{
	WGUID NullGUID;

	memset(&NullGUID, 0, sizeof(WGUID));
	WriteProcessMemory(WoW::handle, (LPVOID)(CurrentTargetGUID), &NullGUID, sizeof(WGUID), NULL);
	WriteProcessMemory(WoW::handle, (LPVOID)(LastTargetGUID), &NullGUID, sizeof(WGUID), NULL);

	WriteProcessMemory(WoW::handle, (LPVOID)(CurrentTargetGUID), &WObject.GUID, sizeof(WGUID), NULL);
	return WriteProcessMemory(WoW::handle, (LPVOID)(LastTargetGUID), &WObject.GUID, sizeof(WGUID), NULL);
}

BOOL AttachWoW()
{
	CHAR szBuffer[256];
	int pID;

	LocalPlayer.clear();

	GetWoWWindow();
	GetCurrentManager();

	LocalPlayer.setBase(WoW::GetPlayerBase());
	if (!Thread::FindType(Thread::eType::InfoLoop))
	{
		LogFile("AttachWoW: New info loop thread spawned");
		Thread::Create(WoWInfoLoop, NULL, Thread::eType::InfoLoop, Thread::ePriority::Low);
	}
	
	pID = GetProcessId(WoW::handle);
	GetLocalPlayerName(LocalPlayer.Obj.Name);

	// Set Dinnerbot's window title to the account name.
	SetWindowText(g_hwMainWindow, WoW::GetAccountName().c_str());

	// Set WoW's window title to "<player name> (account name) - process ID."
	sprintf_s(szBuffer, "%s (%s) - %d", LocalPlayer.name(), WoW::GetAccountName().c_str(), pID);
	SetWindowText(g_hwWoW, szBuffer);

	SetWindowText(g_hwMainWindow, WoW::GetAccountName().c_str());
	SetDlgItemText(g_hwMainWindow, IDC_MAIN_TAB_GROUPBOX_PLAYER, LocalPlayer.name());

	if (g_chatLog) LogStatus(true);
	LogFile(szBuffer);

	LocalPlayer.update(LocationInfo | UnitFieldInfo);
	if (Hack::Collision::GetPlayerWidth() > 0)
		Hack::Collision::playerWidth = Hack::Collision::GetPlayerWidth();
	
	return true;
}

VOID FrameScriptSignalEvent(INT eventId, CHAR *format, INT arg)
{
	DWORD  functionAddress		= FrameScript_SignalEvent;
	DWORD  subroutineAddress	= NULL;
	DWORD  eventIdAddress		= NULL;
	DWORD  formatAddress		= NULL;
	DWORD  argAdress			= NULL;

	DWORD  dwTotalSize	= NULL;
	HANDLE hThread		= NULL;

	CHAR szBuffer[256];

	BYTE Sub_FrameScriptSignalEvent[] = {
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, Offsets::CGlueMgr__DefaultServerLogin
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH arg
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH format
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH eventId
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x0C,					// ADD ESP, C
			0xC3								// RETN

	};

	dwTotalSize = sizeof(Sub_FrameScriptSignalEvent) + strlen(format) + sizeof(arg) + sizeof(eventId);
	
	subroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!subroutineAddress)
		return;

	formatAddress = subroutineAddress + sizeof(Sub_FrameScriptSignalEvent);
	memcpy(&Sub_FrameScriptSignalEvent[24], &functionAddress, sizeof(functionAddress));
	memcpy(&Sub_FrameScriptSignalEvent[29], &arg, sizeof(argAdress));
	memcpy(&Sub_FrameScriptSignalEvent[34], &formatAddress, sizeof(formatAddress));
	memcpy(&Sub_FrameScriptSignalEvent[39], &eventId, sizeof(eventIdAddress));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(subroutineAddress), &Sub_FrameScriptSignalEvent, sizeof(Sub_FrameScriptSignalEvent), 0)))
		return;

	strncpy(szBuffer, format, strlen(format) + 1);
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(formatAddress), &szBuffer, strlen(szBuffer), 0)))
		return;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)subroutineAddress, NULL, NULL, NULL);
	if (!hThread)
		return;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	
	if (!VirtualFreeEx(WoW::handle, (LPVOID)subroutineAddress, dwTotalSize, MEM_DECOMMIT))
		return;

	return;
}

VOID Login(CHAR *user, CHAR *pass)
{
	DWORD  functionAddress = Offsets::CGlueMgr__DefaultServerLogin;
	DWORD  subroutineAddress = NULL;
	DWORD  userAddress = NULL;
	DWORD  passAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

	CHAR szBuffer[256];
	
	//Lua::DoString(string_format("DefaultServerLogin(\"%s\", \"%s\")", user, pass).c_str());
	//return;

	BYTE Sub_DefaultServerLogin[] = {
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, Offsets::CGlueMgr__DefaultServerLogin
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH pass
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH user
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x08,					// ADD ESP, 8
			0xC3								// RETN
	};

	dwTotalSize = sizeof(Sub_DefaultServerLogin) + strlen(user) + strlen(pass);
	
	subroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!subroutineAddress)
		return;

	userAddress = sizeof(Sub_DefaultServerLogin) + subroutineAddress;
	passAddress = userAddress + strlen(user) + 1;

	memcpy(&Sub_DefaultServerLogin[24], &functionAddress, sizeof(functionAddress));
	memcpy(&Sub_DefaultServerLogin[29], &passAddress, sizeof(passAddress));
	memcpy(&Sub_DefaultServerLogin[34], &userAddress, sizeof(userAddress));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(subroutineAddress), &Sub_DefaultServerLogin, sizeof(Sub_DefaultServerLogin), 0)))
		return;

	strncpy(szBuffer, user, strlen(user) + 1);
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(userAddress), &szBuffer, strlen(szBuffer), 0)))
		return;

	strncpy(szBuffer, pass, strlen(pass) + 1);
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(passAddress), &szBuffer, strlen(szBuffer), 0)))
		return;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)subroutineAddress, NULL, NULL, NULL);
	if (!hThread)
		return;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	
	if (!(VirtualFreeEx(WoW::handle, (LPVOID)subroutineAddress, dwTotalSize, MEM_DECOMMIT)))
		return;

	return;
}

BOOL GetLocalPlayerName(CHAR *out)
{
	CHAR szName[MAX_PATH];

	if (!rpm(PLAYER_NAME, &szName, sizeof(szName)))
		return FALSE;

	strcpy(out, szName);
	return TRUE;
}

CHAR *NullTerminateString(CHAR *szString)
{
	for (UINT x = 0; x < strlen(szString); x++)
	{
		if (isascii(szString[x])) continue;
		szString[x] = '\0';
	}

	return szString;
}

BOOL GetWoWPlayerName(CHAR *szNameReturn, DWORD dwPID)
{
	HANDLE hProcess;
	CHAR szName[MAX_PATH];

	hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, NULL, dwPID);
	if (!(hProcess))
		return FALSE;

	if (!(ReadProcessMemory(hProcess, (LPVOID)(PLAYER_NAME), &szName, sizeof(szName), 0)))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	CloseHandle(hProcess);
	strcpy(szNameReturn, szName);
	return TRUE;
}

BOOL GetUnitName(DWORD dwBaseAddress, CHAR *szString)
{
	DWORD dwObjectNamePtr;
	CHAR  szNameBuffer[256];

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress + Offsets::UnitName1), &dwObjectNamePtr, sizeof(DWORD), 0))
	{
		strcpy(szString, "<Error>"); 
		return FALSE;
	}

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(dwObjectNamePtr + Offsets::UnitName2), &dwObjectNamePtr, sizeof(DWORD), 0))
	{
		strcpy(szString, "<Error>"); 
		return FALSE;
	}

	if (!ReadProcessMemory(WoW::handle, (LPVOID)dwObjectNamePtr, &szNameBuffer, sizeof(szNameBuffer), 0))
	{
		strcpy(szString, "<Error>"); 
		return FALSE;
	}

	strcpy(szString, szNameBuffer);
	return TRUE;
}

/* Thanks to whoever made this. */
unsigned long dwStartAddress = 0x00401000, dwLen = 0x00861FFF;

bool bDataCompare(const unsigned char* pData, const unsigned char* bMask, const char* szMask)
{
    for(; *szMask; ++szMask, ++pData, ++bMask)
        if(*szMask=='x' && *pData != *bMask )
            return false;
    return (*szMask) == 0;
}

unsigned long dwFindPattern(unsigned char *bMask, char * szMask, unsigned long dw_Address = dwStartAddress, unsigned long dw_Len = dwLen)
{
    for(unsigned long i=0; i < dw_Len; i++)
		if(bDataCompare((unsigned char*)( dw_Address + i ), bMask,szMask))
            return (unsigned long)(dw_Address + i);
    return 0;
}

CHAR *GetUnitName(DWORD dwBaseAddress)
{
	DWORD dwObjectNamePtr;
	CHAR  szNameBuffer[256];
	CHAR  *szNamePtr;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress + UnitName1), &dwObjectNamePtr, sizeof(DWORD), 0))
		return "<Error>";

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(dwObjectNamePtr + UnitName2), &dwObjectNamePtr, sizeof(DWORD), 0))
		return "<Error>";

	if (!ReadProcessMemory(WoW::handle, (LPVOID)dwObjectNamePtr, &szNameBuffer, sizeof(szNameBuffer), 0))
		return "<Error>";

	szNamePtr = (CHAR *)malloc(sizeof(CHAR)*strlen(szNameBuffer));
	strcpy(szNamePtr, szNameBuffer);
	return szNamePtr;
}

CHAR *GetPlayerAccount(DWORD dwPID)
{
	HANDLE hProcess;
	CHAR szName[64];
	CHAR *szNamePtr;

	memset(&szName, 0, sizeof(szName));
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, dwPID);
	if (!(hProcess))
		return "";

	if (!(ReadProcessMemory(hProcess, (LPVOID)(ACCOUNT_NAME), &szName, sizeof(szName), 0)))
	{
		CloseHandle(hProcess);
		return "";
	}

	szNamePtr = (CHAR*)malloc(sizeof(CHAR)*strlen(szName));
	strcpy(szNamePtr, szName);

	CloseHandle(hProcess);
	return szNamePtr;
}

INT GetUnitReaction(INT nFaction, Object Unit)
{
	HANDLE hThread;
	DWORD  dwTotalSize;
	DWORD  dwCaveAddress;

	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallGetUnitReaction[] = {
			// Update current manager0x2ED0
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xE0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

			// Start CallGetUnitReaction code.     
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Unit.BaseAddress	<-- Insert object base address
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH nFaction			<-- Insert unit faction
			0xE8, 0x60, 0x4C, 0x60, 0x00,		// Call Sub_GetUnitReaction <-- Insert GetUnitReaction address
			0x83, 0xC4, 0x08,					// ADD ESP, 8
			0xC3								// RETN
	};		
	
	dwTotalSize = sizeof(Sub_CallGetUnitReaction);
	
	memcpy(&Sub_CallGetUnitReaction[24], &Unit.BaseAddress, sizeof(Unit.BaseAddress));
	memcpy(&Sub_CallGetUnitReaction[29], &nFaction, sizeof(nFaction));
	dwCaveAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dwCaveAddress == NULL)
		return 0;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwCaveAddress), &Sub_CallGetUnitReaction, sizeof(Sub_CallGetUnitReaction), 0)))
		return 0;


	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwCaveAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return 0;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	VirtualFreeEx(WoW::handle, (LPVOID)dwCaveAddress, dwTotalSize, MEM_DECOMMIT);

	return 0;
}

/*
	Object CurrentObject, NextObject;
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject, sizeof(DWORD), NULL)))
		return CurrentObject;

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | dwFlags);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		if (CompareWGUIDS(CurrentObject.GUID, GUID))
		{
			CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | dwFlags);
			return CurrentObject;
		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo | dwFlags);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	return CurrentObject;*/

int MeTargetCallback(CObject *CurrentObject)
{
	CUnit unit(CurrentObject->object());

	CHAR szBuffer[256];
	if (unit.type() == OT_PLAYER)
	{
		unit.update(BaseObjectInfo | UnitFieldInfo | NameInfo);
		if (unit.hasTarget(LocalPlayer))
		{
			vlog("%s is targetting you", unit.name());
			log(szBuffer);

			AddChatMessage(szBuffer, TurqoiseMessage);
		}
	}

	return true;
}

VOID TargetList()
{
	ObjectManager::ObjectCallback(MeTargetCallback);
}

Object FindLocalBobber()
{
	Object CurrentObject, NextObject;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject.BaseAddress, sizeof(DWORD), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return CurrentObject;
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | GameObjectInfo);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		switch(CurrentObject.Type_ID)
		{
		case OT_GAMEOBJ:
			CurrentObject = ReadGameObjectField(CurrentObject);
			if (CurrentObject.GameObjectField.DisplayId == FISHING_BOBBER_DISPLAYID && CompareWGUIDS(CurrentObject.GameObjectField.CreatedBy, LocalPlayer.guid()))
				return CurrentObject;
		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo | GameObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	return CurrentObject;
}

BOOL Interact(DWORD dwBaseAddress)
{ 
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwInteractAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

	if (!Endscene.CanExecute()) return FALSE;
	if (!WoW::InGame() || dwBaseAddress == 0 || (dwBaseAddress & 1) != 0)
		return FALSE;

	BYTE Sub_CallInteract[] = {
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwInteractAddress
			0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ECX, dwBaseAddress
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallInteract);
	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(dwBaseAddress), &dwInteractAddress, sizeof(dwInteractAddress), NULL)))
		return FALSE;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(dwInteractAddress + (44 * 4)), &dwInteractAddress, sizeof(dwInteractAddress), NULL)))
		return FALSE;

	//57 8B F9 8B 8F A0 01 00 00
	
	memcpy(&Sub_CallInteract[24], &dwInteractAddress, sizeof(dwInteractAddress));
	memcpy(&Sub_CallInteract[29], &dwBaseAddress, sizeof(dwBaseAddress));

	/*dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dwSubroutineAddress == NULL)
		return FALSE;*/

	Endscene.ClearFunctionSpace();
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(Endscene.getFunctionSpace()), &Sub_CallInteract, sizeof(Sub_CallInteract), 0)))
		return FALSE;

	return Endscene.Execute(Endscene.getFunctionSpace());
	/*hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);

	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);*/
}

/*
0x007071D0
int __thiscall GetItemName(int this)
{
    int v1; // eax@1
    ItemCache *v2; // eax@1
    int result; // eax@2
    int v4; // [sp+0h] [bp-8h]@1
    int v5; // [sp+4h] [bp-4h]@1

    v1 = *(_DWORD *)(this + 8);
    v4 = 0;
    v5 = 0;
    v2 = DBItemCache_GetInfoBlockByID(WDB_CACHE_ITEM, *(_DWORD *)(v1 + 12), &v4, 0, 0, 0);
    if ( v2 )
        result = *(_DWORD *)v2->Name[0];
    else
        result = 0;
    return result;
}

*/

BOOL BuyGuildCharter(CHAR *szGuildName)
{ 
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwGuildNameAddress = NULL;
	DWORD  dwTotalSize = NULL;

	HANDLE hThread = NULL;

	BYTE Sub_GetName[] = {
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwGuildNameAddress
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x04,					// ADD ESP, 04
			0xC3								// RETN
	};

	dwTotalSize = sizeof(Sub_GetName) + sizeof(DWORD);

	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dwSubroutineAddress == NULL)
		return FALSE;

	dwGuildNameAddress = dwSubroutineAddress + sizeof(Sub_GetName);

	memcpy(&Sub_GetName[24], &dwGuildNameAddress, sizeof(dwGuildNameAddress));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_GetName, sizeof(Sub_GetName), 0)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);

	if (!(VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT)))
		return FALSE;

	return TRUE;
}

BOOL GetLocalPlayerGUID()
{
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::PlayerGUID), &LocalPlayer.guid(), sizeof(LocalPlayer.guid()), 0))
		return FALSE;

	return TRUE;
}

DWORD GetCurrentManager()
{
	g_bCurManager = TRUE;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::ClientConnection), &g_dwCurrentManager, sizeof(DWORD), NULL))) // Use var as clientConnection
		return FALSE;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::CurrentManager), &g_dwCurrentManager, sizeof(DWORD), NULL))
		return FALSE;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::PlayerGUID), &LocalPlayer.guid(), sizeof(LocalPlayer.guid()), 0))
		return FALSE;

	return g_dwCurrentManager;
}

BOOL GetWoWBuildInfo(CHAR *szBuildReturn, UINT nPID)
{
	CHAR szMajor[7], szMinor[7], szWoWName[18];
	CHAR szBuffer[MAX_PATH];

	HANDLE hProcess = NULL;

	hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, NULL, nPID);

	if (hProcess == NULL) return FALSE;
	if (!ReadProcessMemory(hProcess, (LPVOID)(WOW_NAME), &szWoWName, 17, NULL)) return FALSE;
	if (!ReadProcessMemory(hProcess, (LPVOID)(BUILD_MAJOR), &szMajor, 6, NULL)) return FALSE;
	if (!ReadProcessMemory(hProcess, (LPVOID)(BUILD_MINOR), &szMinor, 6, NULL)) return FALSE;

	sprintf(szBuffer, "%s %s (%s)", szWoWName, szMajor, szMinor);

	strcpy(szBuildReturn, szBuffer);
	return TRUE;
}

INT PopulateWoWList(HWND hListBox)
{
	PROCESSENTRY32 pe32;
	HANDLE hProcessSnap = NULL;
	HANDLE hProcess = NULL;
	CHAR szBuffer[MAX_PATH];
	
	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		LogFile("PopulateWoWList: Invalid process snap");
		return(FALSE);
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		LogFile("PopulateWoWList: Process32First failed");
		CloseHandle(hProcessSnap);  // clean the snapshot object
		return(FALSE);
	}

	// Now walk the snapshot of processes, and
	// add to list when wow.exe is found
	do
	{
		GetWoWBuildInfo(szBuffer, pe32.th32ProcessID);
		if (!_stricmp(szBuffer, SUPPORTED_BUILD))
		{
			GetWoWPlayerName(szBuffer, pe32.th32ProcessID);
			if (!(_stricmp(szBuffer, "")))
			{
				sprintf(szBuffer, "N/A - %d", pe32.th32ProcessID);
			}
			else
			{
				sprintf(szBuffer, "%s - %d", szBuffer, pe32.th32ProcessID);
			}
			
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)szBuffer);
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return NULL;
}

Offsets::eCTM_States GetCTMState()
{
	eCTM_States dwCTMState = eCTM_States::CTM_None;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(CTM_Base + CTM_Action), &dwCTMState, sizeof(DWORD), 0)))
		return eCTM_States::CTM_None;

	return dwCTMState;
}

INT GetUnitReaction(INT nFactionID, DWORD dwObjectBase)
{
  int v2	= 0; // esi@1
  int v4	= 0; // eax@3
  int i		= 0; // eax@8
  int j		= 0; // eax@13

  unsigned int result	= 0; // eax@2
  unsigned int v5		= 0; // ecx@3
  unsigned int v7		= 0; // ecx@8
  unsigned int v9		= 0; // ecx@13

  DWORD dwBuffer	= NULL;
  DWORD dwBuffer1	= NULL;
  DWORD dwBuffer2	= NULL;

  ReadProcessMemory(WoW::handle, (LPVOID)(dwObjectBase + 12), &v2, sizeof(v2), 0); //v2 = *(DWORD *)(dwObjectBase + 12);
  ReadProcessMemory(WoW::handle, (LPVOID)(nFactionID + 20), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)(nFactionID + 20)
  if ( v2 & dwBuffer )
  {
    result = 1;
  }
  else
  {
    v5 = 0;
    v4 = nFactionID + 24;
    ReadProcessMemory(WoW::handle, (LPVOID)(nFactionID + 24), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)(nFactionID + 24)
    if (dwBuffer)
    {
      while ( v5 < 4 )
      {
        ReadProcessMemory(WoW::handle, (LPVOID)(dwObjectBase + 4), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)(dwObjectBase + 4)
        ReadProcessMemory(WoW::handle, (LPVOID)(v4), &dwBuffer1, sizeof(dwBuffer1), 0); // *(DWORD *)v4
        if ( dwBuffer1 == dwBuffer )
          return 1;
        v4 += 4;
        ++v5;
        ReadProcessMemory(WoW::handle, (LPVOID)(v4), &dwBuffer1, sizeof(dwBuffer1), 0); // *(DWORD *)v4
        if ( !dwBuffer1 )
          break;
      }
    }
        
	ReadProcessMemory(WoW::handle, (LPVOID)(nFactionID + 16), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)(nFactionID + 16)
    if ( v2 & dwBuffer )
      goto LABEL_19;
    v7 = 0;
	ReadProcessMemory(WoW::handle, (LPVOID)(nFactionID + 40), &dwBuffer2, sizeof(dwBuffer2), 0); // *(DWORD *)i
    for ( i = nFactionID + 40; dwBuffer2; ++v7 )
    {
      if ( v7 >= 4 )
        break;
	  
	  ReadProcessMemory(WoW::handle, (LPVOID)(dwObjectBase + 4), &dwBuffer1, sizeof(dwBuffer1), 0); // *(DWORD *)(dwObjectBase + 4)
	  ReadProcessMemory(WoW::handle, (LPVOID)(i), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)i
      if ( dwBuffer == dwBuffer1 )
        goto LABEL_19;
      i += 4;
	  ReadProcessMemory(WoW::handle, (LPVOID)(i), &dwBuffer2, sizeof(dwBuffer2), 0); // *(DWORD *)i
    }

	ReadProcessMemory(WoW::handle, (LPVOID)(dwObjectBase + 16), &dwBuffer1, sizeof(dwBuffer1), 0); // *(DWORD *)(dwObjectBase + 16)
	ReadProcessMemory(WoW::handle, (LPVOID)(nFactionID + 12), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)(nFactionID + 12)
    if ( dwBuffer & dwBuffer1 )
    {
LABEL_19:
      result = 4;
    }
    else
    {
      v9 = 0;
	  
	  ReadProcessMemory(WoW::handle, (LPVOID)(dwObjectBase + 40), &dwBuffer2, sizeof(dwBuffer2), 0); // *(DWORD *)j
      for ( j = dwObjectBase + 40; dwBuffer2; ++v9 )
      {
        if ( v9 >= 4 )
          break;
	    ReadProcessMemory(WoW::handle, (LPVOID)(nFactionID + 4), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)(nFactionID + 4) 
	    ReadProcessMemory(WoW::handle, (LPVOID)(j), &dwBuffer1, sizeof(dwBuffer1), 0); // *(DWORD *)(nFactionID + 4) 
        if ( dwBuffer1 == dwBuffer )
          goto LABEL_19;
        j += 4;
	    ReadProcessMemory(WoW::handle, (LPVOID)(j), &dwBuffer2, sizeof(dwBuffer2), 0); // *(DWORD *)j
      }

	  ReadProcessMemory(WoW::handle, (LPVOID)(nFactionID + 8), &dwBuffer, sizeof(dwBuffer), 0); // *(DWORD *)(nFactionID + 8) 
      result = ~(dwBuffer >> 12) & 2 | 1;
    }
  }

  return result;
}

BOOL ClickToMove(WOWPOS Pos)
{
	return ClickToMove(Pos.X, Pos.Y, Pos.Z);
}

BOOL UnloadMap(UINT nMapID)
{
	DWORD  dwUnloadMapAddress = CWorld__UnloadMap;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;
	
	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallUnloadMap[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwUnloadMapAddress	
			0x68, 0x00,	0x00, 0x00, 0x00,		// PUSH MapID	
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x04,					// ADD ESP, 4
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallUnloadMap);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallUnloadMap, sizeof(Sub_CallUnloadMap), 0)))
		return FALSE;

	memcpy(&Sub_CallUnloadMap[24], &dwUnloadMapAddress, sizeof(dwUnloadMapAddress));
	memcpy(&Sub_CallUnloadMap[29], &nMapID, sizeof(nMapID));
	if (dwSubroutineAddress == NULL)
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallUnloadMap, sizeof(Sub_CallUnloadMap), 0)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}

BOOL LoadWorld(UINT nMapID)
{
	DWORD  dwLoadNewWorldAddress = LoadNewWorld;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;
	
	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallLoadNewWorld[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwUnloadMapAddress	
			0x68, 0x00,	0x00, 0x00, 0x00,		// PUSH 0	
			0x68, 0x00,	0x00, 0x00, 0x00,		// PUSH MapID	
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x04,					// ADD ESP, 4
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallLoadNewWorld);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallLoadNewWorld, sizeof(Sub_CallLoadNewWorld), 0)))
		return FALSE;

	memcpy(&Sub_CallLoadNewWorld[34], &dwLoadNewWorldAddress, sizeof(dwLoadNewWorldAddress));
	if (dwSubroutineAddress == NULL)
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallLoadNewWorld, sizeof(Sub_CallLoadNewWorld), 0)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}

BOOL ClickToMove(FLOAT X, FLOAT Y, FLOAT Z, WGUID guid, BYTE action)
{
	DWORD  dwClickToMoveAddress = CGPlayer_C__ClickToMove;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwPositionVector = NULL;
	DWORD  dwGUIDLocation = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;
	
	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallClickToMove[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress	
			0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ECX, dwObjectBase
			0x51,								// PUSH ECX							DWORD this_PlayerBase
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Pos							_D3DVECTOR Pos	- Arg3
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Unknown						DWORD Unknown	- Arg2
			0x6A, 0x00,							// PUSH CTM_State					DWORD CTM_STATE	- Arg1
			0xFF, 0xD7,							// CALL EDI
			//0x83, 0xC4, 0x10,					// ADD ESP, 10
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallClickToMove) + 4 * sizeof(FLOAT) + sizeof(guid);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	dwGUIDLocation = dwSubroutineAddress + sizeof(Sub_CallClickToMove) + sizeof(X) + sizeof(Y) + sizeof(Z);

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallClickToMove)), &X, sizeof(X), 0)))
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallClickToMove) + sizeof(X)), &Y, sizeof(Y), 0)))
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallClickToMove) + sizeof(X) + sizeof(Y)), &Z, sizeof(Z), 0)))
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallClickToMove) + sizeof(X) + sizeof(Y) + sizeof(Z)), &guid, sizeof(guid), 0)))
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + dwTotalSize), &hThread, sizeof(hThread), 0)))
		return FALSE;

	dwPositionVector = dwSubroutineAddress + sizeof(Sub_CallClickToMove);
	memcpy(&Sub_CallClickToMove[24], &dwClickToMoveAddress, sizeof(dwClickToMoveAddress));
	memcpy(&Sub_CallClickToMove[29], &LocalPlayer.Obj.BaseAddress, sizeof(LocalPlayer.base()));
	memcpy(&Sub_CallClickToMove[35], &dwPositionVector, sizeof(dwPositionVector));
	memcpy(&Sub_CallClickToMove[40], &dwGUIDLocation, sizeof(dwGUIDLocation));
	memcpy(&Sub_CallClickToMove[45], &action, sizeof(action));
	dwPositionVector = dwSubroutineAddress + dwTotalSize;

	if (dwSubroutineAddress == NULL)
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallClickToMove, sizeof(Sub_CallClickToMove), 0)))
		return FALSE;

	SuspendWoW();
	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);

	ResumeWoW();
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}

BOOL ClickToMove(FLOAT X, FLOAT Y, FLOAT Z, BYTE action)
{
	DWORD  dwClickToMoveAddress = CGPlayer_C__ClickToMove;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwPositionVector = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;
	
	if (!WoW::InGame())
		return FALSE;

	BYTE Sub_CallClickToMove[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress	
			0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ECX, dwObjectBase
			0x51,								// PUSH ECX							DWORD this_PlayerBase
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Pos							_D3DVECTOR Pos	- Arg3
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Unknown						DWORD Unknown	- Arg2
			0x6A, 0x00,							// PUSH CTM_State					DWORD CTM_STATE	- Arg1
			0xFF, 0xD7,							// CALL EDI
			//0x83, 0xC4, 0x10,					// ADD ESP, 10
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallClickToMove) + 4 * sizeof(FLOAT);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallClickToMove)), &X, sizeof(X), 0)))
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallClickToMove) + sizeof(X)), &Y, sizeof(Y), 0)))
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallClickToMove) + sizeof(X) + sizeof(Y)), &Z, sizeof(Z), 0)))
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + dwTotalSize), &hThread, sizeof(hThread), 0)))
		return FALSE;

	dwPositionVector = dwSubroutineAddress + sizeof(Sub_CallClickToMove);
	memcpy(&Sub_CallClickToMove[24], &dwClickToMoveAddress, sizeof(dwClickToMoveAddress));
	memcpy(&Sub_CallClickToMove[29], &LocalPlayer.Obj.BaseAddress, sizeof(LocalPlayer.Obj.BaseAddress));
	memcpy(&Sub_CallClickToMove[35], &dwPositionVector, sizeof(dwPositionVector));
	memcpy(&Sub_CallClickToMove[45], &action, sizeof(action));
	dwPositionVector = dwSubroutineAddress + dwTotalSize;

	memcpy(&Sub_CallClickToMove[40], &dwPositionVector, sizeof(dwPositionVector));

	if (dwSubroutineAddress == NULL)
		return FALSE;
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallClickToMove, sizeof(Sub_CallClickToMove), 0)))
		return FALSE;

	SuspendWoW();
	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);

	ResumeWoW();
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}


BOOL ClickToMove(WOWPOS pos, WGUID guid, BYTE action)
{
	return ClickToMove(pos.X, pos.Y, pos.Z, guid, action);
}

BOOL ClickToMove(WOWPOS pos, BYTE action)
{
	return ClickToMove(pos.X, pos.Y, pos.Z, action);
}

BOOL ClickToMove(FLOAT X, FLOAT Y, FLOAT Z)
{
	return ClickToMove(X, Y, Z, CTM_Move);
}

/*
BOOL TerrainClick(WOWPOS Pos)
{
	DWORD  dwHandleTerrainClickAddress = Spell_C__HandleTerrainClick;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwClickEventAddress = NULL;
	DWORD  dwTotalSize = NULL;
	DWORD  dwNothing = NULL;
	HANDLE hThread = NULL;

	CTerrainClickEvent clickEvent = {0, 0, Pos.X, Pos.Y, Pos.Z};
	
	if (!IsIngame())
		return FALSE;

	BYTE Sub_CallHandleTerrainClick[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress	
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH &clickEvent
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x04,					// ADD ESP, 0x04
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallHandleTerrainClick) + sizeof(CTerrainClickEvent);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	
	if (dwSubroutineAddress == NULL)
		return FALSE;
	
	dwClickEventAddress = dwSubroutineAddress + sizeof(Sub_CallHandleTerrainClick);
	memcpy(&Sub_CallHandleTerrainClick[24], &dwHandleTerrainClickAddress, sizeof(dwHandleTerrainClickAddress));
	memcpy(&Sub_CallHandleTerrainClick[29], &dwClickEventAddress, sizeof(dwClickEventAddress));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallHandleTerrainClick, sizeof(Sub_CallHandleTerrainClick), 0)))
		return FALSE;
	
	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwClickEventAddress), &dwNothing, sizeof(dwNothing), 0)))
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwClickEventAddress + 4), &dwNothing, sizeof(dwNothing), 0)))
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwClickEventAddress + 8), &Pos.X, sizeof(Pos.X), 0)))
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwClickEventAddress + 12), &Pos.Y, sizeof(Pos.Y), 0)))
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwClickEventAddress + 16), &Pos.Z, sizeof(Pos.Z), 0)))
		return FALSE;

	CHAR szBuffer[64];
	vlog("Loc: 0x%p", dwSubroutineAddress);
	LogAppend(szBuffer);
	return FALSE;

	SuspendWoW();
	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	ResumeWoW();

	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}*/

BOOL CastSpellByID(DWORD dwSpellID)
{
	DWORD  dwCastSpellByIDAddress = Spell_C__CastSpell;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

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
	
	dwTotalSize = sizeof(Sub_CallCastSpellByID);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	
	if (dwSubroutineAddress == NULL)
		return FALSE;

	memcpy(&Sub_CallCastSpellByID[24], &dwCastSpellByIDAddress, sizeof(dwCastSpellByIDAddress));
	memcpy(&Sub_CallCastSpellByID[31], &LocalPlayer.Obj.GUID.low, sizeof(LocalPlayer.Obj.GUID.low));
	memcpy(&Sub_CallCastSpellByID[40], &dwSpellID, sizeof(dwSpellID));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallCastSpellByID, sizeof(Sub_CallCastSpellByID), 0)))
		return FALSE;
	
	SuspendWoW();
	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	ResumeWoW();

	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}

BOOL CallWoWFunction(DWORD dwAddress, LPVOID Parameter)
{
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;

	//if (!IsIngame() || dwAddress == NULL) return FALSE;
	BYTE Sub_CallAddress[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress	
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH Parameter			
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallAddress) + 50;
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(&Sub_CallAddress[24], &dwAddress, sizeof(dwAddress));
	
	if ((DWORD)Parameter == NULLCALL)
		memset(&Sub_CallAddress[28], 0x90, 5);
	else
		memcpy(&Sub_CallAddress[29], &Parameter, 4);

	if (dwSubroutineAddress == NULL)
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallAddress, sizeof(Sub_CallAddress), 0)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);

	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}

/*
BOOL PlaySound(CHAR *szSoundFile)
{
	DWORD  dwSubroutineAddress = CGGameUI__PlaySound;
	DWORD  dwPlaySoundAddress = CGGameUI__PlaySound;
	DWORD  dwSoundFileAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;
	CHAR   szBuffer[64];

	BYTE Sub_CallAddress[] = {
		// Updates TLS 
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwClickToMoveAddress	
			0x6A, 0x00,							// PUSH 0			
			0x6A, 0x00,							// PUSH 0		
			0x6A, 0x00,							// PUSH 0			
			0x68, 0x00,	0x00, 0x00, 0x00,		// PUSH szSoundFile			
			0xFF, 0xD7,							// CALL EDI
			0xC3								// RETN
	};
	
	dwTotalSize = sizeof(Sub_CallAddress) + 50;
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	dwSoundFileAddress = dwSubroutineAddress + sizeof(Sub_CallAddress);

	memcpy(&Sub_CallAddress[24], &dwPlaySoundAddress, sizeof(dwPlaySoundAddress));
	memcpy(&Sub_CallAddress[35], &dwSoundFileAddress, sizeof(dwSoundFileAddress));
	
	if (dwSubroutineAddress == NULL)
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallAddress, sizeof(Sub_CallAddress), 0)))
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSoundFileAddress), szSoundFile, strlen(szSoundFile), 0)))
		return FALSE;

	sprintf(szBuffer, "Address: %p", dwSubroutineAddress);
	LogAppend(szBuffer);
	return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);

	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_DECOMMIT);
	return TRUE;
}*/

BOOL ObjectInObjMgr(Object &obj)
{
	Object CurrentObject, NextObject;

	if (!rpm(g_dwCurrentManager + Offsets::FirstEntry, &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress)))
		return false;

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo);
	while (ValidObject(CurrentObject))
	{
		if (CurrentObject.BaseAddress == obj.BaseAddress)
			return true;

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	return false;
}

BOOL EnableDebugPrivileges()
{
	HANDLE hToken;
	LUID lLuid;
	TOKEN_PRIVILEGES tkPrivileges;

	if (!(OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	if (!(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &lLuid)))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	tkPrivileges.PrivilegeCount = 1;
	tkPrivileges.Privileges[0].Luid = lLuid;
	tkPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkPrivileges, sizeof(tkPrivileges), NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	vflog("Debug privileges enabled");
	return TRUE;
}
