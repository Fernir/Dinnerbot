#pragma once

#include <Windows.h>
#include "Main\Structures.h"

#include <string>

class CSpell
{
private:
	enum SpellType
	{
		Channel,
		Instant,
		Cast
	};

	uint spellId;
	SpellType type;

	std::string name;

	DWORD getSpellContainerByName();
	DWORD getSpellMask();

public:
	void stopCasting();
	void cast();

	int  getCooldown();

	bool isValid();
	void invalidate();

	bool canCast();
	bool targetInRange();

	uint retrieveSpellId();
	uint getId();

	std::string &CSpell::toString();

	CSpell(std::string spellName);
	CSpell(uint spellId);
	CSpell();
	~CSpell();
};

namespace Spell
{
	BOOL StopCasting();
	BOOL Cast(const uint spellId);
	BOOL Cast(const char *spellName);
	BOOL isOnCooldown(const int spellId);
};