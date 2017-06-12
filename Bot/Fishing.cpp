#include "Fishing.h"
#include <time.h>

#include "Game\Interactions.h"

#include "Memory\Endscene.h"
#include "Geometry\Coordinate.h"

#include "Engine\Navigation\Walker.h"
#include "Engine\Navigation\ClickToMove.h"

#include "WoW API\Lua.h"
#include "WoW API\Chat.h"
#include <WoW API\Spell.h>
#include "WoW API\CGLoot.h"
#include "WoW API\Movement.h"
#include "WoW API\Inventory.h"
#include "WoW API\WorldBorders.h"

#include <Main\Structures.h>
#include <Utilities\Wait.h>

#include <Dialog\Dialog.h>
#include <Dialog\ProfileSettings.h>

#include <Engine\Navigation\Pathfinding.h>
#include <Radar\NetEditor.h>

using namespace Engine;

ullong DinnerFish::startTime = 0;

ullong DinnerFish::GetRunTime()
{
	return GetTickCount64() - DinnerFish::startTime;
}

// Shortcut for setting a WOWPOS.
VOID WOWPOS_set(WOWPOS *a, float x, float y, float z)
{
	a->X = x;
	a->Y = y;
	a->Z = z;
}

// Initialize hardcored profiles.
std::vector<FishingProfile> HardcodedProfiles()
{
	int x = 0;
	WOWPOS nodes[10];
	FishingProfile dfin, pureWater, gsalmon, deepsea, crawdad, gminnow, blackmouth, musselmen;
	std::vector<FishingProfile> profiles;
	 
	// Dragonfin Angelfish
	dfin.name = "Dragonfin Angelfish";
	dfin.zoneName = "Dragonblight";
	dfin.poolNames.push_back("Dragonfin Angelfish School");

	dfin.mapID = MAP_NORTHREND;
	dfin.zoneID = 65; // Dragonblight

	WOWPOS_set(&nodes[x], 3092.82, -1598.55, 16.38);
	dfin.spots.push_back(nodes[x++]);

	WOWPOS_set(&nodes[x], 3080.05, -1585.85, 14.65);
	dfin.spots.push_back(nodes[x++]);

	WOWPOS_set(&nodes[x], 3056.88, -1580.58, 14.83);
	dfin.spots.push_back(nodes[x++]);

	// Add this profile to our vector of profiles.
	profiles.push_back(dfin);
	x = 0;
	
	// Glacial Salmon
	gsalmon.name = "Glacial Salmon";
	gsalmon.zoneName = "Grizzly Hills";
	gsalmon.poolNames.push_back("Glacial Salmon School");

	gsalmon.mapID = MAP_NORTHREND;
	gsalmon.zoneID = 394; // Grizzly Hills

	WOWPOS_set(&nodes[x], 4305.016113, -3184.073730, 308.567047);
	gsalmon.spots.push_back(nodes[x++]);

	WOWPOS_set(&nodes[x], 4303.980469, -3223.318604, 308.551514);
	gsalmon.spots.push_back(nodes[x++]);

	WOWPOS_set(&nodes[x], 4305.036621, -3234.313965, 307.877014);
	gsalmon.spots.push_back(nodes[x++]);
	profiles.push_back(gsalmon);
	x = 0;

	// Musselback Sculpin
	musselmen.name = "Musselback Sculpin";
	musselmen.zoneName = "Borean Tundra";
	musselmen.poolNames.push_back("Musselback Sculpin School");

	// Borean Tundra
	musselmen.zoneID = 3537;
	musselmen.mapID = MAP_NORTHREND;

	WOWPOS_set(&nodes[x], 4461.48, 5396.63, -14.66);
	musselmen.spots.push_back(nodes[x++]);
	profiles.push_back(musselmen);
	x = 0;

	// Oily Blackmouth
	blackmouth.name = "Oily Blackmouth";
	blackmouth.zoneName = "The Barrens";
	blackmouth.poolNames.push_back("Abundant Oily Blackmouth School");
	blackmouth.poolNames.push_back("Lesser Oily Blackmouth School");
	blackmouth.poolNames.push_back("Oily Blackmouth School");
	blackmouth.poolNames.push_back("Firefin Snapper School");

	// The Barrens
	blackmouth.zoneID = 17;
	blackmouth.mapID = MAP_KALIMDOR;

	WOWPOS_set(&nodes[x], -2229.72, -3894.72, 1.67);
	blackmouth.spots.push_back(nodes[x++]);

	WOWPOS_set(&nodes[x], -2283.30, -3883.01, -0.37);
	blackmouth.spots.push_back(nodes[x++]);

	WOWPOS_set(&nodes[x], -2184.79, -3897.81, 0.75);
	blackmouth.spots.push_back(nodes[x++]);
	profiles.push_back(blackmouth);
	x = 0;
	
	// Pure water
	pureWater.name = "Pure Water";
	pureWater.zoneName = "Nagrand";
	pureWater.zoneID = 3518;
	pureWater.mapID = 530;
	pureWater.poolNames.push_back("Pure Water");

	WOWPOS_set(&nodes[x], -1707.98, 7708.81, -17.92);
	pureWater.spots.push_back(nodes[x++]);

	WOWPOS_set(&nodes[x], -1743.21, 7664.92, -13.41);
	pureWater.spots.push_back(nodes[x++]);
	profiles.push_back(pureWater);
	x = 0;

	return profiles;
}

