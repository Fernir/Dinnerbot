#pragma once

#include <Windows.h>
#include <vector>

#include "..\Main\Constants.h"
#include "..\Main\Structures.h"
#include "..\WoW API\Object Classes\Object.h"

#define NOP 0x90

class Patch
{
private:
	uint len;		// not 0 based
	uint used;		// not 0 based

	BYTE *restore;
	BYTE *patch;

public:
	Patch(uint n);
	~Patch();

	// also remember idea about external language hack by investigating how recent chat messages are stored in wow.
	uint  getLen()		{ return this->len;		}
	BYTE *getPatch()	{ return this->patch;	}
	BYTE *getRestore()	{ return this->restore; }
	
	uint setPatch(BYTE *byte, uint size);
	uint setPatch(uint pos, uint count, BYTE byte);

	uint setRestore(BYTE *byte, uint size);
	uint setRestore(uint pos, uint count, BYTE byte);
	
	bool isNop();
};

namespace Hack
{
	namespace Render
	{
		bool Objects(bool flag);
		bool Players(bool flag);
		bool hasPlayers();
		bool hasObjects();
		bool hasWorld();

		bool M2(bool state);
		bool WMO(bool state);
		bool Water(bool state);
		bool World(bool state);
		bool Terrain(bool state);
		bool Wireframe(bool state);
		bool HorizonMountains(bool state);
	};
	
	namespace Tracking
	{
		DWORD GetState();
		
		bool Humanoids(bool state);
		bool Clear();
	};

	namespace Collision
	{
		extern float playerWidth;

		bool isM2Enabled();
		bool isWMOEnabled();
		bool M2(bool state);
		bool WMO(bool state);
		bool All(bool state);

		float GetPlayerWidth();
		bool  SetPlayerWidth(float size);
	};

	namespace NameFilter
	{
		bool isPatched();
		bool Restore();
		bool Patch();
	};

	namespace Movement
	{
		bool Teleport(Object *object, WOWPOS pos);
		bool Teleport(float X, float Y, float Z);
		bool Teleport(WOWPOS Pos);
		bool isFlying(DWORD base);
		bool Desync(bool state);

		bool SetState(DWORD base, DWORD state);
		bool SetState(DWORD state);

		DWORD GetState(DWORD base);
		DWORD GetState();

		bool SetFlightSpeed(float flightSpeed);
		bool SetSwimSpeed(float speed);
		bool SetSpeed(float speed);

		bool NoFallDamage(bool state);
		
		bool WeirdFly(bool State, float flightSpeed);
		bool Fly	 (bool state, float flightSpeed);
		bool FreezeZ (bool state);
		bool StopFall();
	};

	namespace Morph
	{
		extern std::vector<DWORD> savedIds;

		DWORD GetTargetDisplayId();

		void Save(DWORD displayId);
		void MorphMount(DWORD displayId);
		void Morph(DWORD displayId);
		void Target();
	};
}

bool LoginHack();
bool EnableHacks();
bool LanguageHack();
bool ChangePI(float value);
bool PreAnimatePatch(bool state);

extern DWORD g_passStringAddress;

extern float g_fFlyHack;

// Honorless Target spellId: 2479
