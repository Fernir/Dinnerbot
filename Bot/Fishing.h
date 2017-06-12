#pragma once

#include <Windows.h>
#include <vector>

#include <Common\Objects.h>
#include <Utilities\Thread.h>
#include <File\FishingProfile.h>
#include <Geometry\Net.h>
#include <WoW API\Spell.h>

#include <mutex>

class Fisherman
{
public:
	enum State
	{
		Cast,
		Wait,
		Loot,
		Hide,
		Stop,
	};

	void fish();
	bool canFish();
	bool castFishing();
	void stopFishing();

	void setProfile(FishingProfile *profile);
	bool runProfile();

	bool isInBlacklist(CObject *pool);

	bool canRun();
	bool wasTimeLimitReached();
	int getTimeLimit();
	int getRunTime();

	Fisherman::State getState();

	CGameObject getLocalBobber();

	Fisherman(ullong timeLimit, Geometry::Net *net);
	Fisherman(ullong timeLimit);
	~Fisherman() {}

	// Fisherman settings
	bool hideFromPlayers;
	bool logoutOnStop;

private:
	// private functions
	void _setState(State state);

	bool _hide();
	bool _onHide();
	void _findSpells();

	void _onFishWait();
	void _onFishLoot();
	void _onFishCasting();
	bool _checkConditions();

	bool _castTimeout();
	bool _walkToNode(WoWPos &, std::vector<WoWPos> &);
	bool _mountUp(WoWPos &);

	void _getLocalPools(std::vector<std::string> &poolNames, std::vector<CObject> *pools);
	std::vector<WoWPos> _findPath(WoWPos &dest);
	void _addToBlacklist(CObject *pool);

	// private variables

	// Fisherman state functions
	Thread *_thread;
	CGameObject _bobber;
	Geometry::Net *_net;

	std::timed_mutex _navigationLock;

	FishingProfile *_profile;

	std::vector<CObject> _poolBlacklist;

	CSpell _hideSpell;
	CSpell _mountSpell;

	ullong _timeLimit;
	ullong _startTime;
	ullong _castElapsed;

	State _state;
	uint _casts;

	WoWPos _currentWalkPoint;
	std::vector<CUnit> _localPlayers;
};

namespace DinnerFish
{
	extern ullong startTime;

	bool Start(int nTimeLimit);
	ullong GetRunTime();

	void PopulateProfiles(HWND hWnd);

};


