#include "Action.h"
#include "WoW API\Spell.h"
#include <Utilities\Utilities.h>

namespace Engine
{
	// Determine if we should perform the current action.
	bool Action::shouldPerform()
	{
		// Consider each condition.
		for (auto &cond : this->_conditions)
		{
			// Check if the condition holds.
			if (!cond.check())
			{
				return false;
			}
		}

		return true;
	}

	// Perform whatever action this is.
	bool Action::perform()
	{
		CSpell *spell = NULL;

		switch (this->type)
		{
			case Type::SpellId:
				spell = new CSpell(this->value);
				if (spell->canCast() && spell->targetInRange())
				{
					spell->cast();
					vlog("Casted spell: %d", spell->getId());
				}

				delete spell;
				return true;

			case Type::SpellName:
				spell = new CSpell(this->_name.c_str());
				if (spell->canCast() && spell->targetInRange())
				{
					spell->cast();
					vlog("Casted spell: %s (%d)", spell->toString().c_str(), spell->getId());
				}

				delete spell;
				return true;
		}

		return false;
	}

	void Action::addCondition(const Condition &condition)
	{
		_conditions.push_back(condition);
	}

	void Action::clearConditions()
	{
		_conditions.clear();
	}

	std::string Action::toString()
	{
		return this->_name;
	}

	Action::Action(std::string const &text, Condition const &condition)
	{
		this->type = Type::SpellName;
		this->_name = text;

		this->addCondition(condition);
	}
	
	Action::Action(std::string const &text, Action::Type type)
	{
		this->type = type;
		this->_name = text;
	}

	Action::Action(const uint val, Action::Type type)
	{
		this->type = type;
		this->value = val;
	}

	Action::Action(std::string const &text)
	{
		this->type = Type::SpellName;
		this->_name = text;
	}

	Action::Action(const uint text)
	{
		this->type = Type::SpellId;
		this->value = text;
		this->_name = "";
	}

	Action::Action()
	{
	}

	Action::~Action()
	{

	}

};
