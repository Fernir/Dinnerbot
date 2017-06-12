#pragma once

#include <Windows.h>

#include "Main\Structures.h"

enum DGetObjectFlags
{
	BaseObjectInfo	= 0x00000F,
	LocationInfo	= 0x0000F0,
	UnitFieldInfo	= 0x000F00,
	GameObjectInfo	= 0x00F000,
	NameInfo		= 0x0F0000,
	ItemFieldInfo	= 0xF00000,
};

// Object types from object manager list
enum eObjType
{
	OT_LOCAL = 0,
	OT_ITEM = 1,
	OT_CONTAINER = 2,
	OT_UNIT = 3,
	OT_PLAYER = 4,
	OT_GAMEOBJ = 5,
	OT_DYNOBJ = 6,
	OT_CORPSE = 7,
	OT_FORCEDWORD = 0xFFFFFFFF
}; // Credit ISXWoW

struct DGameObjectField
{
	DWORD BaseAddress;	// 0x00
	WGUID CreatedBy;	// 0x18
	DWORD DisplayId;	// 0x20
	DWORD Flags;		// 0x24
	byte  OtherFlags;	// 0x44
};

struct DItemField
{
	DWORD BaseAddress;
	WGUID Owner;			// 0x0
	WGUID Container;		// 0x8
	WGUID Creator;			// 0x10
	WGUID GiftCreator;		// 0x18
	int StackCount;			// 0x20
	int Duration;			// 0x24
	int SpellCharges;		// 0x28
	byte padding[16];		// 0x2C
	DWORD Flags;			// 0x3C
	int Enchantment;		// 0x40
	DWORD PropertySeed;		// 0xC4
	int RandomPropertiesId; // 0xC8
	int TextId;				// 0xCC
	int Durability;			// 0xD0
	int MaxDurability;		// 0xD4
};

struct DUnitField
{
	DWORD BaseAddress;
	WGUID Charm;				// 0x18
	WGUID Summon;				// 0x20
	WGUID Critter;				// 0x28
			
	WGUID CharmedBy;			// 0x30
	WGUID SummonedBy;			// 0x38
	WGUID CreatedBy;			// 0x40

	WGUID Target;				// 0x48
	WGUID ChannelObject;		// 0x50
	DWORD ChannelSpell;			// 0x58
	DWORD Bytes_0;				// 0x5C

	uint Health;				// 0x60 0x64 - 0x18 = 0x4C read
	uint Max_Health;			// 0x80

	uint Level;					// 0xD8
	DWORD Faction;				// 0xDC
	DWORD VirtualItemSlotId;	// 0xE0

	DWORD Unit_Flags;			// 0xEC
	DWORD Unit_Dynamic_Flags;	// 0xF0
	DWORD Aura_State;			// 0xF4

	DWORD DisplayId;			// 0x10C
	DWORD NativeDisplayId;		// 0x110
	DWORD MountDisplayId;		// 0x114

	float Bounding_Radius;		// 0x104
	float CombatReach;			// 0x108

	float Min_Damage;			// 0x118
	float Max_Damage;			// 0x11C

	DWORD Bytes_1;				// 0x128

	// Power 
	byte PowerType;
	uint Power;
	uint Max_Power;
	uint Attack_Power;

	// Movement 
	DWORD MovementState;
	float CurrentSpeed;
	float ActiveSpeed;
	float WalkSpeed;
	float SwimSpeed;
	float FlightSpeed;
};

struct DPlayerField
{

};

// Object is the most general.
// Has unit and gameobject structs so it can be defined as either one.
struct Object
{
	/* Order of elements is important up to ZoneID. */
	DWORD BaseAddress; // 0x00
	DWORD Type_ID;	// 0x14
	DWORD BYTES_1;	// 0x18
	DWORD BYTES_2;	// 0x1C
	DWORD BYTES_3;	// 0x20
	DWORD BYTES_4;	// 0x24
	DWORD BYTES_5;	// 0x28
	DWORD BYTES_6;	// 0x2C
	WGUID GUID;		// 0x30
	DWORD BYTES_7;	// 0x38
	DWORD Next_Ptr; // 0x3C
	DWORD Obj_Ptr; // 0x120
	WOWPOS Pos;
	float X;
	float Y;
	float Z;
	float BYTES_8;
	float Rotation;
	CHAR  Name[64];

	DUnitField UnitField;
	DGameObjectField GameObjectField;
	DItemField ItemField;

	DWORD ZoneID;
	DWORD MapID;
};


// ** Create derived class that is both gameobject and unit.
class CObject
{
private:
	DWORD upFlags;

public:	
	enum eCreationFlag
	{
		CreateInvalidObject,
	};

	// Constructors
	CObject(eCreationFlag flag);
	CObject(Object obj);
	CObject(DWORD base);
	CObject(WGUID guid);
	CObject();

	// Variable should be public anyways.
	Object Obj;

	// Info functions
	Object object();

	//DWORD dinnerState() { return this->Obj.DinnerState; }
	DWORD zoneId()			{ return this->Obj.ZoneID;		}
	DWORD mapId()			{ return this->Obj.MapID;		}
	virtual DWORD flags()	{ return this->Obj.BYTES_1;		}

	DWORD nextPtr();
	DWORD objPtr();

	DWORD type();
	DWORD base();
	WGUID guid();

	CHAR *name();

	WOWPOS pos();
	
	DGameObjectField gameObjectField();
	DUnitField		 unitField();

	float dotProduct(const WOWPOS &loc);
	float dotProduct(const Object &obj);
	float dotProduct(CObject *other);
	float dotProduct(CObject &obj);

	float distance();
	float distance(WOWPOS loc);
	float distance(Object &obj);
	float distance(CObject &other);

	// Validity checks
	bool isValid();
	bool isUnit();
	bool isItem();
	bool isPlayer();
	bool isGameObject();
	bool isLocalPlayer();

	// Object Callback
	bool hasReturnBytes();
	void setReturnBytes();

	// Update functions
	void getName();
	void setBase(DWORD newBase);

	void set(Object obj);
	void setUpdateFlags(DWORD flags);

	void update();
	void update(WGUID guid);
	void update(DWORD flags);
	void update(Object newObject);
	void update(WGUID guid, DWORD flags);

	void updateBase(DWORD newBase);

	void clear();

	// Interactions
	bool interact();
	bool target();

	// Teleports this object to the specified location (local player is default).
	void teleport();
	void teleport(WOWPOS pos);
	void setRotation(float r);
	void teleport(Object object);

	bool operator != (CObject const &_obj)
	{
		return (this->Obj.BaseAddress != _obj.Obj.BaseAddress) || (this->Obj.GUID != _obj.Obj.GUID);
	}

	bool operator == (CObject const &_obj)
	{
		return !(*this == _obj);
	}

	// Returns if CObject is a CObject
	// NOTE: for template classes using CObject derivative
	virtual bool isType() { return true; }
};

bool ValidGUID(WGUID guid);