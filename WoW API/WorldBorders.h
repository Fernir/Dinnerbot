#pragma once

#include "..\Common\Common.h"
#include "..\Common\Objects.h"

// **** GetCurrentMapAreaID or something to find the zone ids.
// MAP Ids
#define MAP_EASTERNKINGDOMS 0
#define MAP_KALIMDOR		1
#define MAP_OUTLAND			530
#define MAP_NORTHREND		571

// Eastern Kingdoms
#define ZONE_SILVERMOON		18
#define ZONE_GHOSTLANDS		11
#define ZONE_EVERSONG		10

// Kalimdor
#define ZONE_EXODAR			19
#define ZONE_AZUREMYST		2
#define ZONE_BLOODMYST		3

typedef struct WorldBorder
{
	DWORD zone;
	FLOAT Y1;
	FLOAT Y2;
	FLOAT X1;
	FLOAT X2;
} WorldBorder;

UINT ConvertDBCToGame(UINT dbc, UINT mapid);
UINT ConvertGameToDBC(UINT game, UINT mapid);

WorldBorder *GetWorldBorderFromZone(DWORD zone);

WOWPOS ConvertMapToWorld(DWORD zone, WOWPOS map);
WOWPOS ConvertWorldToMap(DWORD zone, WOWPOS world);