#include "Condition.h"
#include "WoW API\Object Classes\ObjectManager.h"
#include "Utilities\Utilities.h"

#include <algorithm>
#include <Game\Game.h>

namespace Engine
{
	
	//COND_HEALTH_POWER		= 0x0,
	// Options:
		// Check: Player, pet, target
		// Type: Health, mana, energy, rage, runic power
		// Condition: <, =, >.

	typedef bool(*Order)(float *, float);

	bool Order_equalTo(float *x, float y)
	{
		return *x == y;
	}

	bool Order_moreThan(float *x, float y)
	{
		return *x > y;
	}

	bool Order_lessThan(float *x, float y)
	{
		return *x < y;
	}

	bool Condition::handlePower()
	{
		Option *option;
		CUnit unit;

		uint *measure = NULL, *maxMeasure = NULL;

		float value = 0;
		float buffer = 0;
		float *result = NULL;

		std::string text;

		void *ordering = NULL;

		// Get measure and object options.
		for (int x = 0; x < this->_options.size(); x++)
		{
			option = &this->_options[x];
			switch (option->type)
			{
				// Check
			case OPTION_CHECK_PLAYER:
				unit.set(LocalPlayer.object());
				break;

			case OPTION_CHECK_TARGET:
				if (LocalPlayer.hasNoTarget()) return false;

				unit.set(LocalPlayer.getTarget().object());
				break;

			case OPTION_CHECK_PET:
				break;

			case OPTION_TYPE_HEALTH:
				measure = &unit.Obj.UnitField.Health;
				maxMeasure = &unit.Obj.UnitField.Max_Health;
				break;

			case OPTION_TYPE_POWER:
				measure = &unit.Obj.UnitField.Power;
				maxMeasure = &unit.Obj.UnitField.Max_Power;
				break;
			}
		}

		if (!measure || !maxMeasure)
		{
			vlog("Invalid options chosen");
			return false;
		}

		// Get order and value options.
		for (int x = 0; x < this->_options.size(); x++)
		{
			option = &this->_options[x];
			switch (option->type)
			{
			case OPTION_VALUE_BY_PERCENT:
				value = option->value;
				buffer = ((float)*measure / (float)*maxMeasure) * 100;
				result = &buffer;
				break;

			case OPTION_VALUE_BY_NUMBER:
				result = (float *)measure;
				break;

			case OPTION_ORDER_LESS:
				ordering = &Order_lessThan;
				break;

			case OPTION_ORDER_EQUAL:
				ordering = &Order_equalTo;
				break;

			case OPTION_ORDER_MORE:
				ordering = &Order_moreThan;
				break;
			}
		}

		// This might reset the Object and invalidate the pointers.
		// Look into way to make sure this doesn't happen.
		// = DGetObjectEx could be made into out parameter.
		if (!measure || !maxMeasure || !ordering)
		{
			vlog("Invalid options chosen");
			return false;
		}

		unit.update(UnitFieldInfo);

		// Return unit's power type <,=,> the given value.
		return ((Order)ordering)(result, value);
	}

	bool Condition::handleBuffDetect()
	{

		return false;
	}

	bool Condition::handleComboPoints()
	{
		void  *ordering = NULL;
		float *points = NULL;

		float buffer = 0;
		float value = 0;

		CUnit &target = LocalPlayer.getTarget();

		if (!target.isValid())
		{
			vlog("No target!");
			return false;
		}

		buffer = WoW::GetComboPoints(target.guid());

		Option *option = NULL;
		for (int x = 0; x < this->_options.size(); x++)
		{
			option = &this->_options[x];
			switch (option->type)
			{
			case OPTION_VALUE_BY_NUMBER:
				value = option->value;
				break;

			case OPTION_CHECK_PLAYER:
				break;

			case OPTION_ORDER_LESS:
				ordering = &Order_lessThan;
				break;

			case OPTION_ORDER_EQUAL:
				ordering = &Order_equalTo;
				break;

			case OPTION_ORDER_MORE:
				ordering = &Order_moreThan;
				break;
			}
		}

		points = &buffer;
		if (!ordering || !points)
		{
			vlog("Invalid options chosen");
			return false;
		}

		// Return unit's power type <,=,> the given value.
		return ((Order)ordering)(points, value);
		return false;
	}

	bool Condition::handleDistanceToTarget()
	{
		float *distance = NULL;
		void  *ordering = NULL;

		float buffer = 0;
		float value = 0;

		CUnit *target = NULL;
		target = &LocalPlayer.getTarget();

		if (!target)
		{
			vlog("No target!");
			return false;
		}

		target->update(LocationInfo);

		Option *option = NULL;
		for (int x = 0; x < this->_options.size(); x++)
		{
			option = &this->_options[x];
			switch (option->type)
			{
			case OPTION_VALUE_BY_NUMBER:
				value = option->value;
				break;

			case OPTION_CHECK_PLAYER:
				buffer = target->distance();
				distance = &buffer;
				break;

			case OPTION_CHECK_PET:
				break;

			case OPTION_ORDER_LESS:
				ordering = &Order_lessThan;
				break;

			case OPTION_ORDER_EQUAL:
				ordering = &Order_equalTo;
				break;

			case OPTION_ORDER_MORE:
				ordering = &Order_moreThan;
				break;
			}
		}

		if (!ordering || !distance)
		{
			vlog("Invalid options chosen");
			return false;
		}


		// Return unit's power type <,=,> the given value.
		return ((Order)ordering)(distance, value);
	}


	bool Condition::check()
	{
		switch (this->condition)
		{
		case CONDITION_POWER:
			return this->handlePower();
			break;

		case CONDITION_DISTANCE_TO_TARGET:
			return this->handleDistanceToTarget();
			break;

		case CONDITION_COMBO_POINTS:
			return this->handleComboPoints();
			break;

			// Unfinished
		case CONDITION_BUFF_DETECT:
			return this->handleBuffDetect();
			break;

		case CONDITION_TIMER:
			break;

		case CONDITION_RUNE:
			break;
						
		case CONDITION_PET_ACTIVE:
			break;
		}

		return false;
	}

	void Condition::addOption(eOption eO, float value, std::string const &text)
	{
		Option op = {eO, value, text};
		this->_options.push_back(op);
	}

	void Condition::addOption(eOption eO, std::string const &text)
	{
		this->addOption(eO, 0, text);
	}

	void Condition::addOption(eOption eO, float value)
	{
		this->addOption(eO, value, "");
	}

	void Condition::addOption(eOption eO)
	{
		this->addOption(eO, 0, "");
	}

	void Condition::removeLastOption()
	{
		this->_options.pop_back();
	}

	void Condition::removeOption(eOption find)
	{
		Option *current = NULL;

		for (int x = 0; x < this->_options.size(); x++)
		{
			current = &this->_options[x];
			if (current)
			{
				// If the current option is the same as the given, delete it.
				if (current->type == find)
				{
					_options.erase(_options.begin() + x);
					return;
				}
			}
		}
	}

	Condition::Condition(eCondition c)
	{
		this->condition = c;
	}

	Condition::~Condition()
	{

	}

};