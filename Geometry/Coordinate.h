#pragma once

#include "Main\Structures.h"
#include "WoW API\Object Classes\Object.h"

bool ValidCoord(WOWPOS Pos);

float GetDistance3D(float fLocation2X, float fLocation1X, float fLocation2Y, float fLocation1Y, float fLocation2Z, float fLocation1Z);
float GetDistance3D(WOWPOS Pos1, WOWPOS Pos2);

float GetDistance2D(float fLocation2X, float fLocation1X, float fLocation2Y, float fLocation1Y);
float GetDistance(float fLocation2X, float fLocation1X, float fLocation2Y, float fLocation1Y);
float GetDistance(Object Unit1, Object Unit2);
float GetDistance(WOWPOS Pos1, WOWPOS Pos2);
float GetDistance(Object Unit1);
float GetDistance(WOWPOS Pos);

float GetRotationToFace(WOWPOS p1, WOWPOS p2);
float GetRotationToFace(float fLocation2X, float fLocation2Y);
float GetRotationToFace(float fLocation1X, float fLocation1Y, float fLocation2X, float fLocation2Y);
