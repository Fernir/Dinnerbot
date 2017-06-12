#include "WorldBorders.h"

// Uses DBC game ids
// Extracted from WorldMapArea.dbc
WorldBorder m_borders[] = {{1, 1802.083252, -3122.916504, -3877.083252, -7160.416504},
	{3, -2079.166504, -4566.666504, -5889.583008, -7547.916504},
	{4, -1241.666626, -4591.666504, -10566.66602, -12800},
	{8, -2222.916504, -4516.666504, -9620.833008, -11150},
	{10, 833.333313, -1866.666626, -9716.666016, -11516.66602},
	{11, -389.583313, -4525, -2147.916504, -4904.166504},
	{12, 1535.416626, -1935.416626, -7939.583008, -10254.16602},
	{14, -1962.499878, -7249.999512, 1808.333252, -1716.666626},
	{15, -974.999939, -6225, -2033.333252, -5533.333008},
	{16, -3277.083252, -8347.916016, 5341.666504, 1960.416626},
	{17, 2622.916504, -7510.416504, 1612.499878, -5143.75},
	{28, 416.666656, -3883.333252, 3366.666504, 499.999969},
	{33, 2220.833252, -4160.416504, -11168.75, -15422.91602},
	{36, 783.333313, -2016.666626, 1500, -366.666656},
	{38, -1993.749878, -4752.083008, -4487.5, -6327.083008},
	{40, 3016.666504, -483.333313, -9400, -11733.33301},
	{41, -833.333313, -3333.333252, -9866.666016, -11533.33301},
	{44, -1570.833252, -3741.666504, -8575, -10022.91602},
	{45, -866.666626, -4466.666504, -133.333328, -2533.333252},
	{46, -266.666656, -3195.833252, -7031.249512, -8983.333008},
	{47, -1575, -5425, 1466.666626, -1100},
	{51, -322.916656, -2554.166504, -6100, -7587.499512},
	{65, 3627.083252, -1981.249878, 5575, 1835.416626},
	{66, -600, -5593.75, 7668.749512, 4339.583008},
	{67, 1841.666626, -5270.833008, 10197.91602, 5456.25},
	{85, 3033.333252, -1485.416626, 3837.499756, 824.999939},
	{130, 3449.999756, -750, 1666.666626, -1133.333252},
	{139, -2287.5, -6318.75, 3704.166504, 1016.666626},
	{141, 3814.583252, -1277.083252, 11831.25, 8437.5},
	{148, 2941.666504, -3608.333252, 8333.333008, 3966.666504},
	{210, 5443.75, -827.083313, 9427.083008, 5245.833008},
	{215, 2047.916626, -3089.583252, -272.916656, -3697.916504},
	{267, 1066.666626, -2133.333252, 400, -1733.333252},
	{331, 1699.999878, -4066.666504, 4672.916504, 829.166626},
	{357, 5441.666504, -1508.333252, -2366.666504, -6999.999512},
	{361, 1641.666626, -4108.333008, 7133.333008, 3299.999756},
	{394, -1110.416626, -6360.416504, 5516.666504, 2016.666626},
	{400, -433.333313, -4833.333008, -3966.666504, -6899.999512},
	{405, 4233.333008, -262.5, 452.083313, -2545.833252},
	{406, 3245.833252, -1637.499878, 2916.666504, -339.583313},
	{440, -218.749985, -7118.749512, -5875, -10475},
	{490, 533.333313, -3166.666504, -5966.666504, -8433.333008},
	{493, -1381.25, -3689.583252, 8491.666016, 6952.083008},
	{495, -1397.916626, -7443.749512, 3116.666504, -914.583313},
	{618, -316.666656, -7416.666504, 8533.333008, 3799.999756},
	{718, -107374176, -107374176, -107374176, -107374176},
	{1377, 2537.5, -945.833984, -5958.333984, -8281.25},
	{1497, 873.192627, -86.182404, 1877.945313, 1237.841187},
	{1519, 1722.916626, -14.583333, -7995.833008, -9154.166016},
	{1537, -713.59137, -1504.216431, -4569.241211, -5096.845703},
	{1637, -3680.601074, -5083.205566, 2273.877197, 1338.460571},
	{1638, 516.666626, -527.083313, -849.999939, -1545.833252},
	{1657, 2938.362793, 1880.029541, 10238.31641, 9532.586914},
	{2597, 1781.249878, -2456.25, 1085.416626, -1739.583252},
	{2817, 1443.75, -1279.166626, 6502.083008, 4687.5},
	{3277, 2041.666626, 895.833313, 1627.083252, 862.499939},
	{3358, 1858.333252, 102.083328, 1508.333252, 337.5},
	{3430, -4487.5, -9412.5, 11041.66602, 7758.333008},				// Eversong Woods
	{3433, -5283.333008, -8583.333008, 8266.666016, 6066.666504},	// Ghostlands
	{3483, 5539.583008, 375, 1481.25, -1962.499878},				// Hellfire
	{3487, -6400.75, -7612.208496, 10153.70898, 9346.938477},
	{3518, 10295.83301, 4770.833008, 41.666664, -3641.666504},		// Nagrand
	{3519, 7083.333008, 1683.333252, -999.999939, -4600},			// Terokkar Forest
	{3520, 4225, -1275, -1947.916626, -5614.583008},				// Shadowmoon Valley
	{3521, 9475, 4447.916504, 1935.416626, -1416.666626},			// Zangarmarsh
	{3522, 8845.833008, 3420.833252, 4408.333008, 791.666626},		// Blade's Edge Mountains
	{3523, 5483.333008, -91.666664, 5456.25, 1739.583252},			// Netherstorm
	{3524, -10500, -14570.83301, -2793.75, -5508.333008},			// Azuremyst Isle
	{3525, -10075, -13337.49902, -758.333313, -2933.333252},		// Bloodmyst Isle
	{3537, 8570.833008, 2806.25, 4897.916504, 1054.166626},			// Borean Tundra
	{3557, -11066.36719, -12123.1377, -3609.68335, -4314.371094},	// The Exodar
	{3703, 6135.258789, 4829.008789, -1473.954468, -2344.787842},	// Shattrath
	{3711, 6929.166504, 2572.916504, 7287.499512, 4383.333008},
	{3820, 2660.416504, 389.583313, 2918.75, 1404.166626},
	{4080, -5302.083008, -8629.166016, 13568.74902, 11350},
	{4197, 4329.166504, 1354.166626, 5716.666504, 3733.333252},
	{4265, 0, 0, 0, 0},
	{4298, -4047.916504, -7210.416504, 3087.5, 979.166626},
	{4384, 787.5, -956.249939, 1883.333252, 720.833313},
	{4395, 0, 0, 0, 0}};											// Dalaran

