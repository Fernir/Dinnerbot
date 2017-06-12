#pragma once

#include <Windows.h>
#include <vector>
#include <thread>

class Thread
{
public:

	//Thread Types
	enum eType
	{
		Bot,
		GUI,
		Misc,
		Navigation,
		InfoLoop,
		Command,
		Chat,
		Key,
		Multibox,
		Memory,
	};

	//Thread Priorities
	enum ePriority
	{
		Low,
		Medium,
		High,
	};

	//BOT Thread Tasks
	enum eTask
	{
		// General
		Initialize,
		Stopped,
		None,
		Idle,

		// *** Bot Tasks
		Gathering,
		Fishing,

		// *** Engine Tasks

		// Navigation
		Moving,
		Rotating,

		// Combat
		Attacking,

		// Command
		FlagGrab,
		SSClick,
	};

	//Thread States
	enum eState
	{
		Running,
		Paused,
		Stopping,
		Suspended,
		Terminated,
	};

private:
	DWORD		_Id;
	eTask		_task;
	eType		_type;
	eState		_state;
	ePriority	_priority;
	HANDLE		_handle;

	std::thread *_thread;
	
	VOID *_parameter;
	VOID *_address;

	VOID _flagForRemoval();

	std::vector<Thread *> _children;
public:
	// Static variables.
	static std::vector<Thread *> Threads;

	// Static functions.
	static VOID Delete(Thread *thread, const char *moduleName);
	static VOID Delete(Thread *thread);
	static VOID DeleteCurrent(const char *moduleName);
	static VOID DeleteCurrent();

	static BOOL Wait(Thread *t);

	static Thread *GetCurrent();
	static Thread *Find(DWORD threadId);
	static Thread *FindType(eType type);
	static Thread *Create(void *address, void *parameter);
	static Thread *Create(void *address, void *parameter, eType type);
	static Thread *Create(void *address, void *parameter, eType type, ePriority priority);

	// Info functions.

	DWORD  Id()			{ return this->_Id;			}
	eTask  task()		{ return this->_task;		}
	eType  type()		{ return this->_type;		}
	eState  state()		{ return this->_state;		}
	ePriority  priority()	{ return this->_priority;	}

	VOID *parameter()		{ return this->_parameter;	}
	VOID *address()			{ return this->_address;	}

	HANDLE threadHandle()	{ return this->_handle;		}

	// Status function.
	bool running();
	bool stopped();

	// Use functions.
	/*bool kill();
	void killChildren();
	*/

	bool kill();
	VOID stop();
	VOID stopChildren();

	void removeChild(Thread *child);
	void addChild(Thread *child);
	void clearChildren();

	VOID exit();
	VOID exit(const char *moduleName);

	VOID setType(eType type);
	VOID setTask(eTask state);
	VOID setState(eState state);
	VOID setPriority(ePriority state);

	// Constructors and destructors.
	Thread(VOID *address, VOID *parameter, eType threadType, ePriority threadPriority);
	Thread(VOID *address, VOID *parameter);
	Thread(VOID *address);
	Thread();
	~Thread();
	
	bool operator ==(const Thread &t) const
	{
		return (_address == t._address && _Id == t._Id);
	}
};

extern std::vector<Thread *> Threads;

inline bool operator ==(Thread &lhs, Thread &rhs)
{
	return lhs.address() == rhs.address() && lhs.Id() == rhs.Id();
}