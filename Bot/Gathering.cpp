#include "Gathering.h"

#include "Game\Interactions.h"
#include "Geometry\Coordinate.h"

#include "Memory\Endscene.h"
#include "Memory\Hacks.h"

#include "WoW API\Lua.h"
#include "WoW API\Chat.h"
#include "WoW API\Spell.h"
#include "WoW API\CGLoot.h"

#include <time.h>

typedef struct 
{
	Object node;
	ULONGLONG start;
	ULONGLONG duration;
} PlayerBlacklist;

PlayerBlacklist playerBlacklist[256];

Object nodeBlacklist[512];

UINT blacklistSize = 0;
UINT playerBlacklistSize = 0;

BOOL m_hideState = FALSE;

DGatherer Gatherer;

DWORD GetGathererState()
{
	return Gatherer.GatheringState;
}

UINT GetCurrentWaypoint()
{
	return 1;
}

BOOL GetGathererNodeName(CHAR *szName, SIZE_T maxSize)
{
	if (strlen(Gatherer.currentNode.Name) + 1 < maxSize)
	{
		strncpy(szName, Gatherer.currentNode.Name, strlen(Gatherer.currentNode.Name) + 1);
		return true;
	}

	return false;
}

DGatherer InitGatherer()
{
	DGatherer gatherer;

	memset(&Gatherer, 0, sizeof(Gatherer));
	Gatherer.GatheringState = GSTATE_SEARCHING;
	Gatherer.StartGatherTime = GetTickCount64();
	GetGatherTime(Gatherer.StartGatherTime);
	return Gatherer;
}

BOOL CanGather()
{
	return !LocalPlayer.inCombat() && LocalPlayer.isAlive() && WoW::InGame();
}

BOOL EndofNodeList(Object node)
{
	if (node.BYTES_2 == 0x93) return TRUE;

	return FALSE;
}

INT BlackListNode(Object node)
{
	CHAR szBuffer[256];

	if (blacklistSize < sizeof(nodeBlacklist) / sizeof(Object))
	{
		sprintf_s(szBuffer, "dblacklistsize = \"%d\"", blacklistSize + 1);
		Lua::DoString(szBuffer);

		nodeBlacklist[blacklistSize++] = node;
		sprintf_s(szBuffer, "Blacklisted node: %s (%0.2f, %0.2f, %0.2f)", node.Name, node.Pos.X, node.Pos.Y, node.Pos.Z);
		LogFile(szBuffer);
		//AddChatMessage(szBuffer, "DGather", TurqoiseMessage);
		log(szBuffer);
	}

	return blacklistSize;
}

INT PlayerBlackListNode(Object node, DWORD miliseconds)
{
	CHAR szBuffer[256];

	for (int x = 0; x < 256; x++)
	{
		if (GetTickCount64() > playerBlacklist[x].start + playerBlacklist[x].duration)
		{
			playerBlacklist[x].node = node;
			playerBlacklist[x].start = GetTickCount64();
			playerBlacklist[x].duration = miliseconds;

			sprintf_s(szBuffer, "Blacklisted node for %0.2f seconds: %s (%0.2f, %0.2f, %0.2f)", ((FLOAT)miliseconds / 1000), node.Name, node.Pos.X, node.Pos.Y, node.Pos.Z);
			LogFile(szBuffer);
			log(szBuffer);

			sprintf_s(szBuffer, "node index: %d", x);
			log(szBuffer);
			return x;
		}
	}

	return 0;
}

