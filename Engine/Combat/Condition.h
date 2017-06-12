#pragma once

#include <string>
#include <vector>

namespace Engine
{
	enum eOptionType
	{
		OPTION_CHECK,
		OPTION_TYPE ,
		OPTION_VALUE, 
		OPTION_ORDER,
	};

	enum eOption
	{
		// Unit to check condition on.
		OPTION_CHECK_PLAYER		,
		OPTION_CHECK_TARGET		,
		OPTION_CHECK_PET		,

		// Type of condition option to check.
		OPTION_TYPE_HEALTH		,
		OPTION_TYPE_POWER		,
		OPTION_TYPE_RUNIC_POWER	,

		// Subcondition to check.
		OPTION_VALUE_BY_NAME	,
		OPTION_VALUE_BY_ID		,
		OPTION_VALUE_BY_NUMBER  ,
		OPTION_VALUE_BY_PERCENT ,
		OPTION_VALUE_BY_TIME	,

		// Value is <, =, or >.
		OPTION_ORDER_LESS		,
		OPTION_ORDER_EQUAL		,
		OPTION_ORDER_MORE		,

		// Distance to target yards value.
		OPTION_DISTANCE_YARDS	,

		// Timer condition options.
		OPTION_TIMER_READY		,
		OPTION_TIMER_NOT_READY	,

		// Runes condition options.
		OPTION_RUNE_TYPE		,

		// Pet condition options.
		OPTION_PET_ACTIVE		,
		OPTION_PET_INACTIVE		,
	};

	enum eCondition
	{
		CONDITION_POWER,
		// Options:
			// Check: Player, pet, target
			// Type: Health, mana, energy, rage, runic power
			// Condition: <, =, >.
			// Value in %: 0-100, actual value.

		CONDITION_BUFF_DETECT,
		// Options:
			// Check: player, pet, target
			// Condition: Has buff, does not have buff
			// Value: By name, by spell id.

		CONDITION_DISTANCE_TO_TARGET,
		// Options:
			// Distance is: <, =, >.
			// Yards: *

		CONDITION_TIMER,
		// Options:
			// Check: is ready, not ready
			// Time: in miliseconds

		// Class-specific conditions.
		CONDITION_COMBO_POINTS,
		// Options:
			// Combo Points: <, =, >.
			// value: 0-5

		CONDITION_RUNE,
		// Options: 
			// Player has: <, =, >
			// Value: 1
			// Rune(s): Blood, Frost, Unholy

		CONDITION_PET_ACTIVE,
		// Options:
			// Condition: Has pet, does not have pet.
	};

	struct Option
	{
		eOption type;
		float value;

		std::string text;
	};


	class Condition
	{
	private:
		std::vector<Option> _options;

	public:
		eCondition condition;

		bool check();
		// Conditions checks.
		bool handlePower();
		bool handleBuffDetect();
		bool handleComboPoints();
		bool handleDistanceToTarget();

		void addOption(eOption option, float value, std::string const &text);
		void addOption(eOption option, std::string const &text);
		void addOption(eOption option, float value);
		void addOption(eOption);

		void removeOption(eOption);
		void removeLastOption();

		Condition(eCondition);
		~Condition();
	};

	inline bool operator ==(Condition &lhs, eOption rhs)
	{
		return lhs.condition == rhs;
	}

	inline bool operator ==(eOption lhs, Condition &rhs)
	{
		return rhs.condition == lhs;
	}
}
