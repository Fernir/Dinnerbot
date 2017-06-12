#pragma once

#include "Main\Structures.h"
#include <vector>

#include "Action.h"

namespace Engine
{
	class Rotation
	{
	private:
		std::vector<Action> _actions;
		uint _currentAction;

	public:

		void addAction(Action const &action);

		void reset();
		void clear();

		bool performNextAction();

		Action *next();
		uint size();

		Rotation();
		~Rotation();
	};
}

