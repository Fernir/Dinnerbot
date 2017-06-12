#include "ObjectManager.h"
#include "Main\Constants.h"
#include "Memory\Memory.h"
#include "Game\Game.h"

CUnit LocalPlayer;

DWORD ObjectManager::GetCurrent()
{
	DWORD buffer = NULL;

	if (!rpm(Offsets::ClientConnection, &buffer, sizeof(DWORD)))
		return FALSE;

	if (!rpm(buffer + Offsets::CurrentManager, &buffer, sizeof(DWORD)))
		return FALSE;

	return buffer;
}

CObject ObjectManager::ObjectCallback(type_ObjectCallback CallbackAddress)
{
	/* Iterates through object list and calls function on every iteration for general purposes.*/
	/* The callee must set Bytes_1 in the object struct to return. */
	CObject current, next;
	DWORD first;

	if (!Memory::Read(GetCurrent() + Offsets::eObjectManager::FirstEntry, &first, sizeof(DWORD)))
		return current;

	current.setBase(first);
	current.update();
	while (current.isValid())
	{
		CallbackAddress(&current);

		// It's possible that the callee has set other update flags, so be aware.
		if (current.hasReturnBytes())
			return current;

		// Set next.base to the next base address in the object manager.
		next.updateBase(current.nextPtr());
		if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
			break;
		else
			current.update(next.object()); // Otherwise, update the current object as the previous next object.
	}

	current.clear();
	return current;
}

CObject ObjectManager::ObjectCallback(type_ObjectCallback_param CallbackAddress, void *parameter)
{
	/* Iterates through object list and calls function on every iteration for general purposes.*/
	/* The callee must set Bytes_1 in the object struct to return. */
	CObject current, next;
	DWORD first;

	if (!Memory::Read(GetCurrent() + Offsets::eObjectManager::FirstEntry, &first, sizeof(DWORD)))
		return current;

	current.setBase(first);
	current.update();
	while (current.isValid())
	{
		CallbackAddress(&current, parameter);

		// It's possible that the callee has set other update flags, so be aware.
		if (current.hasReturnBytes())
			return current;

		// Set next.base to the next base address in the object manager.
		next.updateBase(current.nextPtr());
		if (next.base() == current.base()) // If they are the same, then we've hit the end of the object manager.
			break;
		else
			current.update(next.object()); // Otherwise, update the current object as the previous next object.
	}

	current.clear();
	return current;
}

WGUID ObjectManager::GetLocalGUID()
{
	WGUID buffer;

	rpm(GetCurrent() + Offsets::PlayerGUID, &buffer, sizeof(WGUID));
	return buffer;
}