// Fill the combo box with fishing profiles.
void DinnerFish::PopulateProfiles(HWND hWnd)
{	
	// Load other profiles.

	// **Alphabetical order or something for some reason.
	// This creates the possibility of selecting the wrong profile because of the index.

	// Add each profile to the list

	//SendDlgItemMessage(hWnd, IDC_FISH_COMBO_PROFILES, CB_ADDSTRING, (WPARAM)0, (LPARAM)fishingProfiles[x].name.c_str()); 
			
	// Set combo box selection to the first entry
	//SendDlgItemMessageA(hWnd, IDC_FISH_COMBO_PROFILES, CB_SETCURSEL, 0, 0);
}

void startFisherman(ULONGLONG timeLimit)
{
	Fisherman dinnerFish(timeLimit, &NetEditor::net);
	FishingProfile profile;

	std::string profileName;

	if (dinnerFish.canRun())
	{
		// If a fishing profile has been loaded, import it.
		if (Settings::currentProfile.isLoaded())
			dinnerFish.setProfile(&Settings::currentProfile);

		vlog("Dinnerfish set to run for %d minutes", timeLimit);
		dinnerFish.fish();
	}
	else
		vlog("Unable to start Dinnerfish");

#ifndef RELEASE
	Thread::DeleteCurrent("DinnerFish::startFisherman");
#else
	Thread::DeleteCurrent();
#endif
}

bool DinnerFish::Start(int timeLimit)
{
	Thread *thread = NULL;

	if (timeLimit <= 0) return FALSE;

	// If there's already a fishing thread active, stop that thread.
	thread = Thread::FindType(Thread::eType::Bot);
	if (thread)
	{
		thread->stop();
		return false;
	}

	// Start our own fishing thread.
	thread = Thread::Create(startFisherman, (VOID *)timeLimit, Thread::eType::Bot, Thread::ePriority::High);
	thread->setTask(Thread::eTask::Fishing);
	return thread != NULL;
}

// Call the in-game lua function.
// This allows for hooking our function so we may delete any unwanted items on each cast.
bool Fisherman::castFishing()
{
	return Lua::DoString
	(
		"if DinnerDank.Fish ~= nil then "
			"DinnerDank.Fish() "
		"else "
			"CastSpellByName(\"Fishing\") "
		"end"
	);
}

