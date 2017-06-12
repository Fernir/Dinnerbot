#include "ConvexCone.h"

namespace Geometry
{
	float ConvexCone::determinant()
	{
		Vec2f localV1 = apexShift(v1);
		Vec2f localV2 = apexShift(v2);

		return localV1.x * localV2.y - localV1.y * localV2.x;
	}

	int ConvexCone::rank()
	{
		Vec2f zero(0, 0);

		if (determinant() == 0)
		{
			if (v1.aEqual(zero) && v2.aEqual(zero))
				return 0;
			else
				return 1;
		}
		
		return 2;
	}

	bool ConvexCone::contains(ConvexCone other)
	{
		return this->contains(&other);
	}

	bool ConvexCone::contains(ConvexCone *other)
	{
		return contains(other->apex) && contains(other->v1) && contains(other->v2);
	}

	bool ConvexCone::contains(Vec2f check)
	{
		return contains(&check);
	}
		
	bool ConvexCone::contains(Vec2f *check)
	{
		Vec2f v = apexShift(check);

		// solve system of v1, v2 = v
		// solution exists if rank = 2
		// if answer is both positive, then the cone contains v
		// Solve the following system:
		/*[ v1.x v2.x | v.x]
	      [ v1.y v2.y | v.y]*/

		float a, b, c, d;
		Vec2f localV1 = apexShift(v1);
		Vec2f localV2 = apexShift(v2);

		if (rank() == 0)
			return *check == apex;

		// Perform a Gaussian row swap if the pivots need to be changed
		if (localV1.x == 0)
		{
			a = localV2.x;
			b = localV2.y;
			c = localV1.x;
			d = localV1.y;
		}
		else
		{
			a = localV1.x;
			b = localV1.y;
			c = localV2.x;
			d = localV2.y;
		}

		// Calculation constants
		float alpha = d - (b * c) / a;
		float beta = v.y - (b * v.x) / a;

		// Coordinates of v in terms of v1 and v2
		float coefficient_v1 = beta / alpha;
		float coefficient_v2 = (v.x - coefficient_v1*c) / a;

		// If both coefficients are nonnegative, then v is inside the cone spanned by v1 and v2 
		return coefficient_v1 >= 0 && coefficient_v2 >= 0;
	}

	void ConvexCone::set(Vec2f o, Vec2f v, Vec2f w)
	{
		this->apex = o;
		this->v1 = v;
		this->v2 = w;
	}

	Vec2f ConvexCone::apexShift(Vec2f v)
	{
		return apexShift(&v);
	}

	Vec2f ConvexCone::apexShift(Vec2f *v)
	{
		return *v - apex;
	}

	ConvexCone::ConvexCone(Vec2f o, Vec2f top, Vec2f bottom)
	{
		this->set(o, top, bottom);
	}

	ConvexCone::ConvexCone()
	{
		Vec2f zero(0, 0);
		this->set(zero, zero, zero);
	}
}