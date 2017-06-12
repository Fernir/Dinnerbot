#include "Coordinate.h"
#include "Main\Constants.h"
#include <WoW API\Object Classes\Object.h>
#include <WoW API\Object Classes\ObjectManager.h>

#include <math.h>

float GetDistance3D(float fLocation2X, float fLocation1X, float fLocation2Y, float fLocation1Y, float fLocation2Z, float fLocation1Z)
{
	return sqrt(((fLocation2X - fLocation1X)*(fLocation2X - fLocation1X)) + ((fLocation2Y - fLocation1Y)*(fLocation2Y - fLocation1Y)) + ((fLocation2Z - fLocation1Z)*(fLocation2Z - fLocation1Z)));
}

float GetDistance3D(WOWPOS Pos1, WOWPOS Pos2)
{
	return GetDistance3D(Pos1.X, Pos2.X, Pos1.Y, Pos2.Y, Pos1.Z, Pos2.Z);
}

float GetDistance2D(float fLocation2X, float fLocation1X, float fLocation2Y, float fLocation1Y)
{
	return sqrt(((fLocation2X - fLocation1X)*(fLocation2X - fLocation1X)) + ((fLocation2Y - fLocation1Y)*(fLocation2Y - fLocation1Y)));
}

bool ValidCoord(WOWPOS Pos)
{
	if ((Pos.X == 0 && Pos.Y == 0 && Pos.Z == 0) || GetDistance(Pos) >= 100000) return FALSE;
	return TRUE;
}

float GetDistance(float fLocation2X, float fLocation1X, float fLocation2Y, float fLocation1Y)
{
	return GetDistance2D(fLocation2X, fLocation1X, fLocation2Y, fLocation1Y);
}

float GetDistance(WOWPOS Pos1, WOWPOS Pos2)
{
	return GetDistance2D(Pos2.X, Pos1.X, Pos2.Y, Pos1.Y);
}

float GetDistance(Object Unit1, Object Unit2)
{
	return GetDistance2D(Unit2.X, Unit1.X, Unit2.Y, Unit1.Y);
}

float GetDistance(Object Unit1)
{
	return GetDistance2D(Unit1.X, LocalPlayer.Obj.X, Unit1.Y, LocalPlayer.Obj.Y);
}

float GetDistance(WOWPOS Pos)
{
	return GetDistance2D(Pos.X, LocalPlayer.Obj.X, Pos.Y, LocalPlayer.Obj.Y);
}

float GetRotationToFace(WOWPOS Pos)
{
	return GetRotationToFace(LocalPlayer.Obj.X, LocalPlayer.Obj.Y, Pos.X, Pos.Y);
}

float GetRotationToFace(WOWPOS p1, WOWPOS p2)
{
	return GetRotationToFace(p1.X, p1.Y, p2.X, p2.Y);
}

float GetRotationToFace(float fLocation2X, float fLocation2Y)
{
	return GetRotationToFace(LocalPlayer.Obj.X, LocalPlayer.Obj.Y, fLocation2X, fLocation2Y);
}

float GetRotationToFace(float fLocation1X, float fLocation1Y, float fLocation2X, float fLocation2Y)
{
	double fRotationToFace = atan2((double)(fLocation2Y - fLocation1Y), (double)(fLocation2X - fLocation1X));

	while (fRotationToFace < 0.0) fRotationToFace += 2*PI;
	while (fRotationToFace > 2*PI) fRotationToFace -= 2*PI;

	return (float)fRotationToFace;
}