// Return the vector in the given list of vectors that is closest to the local player.
WOWPOS GetClosestNode(std::vector<WOWPOS> *nodes)
{
	FLOAT closestDistance = 99999;
	UINT index = 0;

	for (UINT x = 0; x < nodes->size(); x++)
	{
		if (LocalPlayer.distance(nodes->at(x)) < closestDistance)
		{
			closestDistance = LocalPlayer.distance(nodes->at(x));
			index = x;
		}
	}

	return nodes->at(index);
}

// From the given list of objects, determine the closest one.
CObject *GetClosestObject(std::vector<CObject> &list)
{
	FLOAT closestDistance = 99999;
	CObject *closestObject = NULL;

	for (auto &obj : list)
	{
		// If the current object is closer than the previous closest distance,
		if (obj.distance(LocalPlayer) < closestDistance)
		{
			// set it to be the new closest object.
			closestDistance = obj.distance(LocalPlayer);
			closestObject = &obj;
		}
	}

	return closestObject;
}

BOOL IsOffPath(std::vector<WOWPOS> *path)
{
	return LocalPlayer.distance(GetClosestNode(path)) > 5;
}

// Build the list of all nearby fishing pools.
void Fisherman::_getLocalPools(std::vector<std::string> &poolNames, std::vector<CObject> *pools)
{
	/* Iterates through object list and adds local pools to the list. */
	CObject current, next;
	
	if (!Memory::Read(g_dwCurrentManager + Offsets::eObjectManager::FirstEntry, &current.Obj.BaseAddress, sizeof(DWORD)))
		return;
	
	// Set the update flags to include gameobject info for both objects (next, current).
	next.setUpdateFlags(GameObjectInfo);
	current.setUpdateFlags(GameObjectInfo);
	current.update();

	while (current.isValid())
	{
		// Make sure the current object is a game object.
		if (current.isGameObject())
		{
			// Ensure the object is not in the pool blacklist.
			if (!this->isInBlacklist(&current))
			{
				current.update(NameInfo | LocationInfo);

				// Compare object's name against list of given pool names.
				for (auto &each : poolNames)
				{
					if (strstr(current.name(), each.c_str()))
					{
						current.update(LocationInfo);
						pools->push_back(current);
					}
				}

				/*if (vectorHas<std::string>(current.name(), poolNames))
				{
					current.update(LocationInfo);
					pools->push_back(current);
				}*/
			}
		}

		// Set next.base to the next base address in the object manager.
		next.updateBase(current.nextPtr());
		if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
			break;
		else
			current.update(next.object()); // Otherwise, update the current object as the previous next object.
	}
}

WOWPOS GetClosestRelativeNode(std::vector<WOWPOS> *nodes, CObject *relativeObj)
{
	FLOAT closest = 999999;
	int index = 0;

	for (int x = 0; x < nodes->size(); x++)
	{
		if (relativeObj->distance(nodes->at(x)) < closest)
		{
			// new closest is distance from relativeObj to the x'th node.
			closest = relativeObj->distance(nodes->at(x));
			index = x;
		}
	}

	return nodes->at(index);
}

WOWPOS GetFurthestRelativeNode(std::vector<WOWPOS> *nodes, CObject *relativeObj)
{
	FLOAT furthest = 0;
	int index = 0;

	for (int x = 0; x < nodes->size(); x++)
	{
		if (relativeObj->distance(nodes->at(x)) > furthest)
		{
			// new closest is distance from relativeObj to the x'th node.
			furthest = relativeObj->distance(nodes->at(x));
			index = x;
		}
	}

	return nodes->at(index);
}

std::vector<WoWPos> Fisherman::_findPath(WoWPos &dest)
{
	std::vector<WoWPos> path;

	if (!this->_net || !this->_net->isConnected())
		path.push_back(dest);
	else
	{
		Navigation::Pathfinding pf(this->_net);
		pf.findPath(LocalPlayer.pos(), dest);

		if (pf.pathFound)
			path = pf.getPath();
		else
			path.push_back(dest);
	}

	return path;
}

