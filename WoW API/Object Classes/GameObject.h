#pragma once
#include <Windows.h>

#include "Object.h"
#include "Main\Structures.h"

class CGameObject: public CObject
{
public:
	CGameObject(Object obj) : CObject(obj) { this->update(GameObjectInfo); }
	CGameObject(CObject obj) : CObject(obj) { this->update(GameObjectInfo); }
	CGameObject() : CObject() { }

	WGUID createdBy()		{ return this->gameObjectField().CreatedBy;		}
	DWORD baseField()		{ return this->gameObjectField().BaseAddress;	}
	DWORD displayId()		{ return this->gameObjectField().DisplayId;		}
	virtual DWORD flags()	{ return this->gameObjectField().Flags;			}
	byte  otherFlags()		{ return this->gameObjectField().OtherFlags;	}

	bool isFishingBobber();
	bool isFishingBobberSet();

	virtual bool isType() { return this->isGameObject(); }
};

/*
	OBJECT_FIELD_CREATED_BY = 0x18,
	GAMEOBJECT_DISPLAYID = 0x20,
	GAMEOBJECT_FLAGS = 0x24,
	GAMEOBJECT_PARENTROTATION = 0x28,
	GAMEOBJECT_DYNAMIC = 0x38,
	GAMEOBJECT_FACTION = 0x3C,
	GAMEOBJECT_LEVEL = 0x40,
	GAMEOBJECT_BYTES_1 = 0x44,
	TOTAL_GAMEOBJECT_FIELDS = 0x8
*/