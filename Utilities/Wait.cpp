#include "Wait.h"

#include <Windows.h>

using namespace Utilities;

Wait::Wait(wait_conditional *address, unsigned int waitTime, unsigned int timeout)
{
	this->setTime(waitTime);
	this->setTimeout(timeout);
	this->_waitCondition = address;
	this->_waitElapsed = 0;

	this->_param = NULL;
	this->_waitCondition_param = NULL;
}

Wait::Wait(wait_conditional_param *address, void *param, unsigned int waitTime, unsigned int timeout)
{
	this->setTime(waitTime);
	this->setTimeout(timeout);
	this->_waitCondition = NULL;
	this->_waitElapsed = 0;

	this->_param = param;
	this->_waitCondition_param = address;
}

void Wait::setTime(unsigned int time)
{
	this->_waitTime = time;
}
void Wait::setTimeout(unsigned int timeout)
{
	this->_timeout = timeout;
}

bool Wait::checkCondition()
{
	if (this->_waitCondition_param)
		return this->_waitCondition_param(this->_param);

	else if (this->_waitCondition)
		return this->_waitCondition();

	return false;
}

ullong Wait::getElapsed()
{
	return this->_waitElapsed;
}

bool Wait::wait()
{
	ullong elapsed = 0;
	ullong start = GetTickCount64();

	while (this->checkCondition())
	{
		// Timed out
		if (elapsed >= this->_timeout)
		{
			_waitElapsed = elapsed;
			return false;
		}

		// Wait on condition.
		Sleep(this->_waitTime);
		elapsed = GetTickCount64() - start;
	}

	// Condition returned.
	_waitElapsed = elapsed;
	return true;
}

Wait::~Wait()
{
}