BOOL PlayerBlacklistCheck(Object node)
{
	for (int x = 0; x < 256; x++)
	{
		//if (CompareWGUIDS(nodeBlacklist[x].GUID, node.GUID))
		if (GetDistance(node.Pos, playerBlacklist[x].node.Pos) < 5)
		{
			if (GetTickCount() < playerBlacklist[x].start + playerBlacklist[x].duration)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL BlacklistCheck(Object node)
{
	for (int x = 0; x < blacklistSize; x++)
	{
		//if (CompareWGUIDS(nodeBlacklist[x].GUID, node.GUID))
		if (GetDistance(node.Pos, nodeBlacklist[x].Pos) < 5)
			return TRUE;
	}

	return FALSE;
}

BOOL StartDinnerGather(INT nTimeLimit)
{
	Thread *thread = NULL;

	if (nTimeLimit <= 0) return false;

	thread = Thread::FindType(Thread::eType::Bot);
	if (thread)
	{
		thread->stop();
		return false;
	}

	thread = Thread::Create(Gather, (LPVOID)nTimeLimit, Thread::eType::Bot, Thread::ePriority::High);
	thread->setTask(Thread::eTask::Gathering);
	return true;
}

UINT GetLocalNodes(CHAR *szNodeName[], UINT numNodeNames, Object *nodes)
{
	// Given an array of node names, we shall search the object manager for our named nodes.
	Object CurrentObject, NextObject;
	UINT index = 0;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return NULL;
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, NameInfo | GameObjectInfo | LocationInfo | BaseObjectInfo);
	while (ValidObject(CurrentObject))
	{
		if (CurrentObject.Type_ID == OT_GAMEOBJ && ValidCoord(CurrentObject.Pos) && !BlacklistCheck(CurrentObject) && !PlayerBlacklistCheck(CurrentObject))
		{
			for (int x = 0; x < numNodeNames; x++)
			{
				if (!_stricmp(szNodeName[x], CurrentObject.Name)) // If the current object's name is in our list, add this node to the output node list.
					nodes[index++] = CurrentObject;
			}
		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, NameInfo | GameObjectInfo | LocationInfo | BaseObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	if (index == 0) return NULL;
	nodes[index].BYTES_2 = 0x93;
	return index;
}

WOWPOS *SortWaypointsByDistance(WOWPOS *waypoints, size_t size)
{
	WOWPOS *sorted = (WOWPOS *)malloc(sizeof(WOWPOS) * size);

	size_t sublistIndex = 0;
	size_t closestIndex = 0;

	while (sublistIndex < size)
	{
		for (int x = sublistIndex; x < size; x++)
		{
			if (GetDistance3D(waypoints[x], LocalPlayer.pos()) < GetDistance3D(waypoints[closestIndex], LocalPlayer.pos()))
				closestIndex = x;
		}
	}

	sublistIndex++;
	return sorted;
}

UINT GetNextClosestWaypoint(WOWPOS *waypoints, size_t listSize, UINT *nonList, size_t nonListSize)
{
	bool consider = true;
	INT closest = -1;

	for (int x = 0; x < listSize; x++)
	{
		consider = true;

		// if x is not in nonList, then test x against the closest waypoint.
		for (int y = 0; y < nonListSize; y++)
		{
			if (x == nonList[y])
			{
				consider = false;
			}
		}

		if (consider)
		{
			if (closest == -1) 
			{
				closest = x;
			}
			else if (GetDistance3D(waypoints[x], LocalPlayer.pos()) < GetDistance3D(waypoints[closest], LocalPlayer.pos()))
			{
				closest = x;
			}
		}
	}

	return closest;
}

DWORD GetLatency()
{
	CHAR latencyString[MAX_PATH];

	Lua::DoString("earsefsf, esrfshdh, dlatency = GetNetStats()");
	Lua::GetText("dlatency", latencyString);

	if (isASCIIString(latencyString))
	{
		if (isinteger(latencyString))
		{
			return atoi(latencyString);
		}
	}

	return 0;
}

VOID GHide(WGUID playerList[64])
{
	// Unused

	CHAR szBuffer[256];
	Object player;

	if (LocalPlayer.inCombat() || HasAuraBySpellId(LocalPlayer.Obj, 1784)) return; // If player is already stealthed, then return.
	//CastSpellByID(1784)
	
	if (!m_hideState)
	{

		AddChatMessage("Player(s) found, attempting to hide...", "DFish", RedWarningMessage);
		LogAppend("Player(s) found, attempting to hide...");
		m_hideState = TRUE;
	}

	if (m_hideState && !HasAuraBySpellId(LocalPlayer.Obj, 1784))
	{
		Lua::DoString("CastSpellByID(1784)");
	}

	Sleep(2000);
	LogAppend("Players nearby:");

	for (int x = 0; (player = GetObjectByGUIDEx(playerList[x], NameInfo | LocationInfo | BaseObjectInfo)).BaseAddress != NULL; x++)
	{
		sprintf_s(szBuffer, "%s (%0.2f, %0.2f, %0.2f) %0.2f yards away", player.Name, player.X, player.Y, player.Z, GetDistance(player.Pos));
		LogAppend(szBuffer);
	}
}

FLOAT ClosestHostileUnit()
{	// TODO: fucking UnitReaction.

	Object CurrentObject, NextObject;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return 99999;
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, UnitFieldInfo | LocationInfo | BaseObjectInfo);
	while (ValidObject(CurrentObject))
	{
		if (CurrentObject.Type_ID == OT_GAMEOBJ && ValidCoord(CurrentObject.Pos) && !BlacklistCheck(CurrentObject))
		{

		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, UnitFieldInfo | LocationInfo | BaseObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	return 99999;
}

BOOL IsLocalPlayerBeingAttacked()
{
	Object CurrentObject, NextObject;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return true;
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo);
	while (ValidObject(CurrentObject))
	{
		if (CurrentObject.Type_ID == OT_UNIT)
		{
			CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | UnitFieldInfo);
			if (CurrentObject.UnitField.Target == LocalPlayer.guid().low)
			{
				return true;
			}
		}

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}

	return false;
}

VOID InitializeBlacklist()
{
	Object buffer;

	WOWPOS pos[] = {
		{7951.10, 2998.13, 537.98}, 
		{7984.55, 3092.19, 618.79}, 
		{6696.63, -4465.16, 450.38}, 
		{299.40, -5925.31, 169.69},
		{4984.59, 2790.90, 380.65},
		{4278.94, 3477.65, 356.53},
		{7196.61, -833.36, 934.53},
		{6117.12, 5504.67, -72.92},
		{7604.06, 3568.77, 656.51},

	};

	for (int x = 0; x < sizeof(pos) / sizeof(WOWPOS); x++)
	{
		buffer.Pos.X = pos[x].X;
		buffer.Pos.Y = pos[x].Y;
		buffer.Pos.Z = pos[x].Z;
		BlackListNode(buffer);
	}
	
}

ULONGLONG GetGatherTime(DWORD set)
{
	static ULONGLONG time = NULL;

	if (set == -1)
		return time;

	time = set;
	return time;
}

VOID UpdatePosition()
{
	SendKey('A', WM_KEYDOWN);
	Sleep(20);

	SendKey('A', WM_KEYUP);
}

UINT Gather(UINT nTimeLimit)
{
	// Iron, Mithril, Thorium in EK.
	/*{{-7151.917480, -2242.459717, 288.600830},
	{-7451.062012, -2254.197510, 235.128525},
	{-7392.135254, -2303.957031, 238.757507},
	{-7319.035156, -2441.150391, 294.039703},
	{-6827.35, -3704.83, 362.74},
	{-7258.52, -3513.90, 370.59},
	{-7289.31, -3232.75, 359.66},
	{-6680.35, -3113.16, 402.35},
	{-6787.45, -2967.71, 218.79},
	{-6150.63, -2883.35, 211.57},
	{-1799.29, -2186.96, 92.77},
	{-1200.94, -2072.38, 109.30},
	{-962.02, -2924.39, 66.54},
	{-1034.49, -3752.80, 129.91},
	{-1773.21, -3470.04, 83.22},
	{-2007.48, -3322.02, 147.39},
	{-953.24, -3872.74, 232.01},
	{73.50, -3727.32, 154.46},
	{-318.52, -3233.49, 130.14},
	{-195.70, -2540.21, 149.92},
	{408.79, -2864.55, 144.12},
	{610.23, -3377.83, 105.35},
	{758.05, -3826.80, 125.66},
	{375.27, -4675.65, 15.83},
	{-1948.36, -3155.22, 98.53},
	{-6947.098145, -3657.138916, 252.221024},
	{-6497.366211, -3052.193848, 300.850952}, 
	{1614.36, -4745.66, 97.54},
	{1531.49, -5414.42, 93.33},
	{-6712.55, -1592.93, 193.24},
	{-7041.82, -1184.95, 256.20},
	{-7217.90, -1845.21, 294.42},
	{-6580.24, -1833.54, 246.59},
	{-7544.81, -2043.10, 223.49},
	{-7870.68, -2618.45, 221.17},
	{-7907.68, -2738.72, 178.04}};*/
	
	//CHAR *nodeNames[] = {"Iron Deposit", "Mithril Deposit", "Rich Thorium Vein", "Small Thorium Vein"};
	//CHAR *lootNames[] = {"Iron Ore", "Mithril Ore", "Thorium Ore"};
	

	// Outlands mining profile.
	/*
	WOWPOS waypoints[] = {
		// Fel Iron
		{196.87, 4158.97, 78.99},
		{-74.59, 4374.27, 83.67},
		{495.60, 2684.96, 205.92},
		{-313.90, 2013.69, 116.71},
		{-350.23, 1854.57, 93.22},
		{-197.15, 3976.27, 110.73},
		{-1143.23, 4180.32, 21.67},
		{998.83, 5194.71, -19.00},
		{946.20, 5578.41, -2.36},
		{1658.94, 8597.11, 9.65},
		{1714.31, 8532.58, 7.13},
		{1608.94, 8463.29, -22.21},
		{-311.93, 7630.46, 15.44},
		{-238.82, 7373.01, 24.34},
		{-326.93, 7177.23, 58.65},
		{-308.64, 6997.84, 33.00},
		{-395.93, 6783.28, 27.56},
		{-428.03, 6518.85, 27.47},
		{-542.88, 6345.21, 20.61},


		// Khorium 
		{-340.75, 5138.72, 119.36},
		{246.81, 4902.72, 76.88},
		{367.43, 3588.06, 181.24},

	};

	CHAR *nodeNames[] = {"Fel Iron Deposit", "Rich Adamantite Deposit", "Adamantite Deposit", "Khorium Vein"};
	CHAR *lootNames[] = {"Fel Iron Ore", "Adamantite Ore", "Khorium Ore"};*/

	// Northrend herb profile
	WOWPOS herbingWaypoints[] = {
		// Lichbloom
		{7926.01, 1587.19, 406.93},
		{7946.92, 1806.67, 379.89},
		{6314.95, 234.72, 392.04},
		{7666.78, 3492.61, 648.55},
		{7261.20, 590.13, 499.38},
		{6029.99, -1024.67, 414.71},
		{7385.98, -1288.16, 902.78},
		{7589.90, -1373.13, 956.67},
		{7315.14, -818.90, 935.54},
		{8130.95, -590.51, 960.66},
		{7395.00, 1085.52, 395.49},
		{4594.17, 2689.82, 379.26},
		{5092.05, 3578.00, 358.95},
		{4145.78, 3127.50, 354.09},
		{4179.97, 2551.61, 355.66},
		{7244.47, 4214.67, 582.36},
		{8009.77, 3669.09, 617.52},
		{7545.52, 3010.67, 523.53},
		{6883.49, -3862.56, 639.60},
		{6693.82, -3891.01, 565.33},
		{6765.19, -3777.56, 661.87},
		{6868.69, -2952.63, 636.95},
		{8396.08, -1067.35, 918.89},
		{7964.59, -441.45, 897.98},
		{7722.77, -2127.11, 1139.77},
		{7784.41, -2141.33, 1134.38},
		{8221.94, -2646.14, 1145.01},
		{8282.28, -619.26, 919.46},
		{8139.26, 2628.15, 550.70},
		{8030.55, 2565.44, 502.58},
		{5885.56, 2546.63, 553.21},
		{6358.04, 1637.36, 561.88},

		// Adder's Tongue
		{4874.14, 4228.29, -43.04},
		{5103.57, 4006.12, -58.11},
		{6300.08, 4193.44, -43.62},
		{6666.01, 5082.12, -31.56},
		{6393.02, 5313.00, -50.38},
		{5678.59, 5648.43, -77.54},
		{5100.66, 5130.04, -133.07},

		// Talandra's Rose
		/*
		{5917.61, -2083.14, 242.85},
		{5065.34, -1698.63, 234.08},*/

		// Icethorn
		{7641.69, 3480.98, 643.13},
		{7159.16, -711.09, 890.28},
		{7196.61, -833.36, 934.53},
		{8225.38, 20.87, 840.80},
		{7645.87, 953.83, 487.53},
		{4950.22, 2340.90, 327.63},
		//{6830.58, 4045.83, 616.72},
		//{7261.20, 4133.15, 575.78},
		{7495.35, 3700.46, 624.03},
		{7802.00, -319.91, 903.79},
		{7583.37, 212.42, 836.95},
		{6665.38, -491.23, 1167.13},
		{6841.48, -619.66, 977.95},
		{6441.98, -960.15, 448.56},
		{6271.82, -939.32, 406.27},
		{6613.22, -1814.16, 948.23},
		{7898.21, -2326.26, 1153.01},
		{7751.22, -2696.11, 1091.56},
		{8272.80, -2824.13, 1119.56},
		{8210.34, 803.33, 500.62},

		//2 at (7495.35, 3700.46, 624.03)

		// Frost Lotus
		{4833.46, 2445.89, 329.95},

		// Frozen Herb
		/*
		{6703.29, -4330.33, 441.47},
		{6653.35, -4370.28, 441.23},*/


		// Goldclover
		/*
		{6461.52, -3405.58, 390.34},
		{6558.46, -4688.13, 450.53},
		{5103.57, 4006.12, -58.111},
		{6488.85, 5186.67, -59.10},
		{6111.31, 5504.67, -72.83},
		{6044.52, 5448.04, -85.57},
		{5858.65, 5539.52, -58.84},
		{5620.68, 5512.65, -87.79},
		{4862.52, 5504.67, -55.68},*/

	};

	// Northrend mining profile
	WOWPOS miningWaypoints[] = {
		// Cobalt Ore
		{312.29, -5832.38, 197.96},
		{2472.85, -5437.67, 299.14},
		{2616.51, -5789.57, 315.35},
		{2578.00, -4150.00, 300.00},
		{2638.80, -3659.67, 240.57},
		{3274.53, -3459.40, 297.25},
		{2587.93, -2999.02, 111.42},
		{2207.72, -3082.16, 133.57},
		{1527.00, -5333.00, 193.25},
		{635.33, -5190.95, -62.03},
		{993.00, -5143.00, -58.82},
		{178.43, -5327.98, 328.89},
		{370.98, -5277.08, 282.63},
		{1540.45, -5149.96, 182.14},
		{1618.31, -5234.81, 181.51},
		{1016.34, -4291.02, 174.72},
		{836.52, -3215.32, 20.27},
		{1963.10, -2986.03, 155.11},
		{2209.28, -3081.64, 132.29},
		{2565.82, -2760.63, 4.82},
		{2304.81, -2744.75, 1.20},
		{2762.21, -1461.27, 45.54},
		{3115.88, -1179.87, 26.86},
		{3014.98, -841.47, -7.32},
		{2674.71, -24.69, 6.36}, 
		{2623.66, -293.00, 8.70},
		{2830.31, 200.00, 7.04}, 
		{2800.07, 1178.48, 120.89},
		{3236.08, 1966.04, 134.54},
		{4002.59, 2407.75, 348.00},
		{3432.17, 3205.75, 28.73},
		{3137.69, 3682.59, 8.73},
		{4231.03, 4442.75, 31.39},
		{4511.73, 5419.57, 28.97},
		{4144.92, 6066.96, -124.87},
		{4862.00, -601.79, 156.37},
		{3747.24, -179.99, 80.26},
		{4831.33, -321.98, 241.08},
		{2484.00, 5877.00, -23.22},
		{4313.61, 600.40, -6.80},
		{4641.00, -3944.00, 249.00},
		{4583.88, -4561.01, 195.80},
		{4219.89, -3802.78, 184.72},
		{3886.73, -3704.20, 173.55},
		{3372.00, -4777.00, 276.43},
		{4541.48, -663.22, 177.23},
		{5747.00, -1399.96, 234.43},
		{4901.78, -2193.71, 239.69},
		{5016.10, -2525.68, 257.68},
		{5123.18, -2863.68, 292.49},
		{4851.00, -2731.00, 293.00},
		{5828.00, -2202.00, 239.54},
		{6278.00, -2746.00, 303.00},
		{6316.78, -2911.96, 297.15},
		{6258.00, -2929.00, 304.00},
		{5985.00, -3271.00, 366.00},
		{6462.00, -3473.00, 388.00},
		{5180.00, -4312.00, 377.00},
		{5317.00, -4529.00, 414.00},
		{4227.00, 400.00, 17.00},
		{3372.00, 1545.00, 132.00},
		{3027.00, 4981.00, 12.00},
		{2193.00, -2536.00, 5.00},
		{2192.00, -2648.00, 1.00},
		{4915.00, -1229.00, 174.00},
		{6051.00, -1285.00, 503.00},
		{6090.00, -1314.00, 544.00},
		{3792.57, -5172.96, 119.11},
		{4013.86, -5124.25, 18.43},
		{3931.06, -1655.19, 190.64},
		{3658.20, -1475.39, 104.76},
		{3714.68, -473.04, 166.28},

		// Saronite Ore
		{5035.00, -3111.00, 278.73},
		{6802.00, -1607.00, 811.00},
		{6875.00, -1236.00, 943.00},
		{6984.00, -1217.00, 808.00},
		{6959.00, -1273.00, 1098.00},
		{6980.00, -1215.00, 928.00},
		{7120.00, -1408.00, 929.00},
		{7749.00, -270.00, 919.00},
		{7191.00, 220.00, 790.00},
		{8571.00, -1294.00, 541.00},
		{8637.00, -1181.00, 539.00},
		{8092.00, -3001.00, 1216.00},
		{6172.30, 5635.36, -23.13},
		{5518.86, 4925.29, -193.65},
		{5847.57, 5932.94, -31.23},
		{5539.19, 4315.42, -129.83},
		{5458.53, 4720.11, -199.10},
		{7098.35, 3160.02, 532.88},
		{7193.88, 2438.15, 407.57},
		{8003.78, 2816.51, 515.90},
		{8052.20, 2861.83, 510.47},
		{7867.63, 3032.75, 562.83},
		{7952.54, 3000.56, 537.98},
		{7241.25, 2401.26, 418.10},
		{5325.65, 3479.18, 381.63},
		{5441.73, 3328.17, 402.83},
		{4958.05, 3445.47, 362.95},
		{5014.58, 2944.42, 373.98},
		{7207.42, 2280.40, 511.42},
		{7914.92, 2481.86, 406.19},
		{6791.83, 2349.46, 441.51},
		{7254.57, 3169.37, 534.13},
		// {7683.79, 2831.52, 450.51},
		{7854.87, 2981.88, 549.14},
		{7116.09, 1451.26, 315.69},
		{7031.32, 1338.12, 304.30},
		{7825.91, 1811.37, 347.38},
		{8513.55, 2072.66, 628.90},
		{7683.85, 2830.38, 450.45},
		{6030.34, 2448.87, 515.95},
		{4537.97, 3184.14, 371.55},
		{5214.79, 3082.61, 393.26},
		{5012.12, 2881.31, 384.77},
		{4986.04, 2789.84, 378.98},
		{5210.48, 2604.76, 392.09},
		{4893.10, 2409.18, 318.59},
		{4856.15, 3873.40, 364.75},
		{4697.68, 2354.26, 341.76},
		{7938.56, 3480.98, 693.06},
		{8368.55, 3147.72, 606.77},
		{8440.31, 1869.38, 691.27},
		{8444.49, 1674.98, 705.68},
		{8578.29, 1430.42, 590.37},
		{6281.07, 4224.68, -41.76},
		{5603.08, 5430.62, -126.32},
		//  {5754.75, 6002.60, -25.14

		// Titanium Ore
		{4008.14, 1998.91, 357.82},
		{5092.30, 2738.88, 388.95},
		{4640.63, 3326.02, 343.93},
		{5358.70, 4514.48, -127.60},
		{7774.88, 1058.42, 395.62},
		{7205.79, 2283.14, 510.01},
	};

	const INT mapID = 571;

	WOWPOS *waypoints = NULL;

	CHAR *herbingNodeNames[] = {"Lichbloom", "Adder's Tongue", "Talandra's Rose", "Icethorn", "Frost Lotus", "Goldclover", "Frozen Herb"};
	CHAR *miningNodeNames[] = {"Cobalt Deposit", "Rich Cobalt Deposit", "Saronite Deposit", "Rich Saronite Deposit", "Titanium Vein", "Rich Titanium Vein"};

	CHAR **nodeNames = NULL;

	CHAR *lootNames[] = {"Cobalt Ore", "Saronite Ore", "Titanium Ore"};
	CHAR szBuffer[256], name[256];

	UINT interactCount = 0;
	BOOL shouldHide = FALSE;

	DWORD gatherType = 1; // 1 for mining, 0 for herbing.
	DWORD ret = NULL;

	UINT numNodeNames = 6;  
	UINT numLootNames =  0; sizeof(lootNames) / sizeof(*lootNames);

	UINT waypointListSize = 0;
	UINT curWaypointIndex = 0; // current waypoint index.

	UINT *nonWaypoints = NULL;
	UINT nonWaypointsIndex = 0;

	WGUID NearbyPlayers[64];
	Object nodes[500];
	
	bool flyMode = false;
	float hackSpeed = 120;

	Thread *currentThread = Thread::GetCurrent();
	
	// Are we herbing or mining?
	gatherType = SendDlgItemMessageA(g_hwMainWindow, IDC_MAIN_GATHERING_COMBO_MODE, CB_GETCURSEL, 0, 0);				
	SendDlgItemMessageA(g_hwMainWindow, IDC_MAIN_GATHERING_COMBO_MODE, CB_GETLBTEXT, (WPARAM)gatherType, (LPARAM)name); 
	gatherType = _stricmp(name, "Herbalism") != 0;

	if (gatherType == 1) // Mining
	{
		numNodeNames = 6;
		nodeNames = miningNodeNames;
		waypoints = miningWaypoints;
		waypointListSize = sizeof(miningWaypoints) / sizeof(WOWPOS);
	}
	else if(gatherType == 0) // Herbalism
	{
		numNodeNames = 7;
		nodeNames = herbingNodeNames;
		waypoints = herbingWaypoints;
		waypointListSize = sizeof(herbingWaypoints) / sizeof(WOWPOS);
	}

	nonWaypoints = (UINT *)malloc(sizeof(UINT) * waypointListSize);

	// Setup code so we can call lua ingame.
	Memory::Endscene.Start();

	sprintf(szBuffer, "Dinner Gatherer set to run for %d minutes", nTimeLimit);
	LogAppend(szBuffer);
	LogFile(szBuffer);

	Gatherer = InitGatherer();
	srand((UINT)time(NULL));
	
	Lua::DoString("DinnerDank.start = GetTime()");
	// TODO: FreeInventorySlots sometimes doesn't return properly (related to Lua::GetText).
	/*if (FreeInventorySlots() == 0)
	{
		LogFile("Inventory is full");
		log("Inventory is full.");
		SetCurrentThreadTask(STOPPED);
	}*/

	InitializeBlacklist();
	if (LocalPlayer.mapId() != mapID)
	{
		// Stop 
		currentThread->stop();
		LogFile("We are not on the same map as the gathering profile");
	}
	
	// Initialize waypoints and pathing.
	curWaypointIndex = GetNextClosestWaypoint(waypoints, waypointListSize, nonWaypoints, nonWaypointsIndex);
	nonWaypoints[nonWaypointsIndex] = curWaypointIndex;

	nonWaypointsIndex = (nonWaypointsIndex + 1) % waypointListSize;
	while(currentThread->running())
	{
		if (!LocalPlayer.isAlive() || !WoW::InGame()) break;
		
		if (m_hideState == TRUE)
			m_hideState = FALSE;

		// This stops the weird falling when the bot starts to gather from a node.
		if (Gatherer.GatheringState != GSTATE_GATHERING && Gatherer.GatheringState != GSTATE_LOOTING)
		{
			if (!LocalPlayer.isFlying() && flyMode)
			{
				Hack::Movement::SetSwimSpeed(hackSpeed);
				//SetMovementSpeed(hackSpeed);
				//FreezeZ(true);
				Hack::Movement::WeirdFly(true, hackSpeed);
			}
		}

		Hack::Collision::isM2Enabled();
		if (!Hack::Collision::isM2Enabled() 
		 || !Hack::Collision::isWMOEnabled() 
		 || Hack::Collision::GetPlayerWidth() > 0)
		{
			Hack::Collision::All(true);
			Hack::Collision::SetPlayerWidth(0);
		}

		switch(Gatherer.GatheringState)
		{
		case GSTATE_SEARCHING:

			// TODO: Class detection.
			if (!LocalPlayer.isStealthed())
			{
				/*if ((float)LocalPlayer.UnitField.Health/(float)LocalPlayer.UnitField.Max_Health < 0.5 && InCombat())
				{
					vlog("Health %: %0.2f", (float)LocalPlayer.UnitField.Health/(float)LocalPlayer.UnitField.Max_Health);
					CastSpellByName("Vanish");

					SetSwimSpeed(hackSpeed);
					WeirdFlyHack(true, hackSpeed);
				}*/

				if (!LocalPlayer.inCombat())
				{
					Spell::Cast("Stealth");
					if (!HasAuraBySpellId(LocalPlayer.Obj, 768))
						Spell::Cast("Cat Form");

					Sleep(200);
					Spell::Cast("Prowl");
					LogFile("Casted Stealth");

					if (flyMode)
					{
						Hack::Movement::SetSwimSpeed(hackSpeed);
						Hack::Movement::WeirdFly(true, hackSpeed);
					}
				}

				
				//SetMovementSpeed(hackSpeed);
				//FreezeZ(true);
			}

				if (LocalPlayer.health()/LocalPlayer.maxHealth() < 0.5 && LocalPlayer.inCombat())
				{
					vlog("Health %: %0.2f", LocalPlayer.health()/LocalPlayer.maxHealth());
					Spell::Cast("Vanish");
				}
			
			interactCount = 0;
			Gatherer.nodesInArea = GetLocalNodes(nodeNames, numNodeNames, nodes);
			for (int i = 0; i < Gatherer.nodesInArea; i++)
			{
				if (ValidObject(nodes[i]) && !BlacklistCheck(nodes[i]) && !PlayerBlacklistCheck(nodes[i]))
				{
					Gatherer.currentNode = nodes[i];
					Gatherer.lastNode = Gatherer.currentNode;
					Gatherer.GatheringState = GSTATE_MOVING_TO_NODE;
					//Lua::DoString("dstate = \"GSTATE_MOVING_TO_NODE\"");
					break;
				}
			}
			
			if (GetDistance3D(LocalPlayer.pos(), waypoints[curWaypointIndex]) > 3)
			{
				if (GetCTMAction() == 13)
				{
					if (GetDistance(waypoints[curWaypointIndex]) < 100) 
					{
						Hack::Movement::StopFall();
						Hack::Movement::Teleport(waypoints[curWaypointIndex]); // ** Need to change teleport
						UpdatePosition();
						Sleep(150);
					}
					else
					{
						ClickToMove(waypoints[curWaypointIndex]);
					}


					if (LocalPlayer.isCasting()) Spell::StopCasting();
					sprintf_s(szBuffer, "Moving to waypoint %d at (%0.2f, %0.2f, %0.2f)", curWaypointIndex, waypoints[curWaypointIndex].X, waypoints[curWaypointIndex].Y, waypoints[curWaypointIndex].Z);

					log(szBuffer);
					LogFile(szBuffer);

					//Sleep(1500);
					Gatherer.GathererTimer = GetTickCount();
					ret = WaitOnWoW();
					if (ret == WOW_UNRESPONSIVE || ret == WOW_CRASHED)
					{
						log("WoW is unresponsive or it has crashed. Stopping");
						Thread::GetCurrent()->stop();
					}
				}
			}

			if (GetLocalNodes(nodeNames, numNodeNames, nodes) == 0 && GetDistance3D(LocalPlayer.pos(), waypoints[curWaypointIndex]) <= 3)
			{
				curWaypointIndex = GetNextClosestWaypoint(waypoints, waypointListSize, nonWaypoints, nonWaypointsIndex);
				nonWaypoints[nonWaypointsIndex] = curWaypointIndex;

				nonWaypointsIndex = (nonWaypointsIndex + 1) % waypointListSize;
			}

			break;
			
		case GSTATE_MOVING_TO_NODE:
			if (!ValidObject(Gatherer.currentNode) || !ValidCoord(Gatherer.currentNode.Pos))
			{
				log("GSTATE_MOVING_TO_NODE: Invalid node");
				Gatherer.GatheringState = GSTATE_SEARCHING;
			}

			if (PlayersInArea() > 0)
			{
				Gatherer.GatheringState = GSTATE_SEARCHING;
				PlayerBlackListNode(Gatherer.currentNode, 10000);
				// Add timed blackout list for specific nodes
			}

			if (GetDistance3D(Gatherer.currentNode.Pos, LocalPlayer.pos()) > 2)
			{
				if (GetDistance(Gatherer.currentNode.Pos) < 100)
				{
					Hack::Movement::StopFall();
					Hack::Movement::Teleport(Gatherer.currentNode.Pos);
					UpdatePosition();
				}
				else
				{
					ClickToMove(Gatherer.currentNode.Pos);
				}

				sprintf_s(szBuffer, "Moving to %s, at (%0.2f, %0.2f, %0.2f)", Gatherer.currentNode.Name, Gatherer.currentNode.X, Gatherer.currentNode.Y, Gatherer.currentNode.Z);
				//AddChatMessage(szBuffer, "DGather", TurqoiseMessage);
			}
			else
			{
				Hack::Movement::StopFall();
				Hack::Movement::Teleport(Gatherer.currentNode.Pos);
				Gatherer.GathererTimer = GetTickCount();
				Gatherer.GatheringState = GSTATE_GATHERING;
			}
			
			break;

		case GSTATE_GATHERING:
			Gatherer.currentNode = DGetObjectEx(Gatherer.currentNode.BaseAddress, BaseObjectInfo | GameObjectInfo | LocationInfo | NameInfo);
			if (!ObjectInObjMgr(Gatherer.currentNode) || !ValidObject(Gatherer.currentNode) || PlayerBlacklistCheck(Gatherer.currentNode) || BlacklistCheck(Gatherer.currentNode) || !ValidCoord(Gatherer.currentNode.Pos))
			{
				Gatherer.GatheringState = GSTATE_SEARCHING;
			}

			if ((GetTickCount() - Gatherer.GathererTimer) / 1000 > 12 || IsLocalPlayerBeingAttacked()) 
			{
				BlackListNode(Gatherer.currentNode);
				Gatherer.GatheringState = GSTATE_SEARCHING;
			}
			
			if (PlayersInArea() > 0)
			{
				Gatherer.GatheringState = GSTATE_SEARCHING;
				PlayerBlackListNode(Gatherer.currentNode, 10000);
				// Add timed blackout list for specific nodes
			}

			if (!LocalPlayer.isCasting())
			{
				LogFile("Not casting...");
				if (!Loot::isWindowOpen() && interactCount < 5)
				{
					/*if (GetDistance3D(Gatherer.currentNode.Pos, LocalPlayer.Pos) > 15)
					{
						sprintf(szBuffer, "Got teleported away from node, moving back.");
						log(szBuffer);

						Gatherer.GatheringState = GSTATE_MOVING_TO_NODE;
						break;
					}*/

					sprintf_s(szBuffer, "Interacting with %s", Gatherer.currentNode.Name);
					//AddChatMessage(szBuffer, "DGather", TurqoiseMessage);
					log(szBuffer);
					LogFile(szBuffer);
					
					UpdatePosition();
					Lua::DoString("CancelUnitBuff(\"player\", \"Cat Form\")");
					Interact(Gatherer.currentNode.BaseAddress);
					interactCount++;
					Sleep(500);
				}

				if (Loot::isWindowOpen())
				{
					LogFile("Loot window is open");
					Gatherer.GatheringState = GSTATE_LOOTING;
					Gatherer.GathererTimer = GetTickCount();
					interactCount = 0;
				}

				Sleep(200);
			}
			break;

		case GSTATE_LOOTING:
			if (!ValidObject(Gatherer.currentNode) || !ObjectInObjMgr(Gatherer.currentNode))
			{
				if (PlayersInArea() > 0)
				{
					Gatherer.GatheringState = GSTATE_SEARCHING;
					PlayerBlackListNode(Gatherer.currentNode, 10000);
					break;
					// Add timed blackout list for specific nodes
				}

				Gatherer.nodesCollected++;
				Gatherer.GatheringState = GSTATE_SEARCHING;
			}
				
			Sleep(150);
			if (Loot::isWindowOpen() && Loot::getItemCount() > 0)
			{
				LogFile("Loot window is open with items to collect");
				for (int x = Loot::getItemCount(); x > 0; x--)
				{
					Loot::getSlotName(x, name, 256);
					for (int y = 0; y < numLootNames; y++)
					{
						if (strstr(name, lootNames[y]) || Loot::isSlotCoin(x))
						{
							Loot::slot(x);
							sprintf_s(szBuffer, "Looted %s", name);
							log(szBuffer);
							LogFile(szBuffer);
						}
					}
				}

				if (numLootNames == 0)
				{
					for (int x = Loot::getItemCount(); x > 0; x--)
					{
						Loot::getSlotName(x, name, 256);
						Loot::slot(x);
						sprintf_s(szBuffer, "Looted %s", name);
						log(szBuffer);
						LogFile(szBuffer);
					}
				}


				// 
				Gatherer.nodesCollected++;
				Loot::close();
				
				if (LocalPlayer.isCasting())
					Spell::StopCasting();

				/*if (FreeInventorySlots() == 0)
				{
					log("Inventory is full.");
					LogFile("Inventory is full");
					SetCurrentThreadTask(STOPPED);
				}*/

				Sleep(1400 + (rand() % 200));

				Lua::DoString("for bag = 0,4,1 do for slot = 1, GetContainerNumSlots(bag), 1 do local name = GetContainerItemLink(bag,slot); if name and string.find(name,\'Crystallized\') then UseContainerItem(bag,slot) end end end");
				Gatherer.GatheringState = GSTATE_SEARCHING;
				//Lua::DoString("dstate = \"GSTATE_SEARCHING\"");
				if (!LocalPlayer.isStealthed())
					Spell::Cast("Stealth");

				break;
				// Lua::DoString("dstate = \"GSTATE_SEARCHING\"");
			}

			else if ((GetTickCount() - Gatherer.GathererTimer) / 1000 > 6)
			{
				BlackListNode(Gatherer.currentNode);
				Gatherer.GatheringState = GSTATE_SEARCHING;
			}
			
				
			if ((GetTickCount() - Gatherer.StartGatherTime)/1000/60 >= nTimeLimit) // Stop gathering and take appropriate action after time limit.
				currentThread->stop(); // Current thread task is set to stop. 

			break;
		}
		
		
		/*if (PlayersInArea(NearbyPlayers) > 0 && !shouldHide)
		{
			//GHide(NearbyPlayers);
			curWaypointIndex = (curWaypointIndex + 1) % waypointListSize;
			log("Players nearby, moving to next area.");
			// Lua::DoString("dstate = \"GSTATE_SEARCHING\"");
			Gatherer.GatheringState = GSTATE_SEARCHING;
			shouldHide = TRUE;
			continue;
		}

		/*if (InCombat())
		{
			curWaypointIndex = (curWaypointIndex + 1) % waypointListSize;
			Gatherer.GatheringState = GSTATE_SEARCHING;
		}

		if (shouldHide)
			shouldHide = FALSE;*/

		Sleep(200);
	}

	SetDlgItemText(g_hwMainWindow, IDC_MAIN_GATHERING_BUTTON_START, "Start Gathering");
	sprintf(szBuffer, "Stopped after collecting %d nodes", Gatherer.nodesCollected);
	LogAppend(szBuffer);

	if (LocalPlayer.isFlying() && flyMode)
		Hack::Movement::Fly(false, 7);
	
	Hack::Collision::All(false);
	Hack::Movement::SetSwimSpeed(7);

	if (Hack::Collision::playerWidth > 0)
		Hack::Collision::SetPlayerWidth(Hack::Collision::playerWidth);
	else
		Hack::Collision::SetPlayerWidth(0.31);

	vlog("WoWInfoLoop: Delete thread");
	currentThread->exit();
	if (g_DFish_Logout)
		Logout();

	return Gatherer.nodesCollected;
}