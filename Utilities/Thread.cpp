#include "Thread.h"
#include "Utilities.h"

#include <vector>
#include <algorithm>

std::vector<Thread *> Thread::Threads;

/****** Static Thread class functions. ******/

Thread *Thread::GetCurrent()
{
	DWORD threadId = GetCurrentThreadId();
	return Thread::Find(threadId);
}

VOID Thread::DeleteCurrent()
{
	Thread *t = Thread::GetCurrent();
	Thread::Delete(t);
}

VOID Thread::DeleteCurrent(const char *moduleName)
{
	vlog("[THREAD]: %s DeleteCurrent()", moduleName);
	Thread::DeleteCurrent();
}

VOID Thread::Delete(Thread *thread, const char *moduleName)
{
	vlog("[THREAD]: %s DeleteCurrent()", moduleName);
	Thread::Delete(thread);
}

VOID Thread::Delete(Thread *thread)
{
	std::vector<Thread *>::iterator it;

	if (thread)
	{
		// Flag thread for removal.
		thread->_flagForRemoval();

		// Find and erase thread from Threads list.
		it = std::remove(Threads.begin(), Threads.end(), thread);
		Threads.erase(it);

		// Deallocate thread from memory.
		delete thread;
	}
}

bool Thread::kill()
{
	return TerminateThread(this->_handle, NULL);
}

// Return if the thread is still running.
BOOL Thread::Wait(Thread *t)
{
	return t->state() == eState::Running;
}

// Find a thread with Id.
Thread *Thread::Find(DWORD threadId)
{
	Thread *thread = NULL;

	for (int x = 0; x < Threads.size(); x++)
	{
		thread = Threads[x];
		if (thread->Id() == threadId)
		{
			// This returns a valid pointer since the Threads vector is static.
			return thread;
		}
	}

	return NULL;
}

Thread *Thread::FindType(eType type)
{
	Thread *thread = NULL;

	for (int x = 0; x < Threads.size(); x++)
	{
		thread = Threads[x];
		if (thread->type() == type)
		{
			// This returns a valid pointer since Threads is static.
			return thread;
		}
	}

	return NULL;
}

Thread *Thread::Create(void *address, void *parameter, eType type, ePriority priority)
{
	Thread *t = new Thread(address, parameter, type, priority);

	Threads.push_back(t);
	return t;
}

Thread *Thread::Create(void *address, void *parameter, eType type)
{
	return Create(address, parameter, type, ePriority::Medium);
}

Thread *Thread::Create(void *address, void *parameter)
{
	return Create(address, parameter, eType::Misc, ePriority::Medium);
}

/****** Nonstatic Thread class functions. ******/

bool Thread::running()
{
	return this->state() == eState::Running;
}

bool Thread::stopped()
{
	return this->state() != eState::Running;
}

VOID Thread::setState(eState state)
{
	this->_state = state;
}

VOID Thread::setType(eType type)
{
	this->_type = type;
}

VOID Thread::setPriority(ePriority priority)
{
	this->_priority = priority;
}

VOID Thread::setTask(eTask task)
{
	this->_task = task;
}

// Stop execution of thread.
VOID Thread::stop()
{
	this->setState(eState::Stopping);
	//this->stopChildren();
}

VOID Thread::stopChildren()
{
	if (this->_children.empty()) return;
	for (auto &each : this->_children)
	{
		if (each && each->running())
			each->stop();
	}
}

void Thread::addChild(Thread *child)
{
	if (!child) return;

	this->_children.push_back(child);
}

void Thread::clearChildren()
{
	this->_children.clear();
}

void Thread::removeChild(Thread *child)
{
	if (!child) return;

	for (int x = 0; x < this->_children.size(); x++)
	{
		if (*_children[x] == *child)
		{
			this->_children.erase(_children.begin() + x);
		}
	}
}

// Indicate that the thread will be removed.
VOID Thread::_flagForRemoval()
{
	this->setState(eState::Terminated);
	this->setTask(eTask::None);
}

// We do it this way because calling delete this is weird.
VOID Thread::exit(const char *moduleName)
{
	Thread::Delete(this, moduleName);
}

// Stop execution of thread and remove it from our vector.
// Call this before the function return.
VOID Thread::exit()
{
	Thread::Delete(this);
}

Thread::Thread(void *address, void *parameter, eType threadType, ePriority threadPriority)
{
	// Initialize our private variables.
	this->_address = address;
	this->_parameter = parameter;

	// Create a new thread with the given parameters.
	this->_handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)address, parameter, NULL, &this->_Id);

	// If the thread was successfully created, push it onto memory.
	if (this->_handle)
	{
		// Indicate the thread is now running.
		this->setState(eState::Running);

		// Set the thread type and priority.
		this->_type = threadType;
		this->setPriority(threadPriority);

		// Close the thread to prevent a memory leak.
		CloseHandle(this->_handle);
	}

	// Otherwise, the destructor will take care of it.
}

Thread::Thread(void *address, void *parameter)
{
	*this = Thread(address, parameter, eType::Misc, ePriority::Medium);
}

Thread::Thread(void)
{
	this->_address = 0;
	this->_Id = 0;

	this->_task = eTask::None;
	this->_type = eType::Misc;
	this->_state = eState::Running;
	this->_priority = ePriority::Medium;
	this->_parameter = NULL;
	this->_handle = NULL;
}

Thread::~Thread()
{
	// But the destructor is dumb, so he actually won't.
}