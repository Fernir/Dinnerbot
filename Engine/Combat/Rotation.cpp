#include "Rotation.h"

namespace Engine
{
	void Rotation::addAction(Action const &action)
	{
		this->_actions.push_back(action);
	}

	void Rotation::clear()
	{
		this->_actions.clear();
	}

	uint Rotation::size()
	{
		return this->_actions.size();
	}

	void Rotation::reset()
	{
		this->_currentAction = 0;
	}

	Action *Rotation::next()
	{
		if (this->size() == 0) return NULL;

		if (this->_currentAction >= this->size()) this->_currentAction = 0;

		return &this->_actions[this->_currentAction++];
	}

	bool Rotation::performNextAction()
	{
		Action *action = NULL;
		action = this->next();

		if (!action)
			return false;

		// Consider action's conditions.
		if (action->shouldPerform())
			if (action->perform()) 
				return true;

		return true;
	}


	Rotation::Rotation()
	{
		this->_currentAction = 0;
		this->_actions.reserve(6);
	}

	Rotation::~Rotation()
	{

	}
};