// DBC zone ids
// Northrend
int m_NOzoneIDs[] = {
	3537,	// Borean Tundra 
	2817,	// Crystalsong Forest 
	4395,	// Dalaran 
	65,		// Dragonblight 
	394,	// Grizzly Hills 
	495,	// Howling Fjord 
	-1,		// Hrothgar's Landing 
	210,	// Icecrown 
	3711,	// Sholazar Basin 
	67,		// The Storm Peaks 
	4197,	// Wintergrasp 
	66,		// Zul'Drak

};

int m_OLzoneIDs[] = {
	3522,		// 1 Blade's Edge Mountains
	3483,		// 2 Hellfire
	3518,		// 3 Nagrand
	3523,		// 4 Netherstorm
	3520,		// 5 Shadowmoon Valley
	3703,		// 6 Shattrath
	3519,		// 7 Terokkar
	3521,		// 8 Zangarmarsh
	0,			// 9
	0,			// 10
	3430,		// 11 Eversong Woods
	3433,		// 12 Ghostlands
	0,			// 13
	0,			// 14 
	0,			// 15 Isle of Quel'Danas
	0,			// 16
	0,			// 17
	0,			// 18
	0,			// 19
	3557,		// 20 Exodar
};

// Kalimdor
int m_KAzoneIDs[] = {
	331,	// Ashenvale 
	16,		// Azshara 
	3524,	// Azuremyst Isle	*
	3525,	// Bloodmyst Isle	*
	148,	// Darkshore 
	1657,	// Darnassus 
	405,	// Desolace 
	14,		// Durotar 
	15,		// Dustwallow Marsh 
	361,	// Felwood 
	357,	// Feralas
	493,	// Moonglade 
	215,	// Mulgore 
	1637,	// Orgrimmar 
	1377,	// Silithus 
	406,	// Stonetalon Mountains 
	440,	// Tanaris 
	141,	// Teldrassil
	17,		// The Barrens 
	3557,	// The Exodar		* 
	400,	// Thousand Needles 
	1638,	// Thunder Bluff 
	490,	// Un'Goro Crater 
	618,	// Winterspring
};

// Eastern Kingdoms
int m_EKzoneIDs[] = {
	36,		// Alterac Mountains
	45,		// Arathi Highlands },
	3,		// Badlands},
	4,		// Blasted },
	46,		// Burning Steppes },
	41,		// Deadwind Pass },
	1,		// Dun Morogh },
	10,		// Duskwood },
	139,	// Eastern Plaguelands },
	12,		// Elwynn Forest },
	3430,	// Eversong Woods },		*
	3433,	// Ghostlands },			*
	267,	// Hillsbrad },
	1537,	// Ironforge },
	-1,		// Isle of Quel'Danas },
	38,		// Loch Modan },
	44,		// Redridge Mountains },
	51,		// Searing Gorge },
	3487,	// Silvermoon City },		*
	130,	// Silverpine Forest },
	1519,	// Stormwind City },
	33,		// Stranglethorn Vale },
	8,		// Swamp of Sorrows },
	47,		// The Hinterlands },
	85,		// Tirisfal Glades },
	1497,	// Undercity },
	28,		// Western Plaguelands },
	40,		// Westfall },
	11};	// Wetlands};