void Fisherman::setProfile(FishingProfile *profile)
{
	this->_profile = profile;
}

bool playerIsCasting(void *t)
{
	Thread *thread = (Thread *)t;

	if (!thread) false;
	
	vlog("thread check: %d", thread->running());
	vlog("Casting: %d Channeling: %d", LocalPlayer.isCasting(), LocalPlayer.isChanneling());
	return thread->running() && (LocalPlayer.isCasting() || LocalPlayer.isChanneling());
}

bool Fisherman::_mountUp(WoWPos &node)
{
	vlog("Mounting up: %s (%d)", _mountSpell.toString().c_str(), _mountSpell.getId());
	this->_mountSpell.cast();

	// While player is casting, wait.
	Utilities::Wait onMount(playerIsCasting, _thread, 100, 5000);
	if (onMount.wait())
	{
		// Waiting was successfull.
		vlog("Time took to mount: %d", onMount.getElapsed());
	}
	else
	{
		// Otherwise we timed out.
		vlog("Unable to mount");
	}

	// Check fisher status after waiting.
	return this->_checkConditions();
}

bool Fisherman::_walkToNode(WoWPos &node, std::vector<WoWPos> &path)
{
	Thread *walkerThread = NULL;
	Navigation::Walker toNode(path);
	toNode.setTimeout(10000);
	toNode.setTolerance(2);
	toNode.setWait(215);

	// If the node is far away, attempt to mount up.
	/*if (node.distance(LocalPlayer.pos()) > 20)
	{
		if (!this->_mountUp(node))
			return false;
	}*/

	// Create walker thread and make it our child.
	walkerThread = toNode.createWalkThread(&_navigationLock);
	_thread->addChild(walkerThread);

	// Show walk path.
	vlog("Walking to node: (%0.2f, %0.2f, %0.2f): %0.2f yds away", node.x, node.y, node.z, node.distance(LocalPlayer.pos()));
	NetEditor::ShowPath(path);

	// Wait for the walk thread to change state.
	while (!toNode.isDone() && !toNode.pathWalked())
	{
		// Meanwhile, check fisher conditions and stop if any one of them are met.
		if (!this->_checkConditions())
		{
			// Stop walker thread.
			walkerThread->stop();

			// Wait for thread to unlock
			if (_navigationLock.try_lock_for(std::chrono::milliseconds(1000)))
			{
				// Remove walker thread as our child, since it has died.
				vlog("Walker unlocked");
				_navigationLock.unlock();
				_thread->clearChildren();
				NetEditor::ClearPath();
				return true;
			}
			else
			{
				// Otherwise, the operator timed out.
				// Terminate the hanging thread.
				//toNode.getThread()->kill();
				//_thread->removeChild(*walkerThread);
				vlog("Timeout on walker thread (500ms)");
				_navigationLock.unlock();
				walkerThread->kill();
				_thread->clearChildren();
				NetEditor::ClearPath();
				return false;
			}
		}

		Sleep(100);
	}

	NetEditor::ClearPath();
	return true;
}

