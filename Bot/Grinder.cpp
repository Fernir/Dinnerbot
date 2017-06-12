#include "Grinder.h"
#include <algorithm>

#include "WoW API\Object Classes\ObjectManager.h"
#include "Memory\Memory.h"
#include "WoW API\CGLoot.h"

#include <Utilities\Wait.h>
#include <Utilities\Utilities.h>

#include <Engine\Navigation\Walker.h>
#include <Engine\Navigation\Pathfinding.h>
#include <Engine\Navigation\ClickToMove.h>

#include <Radar\NetEditor.h>

#include <WoW API\Movement.h>

namespace Engine
{
	void Grinder::clearLootTable()
	{
		this->lootTable.clear();
	}

	void Grinder::addLoot(LootType type)
	{
		this->lootTable.add(type, type, "");
	}

	void Grinder::addLoot(std::string lootName)
	{
		this->lootTable.add(LootType::ItemName, NULL, lootName);
	}

	void Grinder::addTarget(TargetType type, int value)
	{
		this->targetTable.add(type, value, "");
	}

	void Grinder::addTarget(int faction)
	{
		this->targetTable.add(TargetType::Faction, faction, "");
	}

	void Grinder::addTarget(std::string targetName)
	{
		this->targetTable.add(TargetType::Name, 0, targetName);
	}

	void Grinder::_gcd()
	{
		Sleep(this->_wait);
	}

	WOWPOS _calcCombatDistanceVector(float sigma, const WOWPOS &vector)
	{
		WOWPOS pos;
		WOWPOS connector;

		/*Instead of calculating convex combination, which can sometimes send the player to a point outside the net,
		we find the triangle containing our destination. Then we take the closest vertex of that triangle,
		and calculate the convex combination between that and the destination vector. A variant of this is to 
		do that, but then travel along the side of the triangle so that we are closer to our original point.*/
		connector = vector - LocalPlayer.pos();
		return LocalPlayer.pos() + sigma * connector;
	}

	bool Grinder::_pathTo(const WOWPOS &vector)
	{
		WOWPOS nbhdVector;

		std::vector<WoWPos> path;
		Thread *walkThread = NULL;

		// Make sure the walk thread isn't already running
		if (_navigator.walkThreadRunning())
		{
			vlog("Walk thread already running");
			return false;
		}


		// ** Pull distance variable needed here.
		// ** Pull rotation also needed to act.
		nbhdVector = _calcCombatDistanceVector(0.8, vector);

		// Vector has to be contained in our net.
		if (!this->isVectorInNet(nbhdVector))
		{
			vlog("Error: target not in net");
			return false;
		}
		else
		{
			// Pathfind to our destination.
			Navigation::Pathfinding pf(this->_net);
			pf.findPath(LocalPlayer.pos(), nbhdVector);

			if (pf.pathFound)
				path = pf.getPath();
			else
			{
				vlog("Error: Unable to find path to target");
				return false;
			}
		}

		// Show path on the radar.
		NetEditor::ShowPath(path);

		// Set up navigator
		_navigator.setRoute(path);
		walkThread = _navigator.createWalkThread(&_navigationLock);
		_thread->addChild(walkThread);
		return true;
	}

	bool Grinder::_onRest()
	{
		return false;
	}

	bool Grinder::_navWait(CObject *obj, double distance)
	{
		// Allow us to check external conditions while walking
		while (/*!_navigator.isDone() && !_navigator.pathWalked() &&*/ _navigator.walkThreadRunning())
		{
			// Update objects location information
			obj->update(LocationInfo);

			// If we're close enough, stop
			if (!this->checkConditions() || obj->distance() <= distance)
			{
				if (_navigationLock.try_lock_for(std::chrono::milliseconds(1000)))
				{
					_navigationLock.unlock();
					vlog("unlocked");

					_thread->clearChildren();
					NetEditor::ClearPath();
					return true;
				}
				else
				{
					// Otherwise, the operator timed out.
					// Terminate the hanging thread.
					vlog("Timeout on walker thread (1000ms)");
					_navigationLock.unlock();
					_navigator.stopThread();
					_thread->clearChildren();
					NetEditor::ClearPath();
					return false;
				}
			}

			Sleep(100);
		}

		return true;
	}

	bool Grinder::_onPull()
	{
		CUnit *target = NULL;
		WOWPOS pullVector;

		std::vector<WOWPOS> path;

		if (this->_enemies.empty() || !this->isNetSet())
		{
			this->_setState(State::Search);
			return false;
		}

		target = &this->_enemies.top();
		target->update(LocationInfo);

		// Pathfind-walk to the pull vector.
		if (this->_pathTo(target->pos()))
		{
			if (_navWait(target, this->_combatDistance))
				this->_setState(State::Combat);
		}
		else
			this->_setState(State::Search);

		return true;
	}

	bool Grinder::_onIdle()
	{
		this->_setState(State::Search);
		return false;
	}