UINT SpecialMapConvert(UINT game, UINT mapId)
{
	switch (mapId)
	{
		// Special zoneId cases in which the internal mapId does not match the location of the zone. (Eversong Woods on outland map, but is actually in the Easter Kingdoms).
	case MAP_OUTLAND: // Outland
		switch (game)
		{
			case ZONE_SILVERMOON:
				return MAP_EASTERNKINGDOMS;
				break;

			case ZONE_GHOSTLANDS:
				return MAP_EASTERNKINGDOMS;
				break;

			case ZONE_EVERSONG:
				return MAP_EASTERNKINGDOMS;
				break;
			
			case ZONE_EXODAR:
				return MAP_KALIMDOR;
				break;

			/*case ZONE_AZUREMYST:
				return MAP_KALIMDOR;
				break;

			case ZONE_BLOODMYST:
				return MAP_KALIMDOR;
				break;*/
		}
	}
	
	if (LocalPlayer.zoneId() == 3524)
		return MAP_KALIMDOR;

	else if (LocalPlayer.zoneId() == 3525)
		return MAP_KALIMDOR;

	return mapId;
}

UINT ConvertDBCToGame(UINT dbc, UINT mapId)
{
	for (int x = 0; x < sizeof(m_EKzoneIDs) / sizeof(int); x++)
	{
		switch(mapId)
		{
		case MAP_EASTERNKINGDOMS: // Eastern Kingdoms
			if (m_EKzoneIDs[x] == dbc)
				return x + 1;
			break;

		case MAP_KALIMDOR: // Kalimdor
			if (m_KAzoneIDs[x] == dbc)
				return x + 1;
			break;

		case MAP_OUTLAND: // Outlands
			if (m_OLzoneIDs[x] == dbc)
				return x + 1;
			break;

		case MAP_NORTHREND: // Northrend
			if (m_NOzoneIDs[x] == dbc)
				return x + 1;
			break;
		}
	}

	return -1;
}

UINT ConvertGameToDBC(UINT game, UINT mapId)
{
	mapId = SpecialMapConvert(game - 1, mapId);
	switch(mapId)
	{
	case 0: // Eastern Kingdoms
		return m_EKzoneIDs[game - 1];
	
 	case 1: // Kalimdor
		return m_KAzoneIDs[game - 1];
		
	case 530: // Outland
		return m_OLzoneIDs[game - 1];

	case 571: // Northrend
		return m_NOzoneIDs[game - 1];
	}

	return -1;
}

// input WOWPOS is the world coordinates,
// output WOWPOS is the relative map coordinates for the given zone
WOWPOS ConvertWorldToMap(DWORD zone, WOWPOS world)
{
	WorldBorder *border = NULL;
	WOWPOS map;

	FLOAT xDiff, yDiff, xSpace, ySpace;

	memset(&map, 0, sizeof(WOWPOS));
	border = GetWorldBorderFromZone(zone);
	if (border == NULL) 
	{
		log("ERROR: Unable to find World Border for the specified zone.");
		return map;
	}

	xDiff = border->X2 - border->X1;
	yDiff = border->Y2 - border->Y1;

	xSpace = (world.X - border->X1) / xDiff;
	ySpace = (world.Y - border->Y1) / yDiff;

	map.X = abs(ySpace * 100);
	map.Y = abs(xSpace * 100);
	return map;
}

// input WOWPOS is the relative map coordinates for the given zone
// output WOWPOS is the world coordinates,
WOWPOS ConvertMapToWorld(DWORD zone, WOWPOS map)
{
	WorldBorder *border = NULL;
	WOWPOS world;

	FLOAT xDiff, yDiff, xSpace, ySpace;

	memset(&world, 0, sizeof(WOWPOS));
	border = GetWorldBorderFromZone(zone);
	if (border == NULL)
	{
		log("ERROR: Unable to find World Border for the specified zone.");
		return world;
	}

	xDiff = border->X2 - border->X1;
	yDiff = border->Y2 - border->Y1;

	xSpace = map.Y / 100;
	ySpace = map.X / 100;

	world.X = xSpace * xDiff + border->X1;
	world.Y = ySpace * yDiff + border->Y1;
	return world;
}

WorldBorder *GetWorldBorderFromZone(DWORD zone)
{
	int found = -1;

	for (int x = 0; x < sizeof(m_borders) / sizeof(WorldBorder); x++)
	{
		if (m_borders[x].zone == zone)
		{
			found = x;
			break;
		}
	}

	if (found == -1)
		return NULL;

	return &m_borders[found];
}