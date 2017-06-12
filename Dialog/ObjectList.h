#pragma once

#include <Windows.h>
#include <CommCtrl.h>

#include "Main\Structures.h"

#define OBJECTS 0
#define PLAYERS 1
#define UNITS   2
#define GOBJS   3

typedef struct 
{
  WGUID *PlayerList;
  size_t used;
  size_t size;
} Radar_Players;

typedef struct
{
	WGUID TreeObject;
	HTREEITEM TreeItem;
	HTREEITEM Parent;
	INT nChildIndex;
} TVI_Object;

VOID GetPlayerList();

extern Radar_Players PlayerList;

LRESULT CALLBACK ObjectListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);