#ifndef H_VECTOR2
#define H_VECTOR2

#include <iostream>
#include <cmath>

#include <Utilities\Utilities.h>

namespace Geometry
{
	template <typename T>

	class Vector2
	{
		public:
			// Constructors

			Vector2()
			{
				x = 0;
				y = 0;
			}

			Vector2(T _x, T _y) 
			{
				x = _x;
				y = _y;
			}

			Vector2(const Vector2 &v)
			{
				x = v.x;
				y = v.y;
			}

			void set(const Vector2 &v)
			{
				x = v.x;
				y = v.y;
			}

			bool aEqual(const Vector2 &v)
			{
				return approxEqual(x, v.x) && approxEqual(y, v.y);
			}

			//
			// Operations
			//	
			T dist2(const Vector2 &v)
			{
				T dx = x - v.x;
				T dy = y - v.y;
				return dx * dx + dy * dy;	
			}

			float dist(const Vector2 &v)
			{
				return sqrtf(dist2(v));
			}

			T x;
			T y;

			float norm()
			{
				return sqrtf(x*x + y*y);
			}

			Vector2<T> normalized()
			{
				return Vector2(x / norm(), y / norm());
			}

			void rotate(float radians)
			{
				// Rotation matrix:
				// [ cos a  -sin a ]
				// [ sin a   cos a ]
				x = cos(radians) * x - sin(radians) * y;
				y = sin(radians) * x + cos(radians) * y;
			}

			Vector2<T>& operator -= (Vector2<T> const &rhs)
			{
				x = x - rhs.x;
				y = y - rhs.y;
				return *this;
			}

			Vector2<T>& operator += (Vector2<T> const &rhs)
			{
				x = x + rhs.x;
				y = y + rhs.y;
				return *this;
			}

			Vector2<T>& operator = (Vector2<T> const &v)
			{
				x = v.x;
				y = v.y;
				return *this;
			}
	};

	template<typename T>
	std::ostream &operator << (std::ostream &str, Vector2<T> const &point) 
	{
		return str << "Point x: " << point.x << " y: " << point.y;
	}

	template<typename T>
	bool operator == (Vector2<T> const &v1, Vector2<T> const &v2)
	{
		return (v1.x  == v2.x) && (v1.y == v2.y);
	}

	template<typename T>
	Vector2<T> operator * (T a, Vector2<T> const &v)
	{
		return Vector2<T>(a * v.x, a * v.y);
	}

	template<typename T>
	Vector2<T> operator * (Vector2<T> const &v, T a)
	{
		return Vector2<T>(a * v.x, a * v.y);
	}

	template<typename T>
	Vector2<T> operator * (int a, Vector2<T> const &v)
	{
		return Vector2<T>(a * v.x, a * v.y);
	}

	template<typename T>
	Vector2<T> operator * (Vector2<T> const &v, int a)
	{
		return Vector2<T>(a * v.x, a * v.y);
	}

	template<typename T>
	Vector2<T> operator + (Vector2<T> const &lhs, Vector2<T> const &rhs)
	{
		return Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	template<typename T>
	Vector2<T> operator - (Vector2<T> const &lhs, Vector2<T> const &rhs)
	{
		return Vector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
	}
};

#endif