#pragma once
#include <Windows.h>

#include "Common\Objects.h"

enum GatheringStates
{
	GSTATE_SEARCHING = 0x01,
	GSTATE_WAITING = 0x02,
	GSTATE_LOOTING = 0x03,
	GSTATE_MOVING_TO_NODE = 0x04,
	GSTATE_GATHERING = 0x05
};

typedef struct DGatherer
{
	ULONGLONG StartGatherTime;
	ULONGLONG GathererTimer;

	DWORD  GatheringState;

	Object lastNode;
	Object currentNode;

	UINT    nodesCollected;
	UINT    nodesInArea;
} DGatherer;

UINT Gather(UINT nTimeLimit);

BOOL StartDinnerGather(INT nTimeLimit);
BOOL GetGathererNodeName(CHAR *szName, SIZE_T maxSize);

DWORD GetGathererState();
ULONGLONG GetGatherTime(DWORD set);