	bool Grinder::_combatRotate(CUnit *target)
	{
		Engine::Action *action = NULL;
		Thread *thread = NULL;

		if (this->combatRotation.size() == 0) return false;

		// Store current target's name.
		target->update(NameInfo);
		
		while (target->isValid() && target->isAlive())
		{
			if (!this->checkConditions())
				return false;

			// Make sure we're within combat distance.
			if (target->distance() > this->_combatDistance)
			{
				// Start navigation thread
				if (this->_pathTo(target->pos()))
					_navWait(target, this->_combatDistance);
			}

			// Attack target.
			if (!LocalPlayer.hasTarget(*target) ||
				!PlayerIsFacingPos(target->pos()))
				target->interact();

			// Update our current target's information.
			target->update(LocationInfo | UnitFieldInfo | NameInfo);

			for (int x = 0; x < combatRotation.size(); x++)
				this->combatRotation.performNextAction();

			this->_gcd();
		}

		return true;
	}

	// Handle combat
	bool Grinder::_onCombat()
	{
		CUnit *target = NULL;

		// If the local player is not in the net, then return.
		if (!this->isUnitInNet(&LocalPlayer))
		{
			vlog("Local player not in net");
			this->_setState(State::Idle);
			return false;
		}

		// If we have no targets, go back to searching.
		if (this->_enemies.empty())
		{
			this->_setState(State::Search);
			return true;
		}

		// Attack each enemy until they are dead.
		while (this->getState() == State::Combat && !this->_enemies.empty())
		{
			target = &this->_enemies.top();

			// If the rotation fails, combat also fails.
			if (!this->_combatRotate(target)) return false;

			this->_enemiesToLoot.push(*target);
			this->_enemies.pop();
		}

		// All targets are dead => loot.
		this->_setState(State::Loot);
		return true;
	}

	bool Grinder::isUnitInNet(CUnit *unit)
	{
		return this->isVectorInNet(unit->pos());
	}

	bool Grinder::isVectorInNet(const WOWPOS &pos)
	{
		if (!this->isNetSet()) return true;
		return this->_net->contains(pos);
	}

