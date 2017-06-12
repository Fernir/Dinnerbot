#pragma once

#include "Main\Structures.h"
#include "WoW API\Object Classes\ObjectManager.h"

#include <Engine\Navigation\Walker.h>
#include <Engine\Combat\Rotation.h>
#include <Utilities\Thread.h>
#include <Geometry\Net.h>
#include <queue>
#include <stack>
#include <mutex>

namespace Engine
{
	class Grinder
	{

	public:
		enum State
		{
			Navigate,
			Combat,
			Search,
			Rest,
			Pull,
			Idle,
			Loot,
			Dead,
			Stop,
		};

		//enum CombatState { Attack, AdjustPosition,  };

		template <class T>
		class EntityTable
		{
		public:
			EntityTable()  {}
			~EntityTable() {}

			typedef struct
			{
				T type;
				DWORD val;
				std::string name;
			} Entry;

			void add(T type, DWORD val, const std::string &name) { this->_table.push_back({ type, val, name }); }
			void clear()							  { this->_table.clear(); }

			bool contains(std::string name)
			{
				for (auto &entry : _table)
				{
					if (entry.name == name)
						return true;
				}

				return false;
			}

			bool contains(DWORD val)
			{
				for (auto &entry : _table)
				{
					if (entry.val == val)
						return true;
				}

				return false;
			}

			bool contains(T type)
			{
				for (auto &entry : _table)
				{
					if (entry.type == type)
						return true;
				}

				return false;
			}



		private:
			std::vector<Entry> _table;
		};

		/*class TargetTable
		{
		public:
			TargetTable()  {}
			~TargetTable() {}

			enum Type
			{
				Name,
				Faction,
			};

			typedef struct
			{
				TargetTable::Type type;
				std::string name;
			} Item;

			void add(const std::string &name) { this->_table.push_back({ Type::Name, name }); }
			void add(LootTable::Type type)	  { this->_table.push_back({ type, "" }); }
			void clear()					  { this->_table.clear(); }

			bool contains(std::string name)
			{
				for (auto &item : _table)
				{
					if (item.name == name)
						return true;
				}

				return false;
			}

			bool contains(LootTable::Type type)
			{
				for (auto &item : _table)
				{
					if (item.type == type)
						return true;
				}

				return false;
			}


		private:
			std::vector<Item> _table;
		};
		*/

		enum LootType { ItemName, ItemQuality, Gold, AllItems };
		enum TargetType { Name, Display, Faction, All };

		EntityTable<TargetType> targetTable;
		EntityTable<LootType> lootTable;
		Rotation combatRotation;
		Rotation pullRotation;
		Rotation restRotation;

		uint addCount() { return this->_enemies.size(); }

		bool updateAdds();
		CUnit *getNextTarget();

		void addTarget(int faction);
		void addTarget(std::string targetName);
		void addTarget(TargetType type, int value);

		void addLoot(std::string lootName);
		void addLoot(LootType type);
		void clearLootTable();

		Grinder::State getState();
		bool fsm();

		void setNet(Geometry::Net *net);
		void setGCD(uint);

		bool isActive();
		bool isNetSet();
		bool isUnitInNet(CUnit *unit);
		bool isVectorInNet(const WOWPOS &);

		Grinder(float combatDistance);
		Grinder();
		~Grinder();

	private:
		uint _wait;
		CUnit _target;
		std::priority_queue<CUnit, std::vector<CUnit>, LessThanByLocalDistance> _enemies;
		std::stack<CUnit> _enemiesToLoot;

		Navigation::Walker _navigator;
		std::timed_mutex _navigationLock;
		Thread *_thread;

		float _combatDistance;

		State _state;

		Geometry::Net *_net;

		bool checkConditions();

		bool _navWait(CObject *obj, double distance);

		void _gcd();

		void _addEnemy(CUnit const &unit);

		void _setState(Grinder::State state);

		// Navigation
		bool _pathTo(const WOWPOS &vector);

		// Combat
		bool _combatRotate(CUnit *target);

		// Loot
		void _lootTarget(CUnit &unit);

		// 
		void _clearEnemies();

		// Search related members
		int _searchNoCombat_FindTargets();
		int _searchCombat_FindTargets();

		// Finite state machine states:
		bool _onNavigate();
		bool _onCombat();
		bool  _onSearch();
		bool _onRest();
		bool _onPull();
		bool _onBuff();
		bool _onIdle();
		bool _onLoot();
		bool _onDead();
	};

};
