#pragma once
#include <vector>

#include "Polygon.h"
#include "triangle.h"

typedef unsigned int uint;

namespace Geometry
{
	class Net
	{
	private: 
		bool _connected;

		uint _mapId;
		std::vector<Triangle> _net;

	public:
		//uint zone;

		Net(uint map);
		Net(void);
		~Net(void);

		int order();
		bool isConnected();
		bool contains(Vec2f *p);
		bool contains(Vec2f &p);
		bool contains(const WOWPOS &p);

		Triangle *containedIn(Vec2f *p);

		Geometry::Triangle &Net::getTriangle(uint x);
		uint getMapId();

		void clear();
		void clearParents();
		void setMap(uint map);
		void add(Triangle tri);
		void import(std::vector<Triangle> *t);

		uint connectNet();
	};
}