	// Attack any other units currently attacking us
	int Grinder::_searchCombat_FindTargets()
	{
		CObject current, next;
		CUnit *unit = NULL;
		DWORD first;

		int count = 0;

		if (!Memory::Read(ObjectManager::GetCurrent() + Offsets::FirstEntry, &first, sizeof(DWORD)))
			return false;

		current.setBase(first);
		current.update(BaseObjectInfo);
		while (current.isValid())
		{
			if (current.isUnit())
			{
				unit = (CUnit *)&current;
				unit->update(UnitFieldInfo | NameInfo);

				// If the unit is in combat and attacking the local player, then flag it as a local enemy.
				if (unit->inCombat() && unit->hasTarget(LocalPlayer))
				{
					vlog("Combat: Found a guy");
					// Perhaps a traceline hit test here
					if (/*unit->distance() < this->_combatDistance || */this->isUnitInNet(unit))
					{
						this->_enemies.push(*unit);
						count++;
					}
				}
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

	// Attack closest target in our table of enemies
	int Grinder::_searchNoCombat_FindTargets()
	{
		CObject current, next;
		CUnit *unit = NULL;
		DWORD first;

		float shortestDistance = 99999;
		CUnit closestUnit;

		int count = 0;

		if (!Memory::Read(ObjectManager::GetCurrent() + Offsets::FirstEntry, &first, sizeof(DWORD)))
			return false;

		current.setBase(first);
		current.update(BaseObjectInfo);
		while (current.isValid())
		{
			if (current.isUnit())
			{
				unit = (CUnit *)&current;
				unit->update(UnitFieldInfo | NameInfo);

				// We shouldn't attack corpses.
				if (unit->isAlive())
				{
					// Check if the unit's faction is in our list of targets.
					if (targetTable.contains(unit->faction())
						|| targetTable.contains(unit->name()))
					{
						unit->update(LocationInfo);
						if (this->isUnitInNet(unit) && unit->distance() < shortestDistance)
						{
							closestUnit = *unit;
							shortestDistance = unit->distance();
						}
					}
				}
			}

			// Set next.base to the next base address in the object manager.
			next.updateBase(current.nextPtr());
			if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
				break;
			else
				current.update(next.object()); // Otherwise, update the current object as the previous next object.
		}

		if (closestUnit.isValid())
		{
			this->_enemies.push(closestUnit);
			count++;
		}

		return count;
	}

	// Search for mobs
	bool Grinder::_onSearch()
	{
		int targetCount = 0;

		// If the local player is not in the net, then return.
		if (!this->isUnitInNet(&LocalPlayer))
		{
			vlog("Local player not in net");
			this->_setState(State::Stop);
			return false;
		}

		if (LocalPlayer.inCombat())
			targetCount = this->_searchCombat_FindTargets();
		else
			targetCount = this->_searchNoCombat_FindTargets();

		if (targetCount > 0)
		{
			vlog("Targets: %d", targetCount);
			this->_setState(State::Pull);
		}
		else
		{
			vlog("No targets");
			this->_setState(State::Idle);
		}

		return true;
	}

	// Walk through the net.
	bool Grinder::_onNavigate()
	{
		if (!this->isNetSet() || !this->isUnitInNet(&LocalPlayer))
		{
			vlog("Local player is not inside the net!");
			this->_setState(State::Stop);
			return false;
		}

		this->_setState(State::Search);
		return true;
	}

	void Grinder::_lootTarget(CUnit &unit)
	{
		this->_setState(Grinder::State::Loot);

		std::vector<int> slots;
		DWORD timeout = 1000;

		// Check if the loot table contains gold.
		bool lootGold = this->lootTable.contains(LootType::Gold);

		if (unit.hasLoot())
		{
			unit.interact();

			// Block until loot window is open, or we tmeout.
			Utilities::Wait lootWindow(Loot::isWindowClosed, 100, 1000);
			if (lootWindow.wait())
			{
				for (int x = 1; x <= Loot::getItemCount(); x++)
				{
					// Check if the current item is in our loot table.
					if ((lootGold && Loot::isSlotCoin(x)) || this->lootTable.contains(Loot::getSlotName(x)))
						slots.push_back(x);

				}

				// Loot all the slots we've marked.
				Loot::slots(slots);
				Loot::close();
				vlog("Looted %s", unit.name());
			}
			else
				vlog("Timed out while waiting for loot window.");
		}
	}

	void Grinder::_clearEnemies()
	{
		while (!this->_enemies.empty())
			this->_enemies.pop();
	}

	bool Grinder::_onLoot()
	{
		std::vector<WOWPOS> path;
		CUnit *add = NULL;

		// Go to each add that we've handled and loot them.
		while (!this->_enemiesToLoot.empty())
		{
			if (!this->checkConditions())
				return false;

			add = &this->_enemiesToLoot.top();

			// Make sure we know their location and loot status.
			add->update(LocationInfo | UnitFieldInfo);
			path.clear();

			// **Add combat check in here somewhere
			// ** Might need to use threads with Walker or something.
			vlog("Walking to add: %s", add->name());

			if (add->distance() >= 5)
			{
				// Path to Add's corpse.
				if (this->_pathTo(add->pos()))
					_navWait(add, this->_combatDistance);

				Sleep(100);
			}

			this->_lootTarget(*add);
			this->_enemiesToLoot.pop();
		}

		this->_setState(State::Search);
		return true;
	}

	bool Grinder::_onDead()
	{
		vlog("Player has died. RIP.");
		this->_setState(State::Stop);
		return true;
	}

	Grinder::State Grinder::getState()
	{
		return this->_state;
	}

	bool Grinder::checkConditions()
	{
		if (LocalPlayer.isDead())
		{
			this->_setState(State::Dead);
			return false;
		}

		return true;
	}

	bool Grinder::fsm()
	{
		_thread = Thread::GetCurrent();

		if (!_thread) return false;

		Thread::eType saveType = _thread->type();

		_thread->setType(Thread::eType::Bot);
		_thread->setState(Thread::eState::Running);
		while (this->_thread->running())
		{
			LocalPlayer.update(UnitFieldInfo | LocationInfo);
			this->checkConditions();

			switch (this->getState())
			{
			case State::Combat:
				vlog("Combat");
				this->_onCombat();
				break;

			case State::Loot:
				vlog("Loot");
				this->_onLoot();
				break;

			case State::Pull:
				vlog("Pull");
				this->_onPull();
				break;

			case State::Rest:
				vlog("Rest");
				this->_onRest();
				break;

			case State::Search:
				vlog("Search");
				this->_onSearch();
				break;

			case State::Navigate:
				vlog("Navigate");
				this->_onNavigate();
				break;

			case State::Dead:
				vlog("Player is dead, stopping");
				this->_onDead();
				break;

			case State::Idle:
				vlog("Idle");
				this->_onIdle();
				/*if (!this->_onIdle())
					current->stop();*/
				break;

			case State::Stop:
				_thread->stop();
				break;
			}

			Sleep(100);
		}

		_thread->setType(saveType);
		return true;
	}

	void Grinder::_addEnemy(CUnit const &unit)
	{
		this->_enemies.push(unit);
	}

	void Grinder::_setState(Grinder::State state)
	{
		this->_state = state;
	}

	void Grinder::setNet(Geometry::Net *net)
	{
		this->_net = net;
	}

	void Grinder::setGCD(uint wait)
	{
		this->_wait = wait;
	}

	bool Grinder::isNetSet()
	{
		return this->_net != NULL
			&& this->_net->order() > 0;
	}

	Grinder::Grinder()
	{
		this->_target = LocalPlayer.getTarget();
		this->_state = Grinder::State::Navigate;
		this->_combatDistance = 5;
		this->_wait = 1000;

		this->_navigator.setWait(50);
		this->_navigator.setTimeout(10000);
		//this->_navigator.setTolerance(2);
	}

	Grinder::Grinder(float combatDistance)
	{
		this->_target = LocalPlayer.getTarget();
		this->_combatDistance = combatDistance;
		this->_state = Grinder::State::Navigate;
		this->_wait = 1000;

		this->_navigator.setWait(50);
		this->_navigator.setTimeout(10000);
		//this->_navigator.setTolerance(2);
	}

	Grinder::~Grinder()
	{

	}

};