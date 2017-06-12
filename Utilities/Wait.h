#pragma once

#include <Main\Structures.h>

namespace Utilities
{
	class Wait
	{

	public:
		typedef bool (wait_conditional)(void);
		typedef bool (wait_conditional_param)(void *);

		ullong Wait::getElapsed();

		bool wait();

		bool checkCondition();
		void setTime(unsigned int time);
		void setTimeout(unsigned int timeout);

		Wait(wait_conditional address, unsigned int time, unsigned int _timeout);
		Wait(wait_conditional_param address, void *param, unsigned int time, unsigned int _timeout);
		~Wait();

	private:
		wait_conditional *_waitCondition;
		wait_conditional_param *_waitCondition_param;
		ullong _waitTime;
		ullong _timeout;

		ullong _waitElapsed;
		void *_param;
	};
}

