#include "Player.h"
#include "Main\Main.h"
#include "Memory\Memory.h"

DWORD g_dwDinnerState = NULL;

BOOL ChangeDinnerState(DWORD dwState)
{ 
	// LOOOOOOOOOL
	DWORD dwBuff = g_dwDinnerState;	
	g_dwDinnerState = dwState;

	if (g_dwDinnerState == dwBuff)
		return FALSE;

	return TRUE;
}

