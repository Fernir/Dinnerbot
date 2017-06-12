#pragma once

#include <Windows.h>
#include <vector>

#include <Windows.h>
#include "Main\Structures.h"
#include "Utilities\Thread.h"
#include "WoW API\Object Classes\Unit.h"

#include <mutex>

namespace Engine
{
	namespace Navigation
	{
		class Walker
		{
		public:
			enum State
			{
				Initialize,
				Walking,
				Arrived,
			};

		private:
			std::vector<WoWPos> _route;
			uint _counter;
			CUnit *_unit;

			// Status
			std::timed_mutex *_mutex;
			State _state;
			bool _selfThread;
			Thread *_thread;

			// Settings
			float _tolerance;
			float _speed;
			ULONGLONG _timeout;
			ULONGLONG  _wait;

			// Private member functions
			bool _checkThread();
			bool _cleanupThread();
			ULONGLONG _calcWalkTime(float dist);
			void _init(uint counter, ULONGLONG  timeout, ULONGLONG  wait, std::vector<WoWPos> &route, CUnit *unit, State state, Thread *thread, float tol, float speed);
			void _init(uint counter, ULONGLONG  timeout, ULONGLONG  wait, CUnit *unit, State state, Thread *thread, float tol, float speed);

		public:
			void walk();
			bool isDone();
			bool pathWalked();
			Thread *createWalkThread(std::timed_mutex *m);
			bool walkThreadRunning();

			void setRoute(std::vector<WoWPos> &route);
			void setTimeout(ULONGLONG  timeout);
			void setWait(ULONGLONG  wait);
			void setTolerance(float tol);
			void setSpeed(float speed);
			void stopThread();

			Thread *getThread();

			Walker(std::vector<WoWPos> &route, Thread *thread);
			Walker(std::vector<WoWPos> &route);
			Walker(WoWPos &route);
			Walker();
			~Walker();
		};
	}
}

