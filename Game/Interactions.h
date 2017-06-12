#pragma once

#include <Windows.h>

#include "..\WoW API\Movement.h"

INT  SendKey(CHAR cKey, DWORD dwKey);
BOOL Move(DIRECTION Direction);
BOOL Turn(DIRECTION Direction);
VOID PlayerJump();