bool Fisherman::runProfile()
{
	CObject *closestPool;

	WoWPos node;
	std::vector<CObject> pools;
	std::vector<WoWPos>  path;

	// Make sure there are no other navigation thread.
	if (Thread::FindType(Thread::eType::Navigation)) return false;

	// Update information about the local player.
	LocalPlayer.update(LocationInfo | UnitFieldInfo);

	// Get a list of local pools.
	this->_getLocalPools(this->_profile->poolNames, &pools); 
	if (pools.size() == 0)
	{
		return false;
	}

	do
	{
		if (!this->_checkConditions()) break;

		// Get the pool closest to the local player.
		closestPool = GetClosestObject(pools);

		// Ensure the found pool is valid.
		if (!closestPool || !closestPool->isValid())
			return false;
		
		// Get closest fishing spot relative to that pool.
		node = GetClosestRelativeNode(&this->_profile->spots, closestPool);

		if (node.distance(LocalPlayer.pos()) > 1000)
		{
			vlog("Closest node too far away");
			return false;
		}

		if (node.distance(LocalPlayer.pos()) >= 2)
		{
			// Create the path to the fishing spot.
			path = this->_findPath(node);

			// Walk to the node. (Blocking)
			this->_walkToNode(node, path);
		}

		// If the pool is within range, turn to it.
		if (closestPool->distance() < 20)
		{
			LocalPlayer.setRotation(GetRotationToFace(LocalPlayer.pos(), closestPool->pos()));

			//Navigation::face(closestPool->pos());
			Sleep(500);
			SendKey('W', WM_KEYDOWN);
			SendKey('W', WM_KEYUP);
		}

	} while (IsOffPath(&this->_profile->spots)
		|| LocalPlayer.distance(GetClosestRelativeNode(&this->_profile->spots, closestPool)) > 10);

	return true;
}

bool Fisherman::isInBlacklist(CObject *pool)
{
	for (auto &entry : this->_poolBlacklist)
	{
		// If the coordinates are the same between pool and any pool in  the blacklist, return true.
		if (pool->pos() == entry.pos())
			return true;
	}

	return false;
}

void Fisherman::_addToBlacklist(CObject *pool)
{
	this->_poolBlacklist.push_back(*pool);
	vlog("Blacklisted pool: (%0.2f, %0.2f, %0.2f):%0.2f yds away", pool->pos().X, pool->pos().Y, pool->pos().Z, pool->distance());
}

// Returns if we've hidden or not.
bool Fisherman::_hide()
{
	static ullong lastMessageTime = 0;

	// Return if the player is already in stealth.
	if (LocalPlayer.inCombat() || LocalPlayer.isStealthed()) return false; 

	// Message timeout is 4 seconds. Otherwise we get spammed.
	if (GetTickCount64() - lastMessageTime >= 4000)
	{
		AddChatMessage("Player(s) found, attempting to hide...", "DFish", RedWarningMessage);
		log("Player(s) found, attempting to hide...");

		log("Players nearby:");

		// Log the name of every nearby player.
		for (auto &each : this->_localPlayers)
		{
			each.getName();
			vlog("Player %s at (%0.2f, %0.2f, %0.2f) %0.2f yards away", each.name(), each.pos().X, each.pos().Y, each.pos().Z, each.distance());
		}

		lastMessageTime = GetTickCount64();
	}

	// Attempt to hide.
	if (!LocalPlayer.hasAura(_hideSpell))
	{
		if (_hideSpell.canCast())
			_hideSpell.cast();
	}

	//Sleep(2000);

	return true;
}

bool Fisherman::canFish()
{
	return  WoW::InGame() && LocalPlayer.isAlive() && !LocalPlayer.inCombat() && !LocalPlayer.isFishing();
}

CGameObject Fisherman::getLocalBobber()
{
	CObjectManager<CGameObject> manager;

	for (auto &obj : manager)
	{
		if (obj.isFishingBobber() && LocalPlayer.owns(obj))
		{
			return obj;
		}
	}

	CGameObject invalid(CObject::CreateInvalidObject);
	return invalid;
}


uint countLocalBobbers()
{
	uint count = 0;

	CObjectManager<CGameObject> manager;

	for (auto &obj : manager)
	{
		if (obj.isGameObject())
		{
			obj.update(GameObjectInfo);
			if (obj.isFishingBobber() && LocalPlayer.owns(obj))
				count++;
		}
	}

	return count;
}

void GetNearbyPlayers_proc(void *s);

