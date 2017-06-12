#pragma once

#include <Windows.h>
#include <vector>

#include "Object Classes\Object.h"

namespace Loot
{
	bool		getSlotName(int slot, CHAR *nameResult, size_t maxSize);
	std::string getSlotName(int slot);

	WGUID getCurrentObject();
	UINT  getItemCount();

	bool slot(int slot);
	bool close();
	bool all();

	int local();

	bool slots(std::vector<int> &list);

	bool isWindowOpen();
	bool isWindowClosed();
	bool isSlotCoin(INT slot);

}