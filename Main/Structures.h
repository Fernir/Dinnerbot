#pragma once

#include <math.h>

typedef unsigned int	 uint;
typedef unsigned __int64 uint64;
typedef unsigned long long ullong;

struct WGUID
{
    union
    {
        __int64 ullGuid;
        struct
        {
            unsigned long low;	// low
            unsigned long high;	// high
        };
    };

    WGUID()
    {
    }

    WGUID(unsigned long dwLow, unsigned long dwHigh)
    {
        low = dwLow;
        high = dwHigh;
    }
    WGUID(__int64 iGuid)
    {
        ullGuid = iGuid;
    }

	bool operator ==(const WGUID &g) const
	{
		return (low == g.low && high == g.high);
	}

	bool operator !=(const WGUID &g) const
	{
		return (g.low != low) || (g.high != high);
	}

};

typedef struct WOWPOS
{   // Order of elements is important.
	float X;
	float Y;
	float Z;
	float Null;
	float Rotation;

	bool operator ==(const WOWPOS &a) const
	{
		return (X == a.X && Y == a.Y && Z == a.Z);
	}

} WOWPOS;

inline WOWPOS operator +(const WOWPOS &lhs, const WOWPOS &rhs)
{
	WOWPOS n;

	n.X = lhs.X + rhs.X;
	n.Y = lhs.Y + rhs.Y;
	n.Z = lhs.Z + rhs.Z;
	return n;
}

inline WOWPOS operator *(float x, const WOWPOS &rhs)
{
	WOWPOS n;

	n.X = x * rhs.X;
	n.Y = x * rhs.Y;
	n.Z = x * rhs.Z;
	return n;
}

inline WOWPOS operator -(const WOWPOS &lhs, const WOWPOS &rhs)
{
	WOWPOS n;

	n.X = lhs.X - rhs.X;
	n.Y = lhs.Y - rhs.Y;
	n.Z = lhs.Z - rhs.Z;
	return n;
}


class WoWPos
{
private:

public:
	float x, y, z;
	float rotation;
	float tolerance;

	bool finite()
	{
		return false; // (WORD3(X) & 0x7FF0) != 32752;
	}

	bool isValid()
	{
		double v4; // st7@4
		double v5; // st6@4
		double v6; // st5@4

		if (this->finite())
		{
			v4 = -(y - 17066.666);
			v5 = -(x - 17066.666);
			v6 = tolerance;
			if (tolerance > v4)
				return false;

			if (34133.332 - v6 > v4 && v6 <= v5 && v5 < 34133.332 - v6)
				return true;
		}

		return false;
	}

	float distance(WOWPOS loc) { return sqrt(((loc.X - x)*(loc.X - x)) + ((loc.Y - y)*(loc.Y - y))); }

	WoWPos(float X, float Y) { x = X; y = Y; z = 0; rotation = 0; tolerance = 0; }
	WoWPos(float X, float Y, float Z) { x = X; y = Y; z = Z; rotation = 0; tolerance = 0; }
	WoWPos(float X, float Y, float Z, float Rotation) { x = X; y = Y; z = Z; rotation = Rotation; tolerance = 0; }
	WoWPos(WOWPOS pos) { x = pos.X; y = pos.Y; z = pos.Z; rotation = pos.Rotation; tolerance = 0; }
	WoWPos() { x = 0; y = 0; z = 0; tolerance = 0; }
	~WoWPos() {}

	// WOWPOS typecast to WoWPos.
	operator WOWPOS()
	{
		return { x, y, z, rotation };
	}

	WoWPos& operator =(const WOWPOS &rhs)
	{
		this->x = rhs.X;
		this->y = rhs.Y;
		this->z = rhs.Z;
		this->rotation = rhs.Rotation;
		return *this;
	}
};

inline WoWPos operator +(const WoWPos &lhs, const WoWPos &rhs)
{
	WoWPos n;

	n.x = lhs.x + rhs.x;
	n.y = lhs.y + rhs.y;
	n.z = lhs.z + rhs.z;
	return n;
}

inline WoWPos operator *(float x, const WoWPos &rhs)
{
	WOWPOS n;

	n.X = x * rhs.x;
	n.Y = x * rhs.y;
	n.Z = x * rhs.z;
	return n;
}

inline WoWPos operator -(const WoWPos &lhs, const WoWPos &rhs)
{
	WoWPos n;

	n.x = lhs.x - rhs.x;
	n.y = lhs.y - rhs.y;
	n.z = lhs.z - rhs.z;
	return n;
}
