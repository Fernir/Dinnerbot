#pragma once
#include <Windows.h>
#include <vector>

#include "triangle.h"
#include "..\Main\Structures.h"

namespace Geometry
{
	// This is the convex hull described by the points in the boundary vector.
	class Polygon
	{
	public:
		// World 
		std::vector<Vec2f> vertices;

		// Render
		// For rendering in radar.
		std::vector<POINT> transformed;

		Polygon(void);
		Polygon(int order);
		Polygon(const std::vector<Vec2f> &p);

		~Polygon(void);

		void addTriangle(const Triangle &t);
		void import(const std::vector<Triangle> &ts);

		float area();

		bool contains(POINT v);
		bool contains(Vec2f v);
		bool contains(const Vec2f &v);

		void remove(const Vec2f &v);
		void add(const Vec2f &v);
		void add(WOWPOS v);
		void add(Vec2f v);
		void clear();

		void convertVertices();
		int order();

		bool operator ==(const Polygon &t) const
		{
			return t.vertices == vertices;
		}
	};
}
