#pragma once
#include <Windows.h>
#include <vector>
#include <stack>

#include "Geometry\triangle.h"
#include "Geometry\Net.h"
#include <Main\Structures.h>
#include <Geometry\ConvexCone.h>

namespace Engine
{
	namespace Navigation
	{
		class Pathfinding
		{
		private:
			Geometry::Vec2f _start, _end;

			std::stack<Geometry::Triangle *> _path;
			Geometry::Net *net;

			std::vector<Geometry::Triangle *> openList;
			std::vector<Geometry::Triangle *> closedList;

		public:
			bool pathFound;
			
			std::vector<WoWPos>smoothPath(std::vector<Geometry::ConvexCone> &path);

			bool findPath(Geometry::Vec2f start, Geometry::Vec2f end);
			bool findPath(WOWPOS start, WOWPOS end);

			void setNet(Geometry::Net *n);
			void clearNet();

			uint getPathNodeSize();

			std::vector<WoWPos> getPath();

			Pathfinding(Geometry::Net *n);
			Pathfinding(void);
			~Pathfinding(void);
		};
	}
}

