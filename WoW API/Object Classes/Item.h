#pragma once

#include "Object.h"

//enum eItemFields
//{
//	ITEM_FIELD_OWNER = 0x0,
//	ITEM_FIELD_CONTAINED = 0x8,
//	ITEM_FIELD_CREATOR = 0x10,
//	ITEM_FIELD_GIFTCREATOR = 0x18,
//	ITEM_FIELD_STACK_COUNT = 0x20,
//	ITEM_FIELD_DURATION = 0x24,
//	ITEM_FIELD_SPELL_CHARGES = 0x28,
//	ITEM_FIELD_FLAGS = 0x3C,
//	ITEM_FIELD_ENCHANTMENT = 0x40,
//	ITEM_FIELD_PROPERTY_SEED = 0xC4,
//	ITEM_FIELD_RANDOM_PROPERTIES_ID = 0xC8,
//	ITEM_FIELD_ITEM_TEXT_ID = 0xCC,
//	ITEM_FIELD_DURABILITY = 0xD0,
//	ITEM_FIELD_MAXDURABILITY = 0xD4,
//	TOTAL_ITEM_FIELDS = 0xE
//};

//ItemCache *__thiscall CGItem_C::GetItemEntry(int this)
// Look into this function

class CItem : public CObject
{
private:
	DItemField itemField;

public:
	CItem(Object obj) : CObject(obj) { this->update(ItemFieldInfo); }
	CItem() : CObject() { }

	DWORD fieldBase()			{ return this->itemField.BaseAddress;			}			
	WGUID owner()				{ return this->itemField.Owner;					}
	WGUID contained()			{ return this->itemField.Container;				}
	WGUID creator()				{ return this->itemField.Creator;				}
	WGUID giftCreator()			{ return this->itemField.GiftCreator;			}
	uint stackCount()			{ return this->itemField.StackCount;			}
	uint duration()				{ return this->itemField.Duration;				}
	uint charges()				{ return this->itemField.SpellCharges;			}
	virtual DWORD flags()		{ return this->itemField.Flags;					}
	uint enchantment()			{ return this->itemField.Enchantment;			}
	uint propertySeed()			{ return this->itemField.PropertySeed;			}
	uint randomPropertiesId()	{ return this->itemField.RandomPropertiesId;	}
	uint textId()				{ return this->itemField.TextId;				}
	uint durability()			{ return this->itemField.Durability;			}
	uint maxDurability()		{ return this->itemField.MaxDurability;			}

	virtual bool isType() { return this->isItem(); }
};