#include "Inventory.h"
#include "Lua.h"
#include "..\Utilities\Utilities.h"

INT FreeInventorySlots()
{
	std::string var = Lua::GenerateLocalVariable(10);

	char buffer[256];
	
	Lua::vDo("%s = GetContainerNumFreeSlots(0) + GetContainerNumFreeSlots(1) + GetContainerNumFreeSlots(2) + GetContainerNumFreeSlots(3) + GetContainerNumFreeSlots(4)", var.c_str());
	Lua::GetText(var.c_str(), buffer);

	if (isASCIIString(buffer)) return -1;
	if (isinteger(buffer))
		return atoi(buffer);

	return 0;
}