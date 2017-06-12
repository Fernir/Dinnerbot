#include "Interactions.h"

VOID PlayerJump()
{
	SendKey(0x20, WM_KEYDOWN);
	SendKey(0x20, WM_KEYUP);
}

BOOL Turn(DIRECTION Direction)
{
	SendKey(Direction, WM_KEYDOWN);
	return TRUE;
}

BOOL Move(DIRECTION Direction)
{/*
	if (IsPlayerMoving())
	{
		SendKey(Direction, WM_KEYUP);
	}
	else
	{
		SendKey(Direction, WM_KEYDOWN);
	}*/

	return FALSE;
}

INT SendKey(CHAR cKey, DWORD dwKey)
{
	PostMessage(g_hwWoW, dwKey, cKey, 0);
	return TRUE;
}