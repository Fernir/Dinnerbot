#pragma once

#include "Common\Common.h"
#include "Common\Objects.h"

namespace Debug
{
	VOID Test();
	VOID DumpAddresses();
	VOID DumpAuras(Object &Unit);
	VOID DumpLocation(Object const &Dump);
	VOID DumpBaseObject(Object const &Dump);
	VOID DumpUnitField(DUnitField const &UnitField);
	VOID DumpGameObjectField(DGameObjectField const &GameObject);
};
