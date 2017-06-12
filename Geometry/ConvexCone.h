#pragma once

#include <Geometry\vector2.h>
#include <vector>

namespace Geometry
{
	typedef Vector2<float> Vec2f;

	class ConvexCone
	{
		private:
			Vec2f apexShift(Vec2f v);
			Vec2f apexShift(Vec2f *v);

		public:
			Vec2f apex;
			Vec2f v1, v2;	

			float determinant();
			int rank();

			bool contains(Vec2f vec);
			bool contains(Vec2f *check);
			bool contains(ConvexCone other);
			bool contains(ConvexCone *other);
			void set(Vec2f origin, Vec2f v, Vec2f w);

			ConvexCone(Vec2f origin, Vec2f v, Vec2f w);
			ConvexCone();
	};
}
