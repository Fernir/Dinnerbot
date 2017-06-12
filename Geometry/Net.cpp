#include "Net.h"

namespace Geometry
{
	bool Net::contains(Vec2f *p)
	{
		return containedIn(p) != NULL;
	}

	bool Net::contains(Vec2f &p)
	{
		return containedIn(&p) != NULL;
	}

	bool Net::contains(const WOWPOS &p)
	{
		return containedIn(&Vec2f(p.X, p.Y)) != NULL;
	}

	Triangle *Net::containedIn(Vec2f *p)
	{
		for (int x = 0; x < this->order(); x++)
		{
			if (this->_net[x].contains(p)) return &this->_net[x];
		}

		return NULL;
	}

	void Net::setMap(uint map)
	{
		this->_mapId = map;
	}

	uint Net::getMapId()
	{
		return this->_mapId;
	}

	Geometry::Triangle &Net::getTriangle(uint x)
	{
		return this->_net[x];
	}

	// Import a vector of triangles.
	void Net::import(std::vector<Triangle> *ts)
	{
		Triangle *t = NULL;
		for (int x = 0; x < ts->size(); x++)
		{
			t = &ts->at(x);
			this->add(*t);
		}
	}
	
	// Add triangle to our net.
	void Net::add(Triangle tri)
	{
		_net.push_back(tri);
	}

	// Determine distance between centroids of triangles.
	// Hcost will be the distance between the starting
	// and the ending triangle. (again, distance between centroids).

	uint Net::connectNet()
	{
		Triangle *base;
		Triangle *test;

		uint count = 0;

		// For each polygon, iterate through the list of other polygons and determine if they are connected.
		for (int x = 0; x < order(); x++)
		{
			base = &_net[x];
			for (int y = 0; y < order(); y++)
			{
				test = &_net[y];

				// Exclude the base so we don't lead to an infinite loop
				// when it comes to pathfinding. (base -> base -> base -> ...)
				if (test != base)
				{
					// If the base triangle is connected to the test triangle, 
					// add it to the list of neighboring triangles.
					if (base->isAdjacent(test))
					{
						base->neighbors.push_back(test);
						count++;
					}
				}
			}
		}

		this->_connected = true;
		return count;
	}

	bool Net::isConnected()
	{
		return this->_connected;
	}

	int Net::order()
	{
		return _net.size();
	}

	void Net::clearParents()
	{
		for (int x = 0; x < this->order(); x++)
			this->_net[x].parent = NULL;
	}

	void Net::clear()
	{
		_net.clear();
	}

	Net::Net(uint map)
	{
		this->_connected = false;
		this->_net.reserve(10);
		this->_mapId = map;
	}

	Net::Net(void)
	{
		this->_connected = false;
		this->_net.reserve(10);
		this->_mapId = 0;
	}

	Net::~Net(void)
	{
	}
}
