#pragma once

#include "Geometry\Polygon.h"
#include <gdiplus.h>
#include <vector>

namespace Radar
{
	class RenderObject
	{
	public:
		enum eType
		{
			Line,
			Point,
			Polygon,
		};
		
		void addRender(const POINT &p);
		void addWorld(const WOWPOS &p);
		void addWorld(const Geometry::Vec2f &p);
		void addWorld(const Geometry::Triangle &t);

		void setColor(Gdiplus::Color);
		void setType(eType);

		void clearRender();
		void clearWorld();
		void clear();

		int size();
		float area();
		eType type();

		Gdiplus::Color color();
		Geometry::Polygon &polygon();

		std::vector<Geometry::Vec2f> &worldBoundary();
		std::vector<POINT> &renderBoundary();

		RenderObject::RenderObject(eType type);
		RenderObject();
		~RenderObject();


	private:
		Gdiplus::Color		_color;
		Geometry::Polygon	_polygon;
		RenderObject::eType _type;
	};

	inline bool operator ==(Radar::RenderObject &lhs, Radar::RenderObject &rhs)
	{
		return lhs.type() == rhs.type() && lhs.polygon() == rhs.polygon();
	}
}