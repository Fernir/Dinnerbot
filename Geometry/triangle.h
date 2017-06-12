// Credit to guy who wrote most of the Delaunay library.
// Although I'm using another triangulation library (the french guy's triangulation didn't handle holes), 
// this still helps with the work I've put into it.

#ifndef H_TRIANGLE
#define H_TRIANGLE

#include <vector>

#include "vector2.h"
//#include "edge.h"

namespace Geometry
{
	typedef Vector2<float> Vec2f;

	class Triangle
	{
		public:

			Triangle(const Vec2f &_p1, const Vec2f &_p2, const Vec2f &_p3);
	
			bool containsVertex(const Vec2f &v);
			bool circumCircleContains(const Vec2f &v);

			// Dinner added.
			float sign(Vec2f *p1, Vec2f *p2, Vec2f *p3);
			float centroidDistance(Triangle *other);

			void set(Vec2f p1, Vec2f p2, Vec2f p3);

			Vec2f centroid();
			std::vector<Vec2f> getCommonVertices(Triangle *other);
			Vec2f edgeMidpointBetween(Triangle *other);

			bool contains(Vec2f *v);
			bool isAdjacent(Triangle *t);

			void setParent(Triangle *p);

			std::vector<Triangle *> neighbors;
			Triangle *parent;

			// For pathfinding.
			float Gcost;
			float Hcost;
			float Fcost;
	
			// Points
			Vec2f p1;
			Vec2f p2;
			Vec2f p3;

			/*Edge start;
			Edge end;
			Edge apex;*/
	};

	inline bool operator == (const Triangle &t1, const Triangle &t2)
	{
		return	(t1.p1 == t2.p1 || t1.p1 == t2.p2 || t1.p1 == t2.p3) &&
				(t1.p2 == t2.p1 || t1.p2 == t2.p2 || t1.p2 == t2.p3) && 
				(t1.p3 == t2.p1 || t1.p3 == t2.p2 || t1.p3 == t2.p3);
	}

};
#endif
