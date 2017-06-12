#include "Walker.h"

#include "Main\Main.h"
#include "ClickToMove.h"

#include <Utilities\Utilities.h>

using namespace Engine;
using namespace Navigation;

ULONGLONG Walker::_calcWalkTime(float dist)
{
	return dist / this->_speed;
}

void walk_wrapper(void *param)
{
	Walker *walker = (Walker *)param;

	if (walker)
		walker->walk();
}

Thread *Walker::createWalkThread(std::timed_mutex *m)
{
	this->_selfThread = true;
	this->_mutex = m;
	this->_thread = Thread::Create(walk_wrapper, this, Thread::eType::Navigation);
	return this->_thread;
}

Thread *Walker::getThread()
{
	return this->_thread;
}

void Walker::stopThread()
{
	if (this->_thread)
		this->_thread->stop();
}

void Walker::walk()
{
	float dist = 0;

	ULONGLONG start = GetTickCount64();

	// Reset status
	_state = State::Walking;

	if (this->_mutex)
	{
		// Make sure we can lock the given navigation mutex.
		if (!this->_mutex->try_lock())
		{
			this->_cleanupThread();
			return;
		}
	}

	// Walk through each point in the route.
	for (; _counter < _route.size(); _counter++)
	{
		while (this->_checkThread() 
			&& (dist = _unit->distance(_route[_counter])) > _tolerance)
		{
			move(_route[_counter]);

			// Wait until we can move again.
			while (this->_checkThread() 
				&& !Navigation::idle())
			{
				// If the timeout has been reached without any change in movement status, return.
				if (this->_timeout > 0 && GetTickCount64() - start > this->_timeout)
				{
					/*if (this->_speed > 0 && this->_calcWalkTime(dist))
					{
					}*/
					_counter = 0;
					_state = Arrived;
					this->_cleanupThread();
					return;
				}

				Sleep(_wait);
			}
		}
	}

	// Set status to indicate that we've finished.
	_state = State::Arrived;
	this->_cleanupThread();
}

bool Walker::_cleanupThread()
{
	if (this->_selfThread && this->_thread)
	{
		this->_thread->exit();
		this->_thread = NULL;
		if (this->_mutex)
			this->_mutex->unlock();

		return true;
	}

	return false;
}

bool Walker::_checkThread()
{
	if (this->_thread == NULL)
	{
		if (this->_selfThread) return false;
		return true;
	}

	return this->_thread->running();
}

bool Walker::pathWalked()
{
	return this->_counter == this->_route.size();
}

bool Walker::walkThreadRunning()
{
	return this->_selfThread && this->_thread != NULL;
}

bool Walker::isDone()
{
	return this->_state == State::Arrived;
}

void Walker::setSpeed(float speed)
{
	this->_speed = speed;
}

void Walker::setTolerance(float tol)
{
	this->_tolerance = tol;
}

void Walker::setTimeout(ULONGLONG  timeout)
{
	this->_timeout = timeout;
}

void Walker::setWait(ULONGLONG  wait)
{
	this->_wait = wait;
}

void Walker::setRoute(std::vector<WoWPos> &route)
{
	this->_route = route;
}

// _timeout = 0 => no timeout
// _speed = 0 => no speed check
void Walker::_init(uint counter, ULONGLONG  timeout, ULONGLONG  wait, std::vector<WoWPos> &route, CUnit *unit, State state, Thread *thread, float tol, float speed)
{
	this->_counter = counter;
	this->_timeout = timeout;
	this->_wait = wait;
	this->_route = route;
	this->_unit = unit;
	this->_state = state;
	this->_thread = thread;
	this->_tolerance = tol;
	this->_speed = speed;
	this->_selfThread = false;
	this->_mutex = NULL;
}

void Walker::_init(uint counter, ULONGLONG  timeout, ULONGLONG  wait, CUnit *unit, State state, Thread *thread, float tol, float speed)
{
	this->_counter = counter;
	this->_timeout = timeout;
	this->_wait = wait;
	this->_unit = unit;
	this->_state = state;
	this->_thread = thread;
	this->_tolerance = tol;
	this->_speed = speed;
	this->_selfThread = false;
	this->_mutex = NULL;
}

Walker::Walker(std::vector<WoWPos> &route, Thread *thread)
{
	this->_init(0, 0, 200, route, &LocalPlayer, State::Initialize, thread, 1, 0);
}

Walker::Walker(std::vector<WoWPos> &route)
{
	this->_init(0, 0, 200, route, &LocalPlayer, State::Initialize, NULL, 1, 0);
}

Walker::Walker(WoWPos &route)
{
	this->_route.push_back(route);
	this->_init(0, 0, 200, &LocalPlayer, State::Initialize, NULL, 1, 0);
}

Walker::Walker() 
{
	this->_init(0, 0, 200, &LocalPlayer, State::Initialize, NULL, 1, 0);
}

/*Walker::Walker(const WOWPOS &dest, Thread *thread)
{
	this->_init(0, 2000, 200, route, &LocalPlayer, false, thread, 1);
}

Walker::Walker(const WOWPOS &dest)
{
	this->_init(0, 2000, 200, route, &LocalPlayer, false, NULL, 1);
}*/

Walker::~Walker()
{
	// Only delete _thread if we've created it.
	//this->_cleanupThread();
	if (this->walkThreadRunning())
	{
		vlog("ERROR: Walker thread still running on destruct");
	}
}