#pragma once

#include "Main\Structures.h"
#include "WoW API\Spell.h"
#include "Condition.h"

#include <string>
#include <vector>

namespace Engine
{

	class Action
	{

	public:
		enum Type
		{
			// In case we have different types of actions
			SpellName,
			SpellId,
		};

		uint type;
		uint value;

		bool shouldPerform();
		bool perform();

		void addCondition(Condition const &condition);
		void clearConditions();

		std::string toString();

		Action(std::string const &text, Condition const &condition);
		Action(std::string const &text, Action::Type type);
		Action(std::string const &text);
		Action(const uint val, Action::Type type);
		Action(uint const id);
		Action();
		~Action();

	private:
		Type _type;
		std::string _name;
		std::vector<Condition> _conditions;
	};
}