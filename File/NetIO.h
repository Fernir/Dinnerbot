#pragma once

#include <vector>
#include "Geometry\Net.h"

namespace NetIO
{
	int  ImportNet(Geometry::Net &, const std::string &fileName);
	bool OutputNet(Geometry::Net &, const std::string &fileName);
};