// Get a list of nearby players.
void GetNearbyPlayers(std::vector<CUnit> *players)
{
	CUnit current, next;
	
	if (!rpm(g_dwCurrentManager + Offsets::eObjectManager::FirstEntry, &current.Obj.BaseAddress, sizeof(DWORD)))
		return;
	
	// Get the first object's base info.
	current.update();

	while (current.isValid())
	{
		// Make sure the current object is a nonlocal player.
		if (current.isPlayer() && !current.isLocalPlayer())
		{
			current.update(NameInfo | LocationInfo);
			players->push_back(current);
		}

		// Set next.base to the next base address in the object manager.
		next.updateBase(current.nextPtr());
		if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
			break;
		else
			current.update(next.object()); // Otherwise, update the current object as the previous next object.
	}
}

bool Fisherman::wasTimeLimitReached()
{
	return this->getRunTime() / 1000 / 60 >= this->_timeLimit;
}

bool Fisherman::_onHide()
{
	static bool hasHidden = false;

	// If the option has been set.
	if (this->hideFromPlayers)
	{
		this->_thread->stopChildren();
		// If the list is nonempty and we haven't already hidden ourselves.
		if (!this->_localPlayers.empty() && !hasHidden)
			hasHidden = this->_hide();
		else
		{
			this->_setState(State::Cast);
			hasHidden = false;
		}
	}

	return true;
}

void Fisherman::_onFishCasting()
{
	int localBobbers = countLocalBobbers();

	// If we either can't fish, or there are too many local bobbers, wait for either to clear.
	if (localBobbers > 1 || !this->canFish())
	{
		this->_setState(State::Wait);
		return;
	}

	// If the fishing bobber we found has not been set, wait for it to be ready.
	if (!this->_bobber.isFishingBobberSet() && LocalPlayer.isFishing())
	{
		this->_setState(State::Wait);
		return;
	}
			
	// Delete our old fishing bobber.
	this->_bobber.clear();

	// If we can fish, then fish.
	if (this->canFish())
		this->castFishing();
	else		
		return;

	this->_casts++;
	this->_castElapsed = GetTickCount64();
	this->_setState(State::Wait);
}

void Fisherman::_onFishLoot()
{
	std::string name;

	// Get the local player's fishing bobber.
	_bobber = this->getLocalBobber();

	// If fishing bobber has caught a fish, loot.
	if (_bobber.isFishingBobberSet() && countLocalBobbers() > 0 && ObjectInObjMgr(_bobber.Obj))
	{
		Sleep(100);
		// Loot the fishing bobber.
		_bobber.interact();

		// Wait for the loot window to appear, and timeout after 1 second.
		Utilities::Wait lootWindow(Loot::isWindowClosed, 100, 1000);
		if (lootWindow.wait())
		{
			// Ensure WoW is ready for our input.
			Sleep(100);

			// Loop through each loot item.
			for (int x = Loot::getItemCount(); x > 0; x--)
			{
				// Get the name of the item to be looted, then loot it.
				name = Loot::getSlotName(x);
				Loot::slot(x);

				// If we have a bop item that requires confirmation, accept the dialog.
				Lua::vDo("ConfirmLootSlot(%d)", x);
				vflog("Looted %s", name.c_str());
				vlog("Looted %s", name.c_str());
			}

			// Close loot window.
			Loot::close();
		}
	}
		
	// Return back to casting.
	this->_setState(State::Cast);
}

void Fisherman::_onFishWait()
{
	int localBobberCount = countLocalBobbers();

	// If there are too many active fishing bobbers owned by the local player, then return.
	if (localBobberCount > 0)
	{
		// Get the local player's fishing bobber.
		_bobber = this->getLocalBobber();

		if (localBobberCount > 1 && !LocalPlayer.isFishing())
			return;
	}

	// If there is one that has not been set, then return.
	if (localBobberCount == 1)
	{
		if (!_bobber.isFishingBobberSet())
			return;
	}

	if (this->_castTimeout())
	{
		// If there are no fishing active bobbers owned by the local player, then set our state to cast.
		if (localBobberCount == 0)
		{
			this->_setState(State::Cast);
			return;
		}

		// Make sure the fishing bobber is ours (Warmane and fishing bobber bugs are numerous
		if (!LocalPlayer.owns(_bobber))
			this->_setState(State::Cast);

		// If our bobber was set, then loot it.
		if (_bobber.isFishingBobberSet() && LocalPlayer.owns(_bobber))
			this->_setState(State::Loot);
	}
}

