#pragma once
#include <Windows.h>

#include <Main\Structures.h>
#include "Unit.h"

#include <vector>

// return 0 to stop enumeration, 1 to continue.
typedef BOOL (__cdecl *VisibleObjectsEnumProc)(uint64 objectGuid, void *param);

typedef int (type_ObjectCallback)(CObject *current);
typedef int (type_ObjectCallback_param)(CObject *current, void *parameter);

namespace ObjectManager
{
	CObject ObjectCallback(type_ObjectCallback);
	CObject ObjectCallback(type_ObjectCallback_param, void *parameter);
	WGUID   GetLocalGUID();
	DWORD   GetCurrent();
};

extern CUnit LocalPlayer;

// We currently have to load the list before allowing any use of it
// A much more efficient way is to define iterators in such a way that each 
// retrieval of an element would be to go through the object manager in memory
template <typename T>
class CObjectManager
{
private:
	std::vector<T> list;

public:
	enum ErrorCode
	{
		MemoryError,
		EmptyError,
		SafeLoad,
	};

	// Iterators
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;

	iterator begin() { return list.begin(); }
	const_iterator begin() const { return list.cbegin(); }

	iterator end() { return list.end(); }
	const_iterator end() const { return list.cend(); }

	void add(T &obj) { this->list.push_back(obj); }
	void clear() { this->list.clear(); }

	uint size() { return this->list.size(); }

	bool empty() { return this->list.empty(); }

	ErrorCode load()
	{
		/* Iterates through object list and calls function on every iteration for general purposes.*/
		/* The callee must set Bytes_1 in the object struct to return. */
		T current, next;
		DWORD first;

		if (!Memory::Read(ObjectManager::GetCurrent() + Offsets::eObjectManager::FirstEntry, &first))
			return ErrorCode::MemoryError;

		// Get first object in manager
		current.setBase(first);
		current.update();

		while (current.isValid())	
		{
			// Push onto object manager
			if (current.isType())
				this->add(current);

			// Set next.base to the next base address in the object manager.
			next.updateBase(current.nextPtr());
			if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
				break;
			else
				current.update(next.object()); // Otherwise, update the current object as the previous next object.
		}

		if (this->empty())
			return ErrorCode::EmptyError;

		return ErrorCode::SafeLoad;
	}

	CObjectManager(void) { this->load(); }
	~CObjectManager(void) {}
};
