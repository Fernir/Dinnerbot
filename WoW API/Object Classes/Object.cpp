#include "Object.h"
#include "ObjectManager.h"

#include "Game\Game.h"
#include "Memory\Memory.h"
#include "Memory\Hacks.h"

#include <math.h>

bool ValidGUID(WGUID guid)
{
	if (guid.low != NULL || guid.high != NULL) return TRUE;

	return FALSE;
}

CObject::CObject(CObject::eCreationFlag flag)
{
	this->Obj.BaseAddress = NULL;
}

CObject::CObject(DWORD base)
{
	this->upFlags = BaseObjectInfo;
	this->setBase(base);
	this->update();
}

CObject::CObject(Object obj)
{
	upFlags = BaseObjectInfo;
	this->update(obj);
}

CObject::CObject(WGUID guid)
{
	upFlags = BaseObjectInfo;
	this->update(guid);
}

CObject::CObject()
{
	upFlags = BaseObjectInfo;
	this->clear();
}
	
Object CObject::object()
{
	return Obj;
}

DWORD CObject::nextPtr()
{
	return Obj.Next_Ptr;
}

DWORD CObject::objPtr()
{
	return Obj.Obj_Ptr;
}

DWORD CObject::base()
{
	return Obj.BaseAddress;
}

DWORD CObject::type()
{
	return Obj.Type_ID;
}

WGUID CObject::guid()
{
	return Obj.GUID;
}

DGameObjectField CObject::gameObjectField()
{
	return Obj.GameObjectField;
}

DUnitField CObject::unitField()
{
	return Obj.UnitField;
}

CHAR *CObject::name()
{
	return Obj.Name;
}

WOWPOS CObject::pos()
{
	return Obj.Pos;
}

void CObject::getName()
{
	DGetObjectName(this->object(), this->Obj.Name);;
}

// Default update according to this update flags.
void CObject::update()
{
	this->update(upFlags);
}

void CObject::update(WGUID guid, DWORD flags)
{
	upFlags = flags;
	Obj = GetObjectByGUIDEx(guid, flags);
}

// Find object by GUID and set it to this object.
void CObject::update(WGUID guid)
{
	Obj = GetObjectByGUIDEx(guid, upFlags);
}

// Update this object according to flags, using this base.
void CObject::update(DWORD flags)
{
	this->upFlags = flags;
	DGetObjectEx_Out(this->base(), flags, this->Obj);
}

// Set this object to newObject, then update this based on the update flags.
void CObject::update(Object newObject)
{
	this->set(newObject);
	this->update(upFlags);
}

// Copy newObject to this->object().
void CObject::set(Object newObject)
{
	Obj = newObject;
}

// Set the update flags for when we call an update function. 
void CObject::setUpdateFlags(DWORD flags)
{
	upFlags = flags;
}

// Change this base, then update this object.
void CObject::updateBase(DWORD newBase)
{
	Obj.BaseAddress = newBase;
	this->update();
}

// Set this base to newBase.
void CObject::setBase(DWORD newBase)
{
	Obj.BaseAddress = newBase;
}

// Clear this object.
void CObject::clear()
{
	memset(&Obj, 0, sizeof(Object));
}

// Returns if this object is valid or not.
bool CObject::isValid()
{
	return (Obj.BaseAddress != 0 && (Obj.BaseAddress & 1) == 0) && (this->Obj.GUID != 0);
}

bool CObject::isItem()
{
	return this->type() == OT_ITEM;
}

bool CObject::isUnit()
{
	return this->type() == OT_UNIT;
}

bool CObject::isPlayer()
{
	return this->type() == OT_PLAYER;
}

bool CObject::isGameObject()
{
	return this->type() == OT_GAMEOBJ;
}

bool CObject::isLocalPlayer()
{
	return LocalPlayer.guid() == this->guid();
}

bool CObject::hasReturnBytes()
{
	if (Obj.BYTES_1 == 0xFA)
		return true;

	return false;
}

void CObject::setReturnBytes()
{
	if (!this->hasReturnBytes())
	{
		Obj.BYTES_1 = 0xFA;
	}
}


float dotProduct(WOWPOS loc);
float dotProduct(Object obj);
float dotProduct(CObject *other);

float CObject::dotProduct(const WOWPOS &loc)
{
	return this->pos().X * loc.X + this->pos().Y * loc.Y;
}

float CObject::dotProduct(const Object &obj)
{
	return this->pos().X * obj.Pos.X + this->pos().Y * obj.Pos.Y;
}

float CObject::dotProduct(CObject &obj)
{
	return this->pos().X * obj.pos().X + this->pos().Y * obj.pos().Y;
}

float CObject::dotProduct(CObject *obj)
{
	return this->pos().X * obj->pos().X + this->pos().Y * obj->pos().Y;
}

float CObject::distance()
{
	return this->distance(LocalPlayer);
}

float CObject::distance(WOWPOS loc)
{
	float x, y;

	x = loc.X - this->pos().X;
	y = loc.Y - this->pos().Y;
	return sqrt(x*x + y*y);
}

float CObject::distance(CObject &other)
{
	return this->distance(other.pos());
}

float CObject::distance(Object &obj)
{
	return this->distance(obj.Pos);
}

bool CObject::interact()
{
	return Interact(this->base());
}

bool CObject::target()
{
	return TargetObject(this->guid());
}

void CObject::setRotation(float r)
{
	this->Obj.Rotation = r;
	wpm(this->base() + OBJECT_ROT, &r, sizeof(r));
}

void CObject::teleport()
{
	Hack::Movement::Teleport(&this->object(), LocalPlayer.pos());
	WriteProcessMemory(WoW::handle, (LPVOID)(this->base() + OBJECT_ROT), &LocalPlayer.Obj.Pos.Rotation, sizeof(LocalPlayer.Obj.Pos.Rotation), NULL);
}

// Teleports this to pos.
void CObject::teleport(WOWPOS pos)
{
	Hack::Movement::Teleport(&this->object(), pos);
	WriteProcessMemory(WoW::handle, (LPVOID)(this->base() + OBJECT_ROT), &pos.Rotation, sizeof(pos.Rotation), NULL);
}

// Teleports this to object.
void CObject::teleport(Object object)
{
	Hack::Movement::Teleport(&this->object(), object.Pos);
	WriteProcessMemory(WoW::handle, (LPVOID)(this->base() + OBJECT_ROT), &object.Pos.Rotation, sizeof(object.Pos.Rotation), NULL);
}