int Fisherman::getTimeLimit()
{
	return this->_timeLimit;
}

int Fisherman::getRunTime()
{
	return GetTickCount64() - this->_startTime;
}

bool Fisherman::_castTimeout()
{
	return (GetTickCount64() - this->_castElapsed) / 1000 > 1;
}

bool Fisherman::canRun()
{
	return Memory::Endscene.IsHooked() && Memory::Endscene.CanExecute();
}

void Fisherman::stopFishing()
{
	this->_setState(State::Stop);
	if (this->_thread)
		this->_thread->stop();
}

void Fisherman::_setState(State state)
{
	this->_state = state;
}

Fisherman::State Fisherman::getState()
{
	return this->_state;
}

bool Fisherman::_checkConditions()
{
	// If the local player is either dead or not in game, then stop.
	if (!this->_thread->running() || LocalPlayer.isDead() || !WoW::InGame())
	{
		// Stop fishing bot.
		this->stopFishing();
		return false;
	}

	/*if (FreeInventorySlots() == 0)
	{
		log("Inventory is full");
		this->stopFishing();
		return false;
	}*/

	// Build the list of local players.
	if (this->hideFromPlayers)
	{
		this->_localPlayers.clear();
		GetNearbyPlayers(&this->_localPlayers);

		if (!this->_localPlayers.empty() && this->_hideSpell.isValid())
		{
			this->_setState(State::Hide);
			return false;
		}
	}
	
	return true;
}

void Fisherman::_findSpells()
{
	CSpell stealth("Stealth");
	CSpell prowl("Prowl");
	CSpell smeld("Shadowmeld");

	std::vector<std::string> mountSpellNames{
		"Swift Palomino", "Swift White Steed"
	};

	// Hide spells.
	if (stealth.isValid())
		this->_hideSpell = stealth;

	else if (prowl.isValid())
		this->_hideSpell = prowl;

	else if (smeld.isValid())
		this->_hideSpell = smeld;

	else
		this->_hideSpell.invalidate();

	// Mount spells.
	for (auto &name : mountSpellNames)
	{
		CSpell mount(name);

		if (mount.isValid())
		{
			_mountSpell = mount;
			break;
		}
	}

	if (_hideSpell.isValid())
		vlog("Found hide spell: %s (%d)", _hideSpell.toString().c_str(), _hideSpell.getId());

	if (_mountSpell.isValid())
		vlog("Found mount spell: %s (%d)", _mountSpell.toString().c_str(), _mountSpell.getId());
}

void Fisherman::fish()
{
	/* Pool List
	   Dragonfin Angelfish: {{3057.175293, -1579.879272, 14.688557}, {3080.058105, -1586.182617, 14.721125}, {3091.589844, -1595.407837, 15.212639}};
	   Tastyfish: {{-13973.818359, 621.078735, 1.246970}, {-14010.210938, 580.748718, 1.976477}};
	   Glacial Salmon: {{4305.016113, -3184.073730, 308.567047}, {4303.980469, -3223.318604, 308.551514}, {4305.036621, -3234.313965, 307.877014}}; "Dustbringer", "Pygmy Suckerfish", "Glacial Salmon", "Reinforced Crate", "Sea Turtle", "Crystallized Water"
	   Imperial Manta Ray School: {{1483.555298, -3006.256836, 1.220604}, {1485.445557, -3008.708984, 1.151438}, {1498.689941, -3032.760742, 0.203080}};
	   Deep Sea 2: {{2658.686768, 3774.576904, 4.649775}, {2658.319580, 3765.974609, 4.647061}, {2653.654053, 3761.896240, 5.899695}};
	   Deep Sea 1: {{2683.930420, 3904.404053, 0.576722}, {2688.203125, 3903.016602, 0.576555}};
	   Mr. Pinchy: {{-3868.868896, 3640.610596, 273.170898}, {-3803.923340, 3672.464111, 273.813507}};
	   Glassfin Minnow: {{5545.80, 423.38, 148.42}, {5519.06, 403.27, 148.74}, {5501.30, 370.69, 147.06}, {5557.52, 435.00, 148.68}};
	   Oily Blackmouth: {{-2229.72, -3894.72, 1.67}, {-2283.30, -3883.01, -0.37}, {-2184.79, -3897.81, 0.75}}; Lesser Oily Blackmouth School
	   */

	bool profileEnabled = false;

	// Retrieve local player zone information.
	LocalPlayer.update(UnitFieldInfo | LocationInfo);

	// Detect any hide and mount spells that the local player might have.
	this->_findSpells();

	// Local player needs to be in the given zone.
	if (this->_profile)
	{
		if (LocalPlayer.zoneId() != this->_profile->zoneID)
		{
			vlog("Fishing profile turned off: Player is currently in zone (%d)", LocalPlayer.zoneId());
			vlog("The profile is located in %s (%d)", this->_profile->zoneName.c_str(), this->_profile->zoneID);
		}
		else
		{
			// Load profile here.
			profileEnabled = true;
			vlog("Fishing profile loaded: %s in zone %s (%d)", this->_profile->name.c_str(), this->_profile->zoneName.c_str(), this->_profile->zoneID);
		}
	}

	if (!profileEnabled)
		vlog("Static fishing routine");

	// This allows for dynamic in game lua hooking (Ex: delete items before we cast fish, etc).
	Lua::DoString("function DinnerDank.Fish() CastSpellByName(\"Fishing\") end");

	// Record in-game the time we started fishing.
	Lua::DoString("DinnerDank.start = GetTime()");

	// While the fishing bot should not be stopped.
	while(this->_thread && this->_thread->running())
	{
		this->_checkConditions();
		switch (this->getState())
		{
		case State::Hide:
			this->_onHide();
			break;

		case State::Cast:
			// Follow whichever profile was selected.
			if (profileEnabled) this->runProfile();
			this->_onFishCasting();
			break;

		case State::Wait:
			this->_onFishWait();
			break;

		case State::Loot:
			this->_onFishLoot();
			break;

		case State::Stop:
			break;
		}

		// Stop fishing and take appropriate action after time limit has been reached.
		if (this->getRunTime() / 1000 >= this->getTimeLimit() * 60)
			this->_thread->stop();

		Sleep(100);
	}

	SetDlgItemText(g_hwMainWindow, IDC_FISH_TAB_BUTTON_START, "Start Fishing");
	vlog("Stopped after %d casts", this->_casts);
	
	if (this->logoutOnStop)
		Logout();

}

Fisherman::Fisherman(ullong timeLimit, Geometry::Net *net)
{
	this->_net = net;
	this->_casts = 0;
	this->_setState(State::Cast);
	this->_timeLimit = timeLimit;
	this->_thread = Thread::GetCurrent();
	this->_startTime = GetTickCount64();
	this->hideFromPlayers = g_DFish_Hide;
	this->logoutOnStop = g_DFish_Logout;
	DinnerFish::startTime = this->_startTime;
}

Fisherman::Fisherman(ullong timeLimit)
{
	this->_net = NULL;
	this->_casts = 0;
	this->_setState(State::Cast);
	this->_timeLimit = timeLimit;
	this->_thread = Thread::GetCurrent();
	this->_startTime = GetTickCount64();
	this->hideFromPlayers = g_DFish_Hide;
	this->logoutOnStop = g_DFish_Logout;
	DinnerFish::startTime = this->_startTime;
}