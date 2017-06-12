#pragma once

#include <Windows.h>

// g_CInputControl = 0x00C24954

class CInputControl
{
private:
	static const DWORD class_WoW = 0x00C24954;

public:

	static void updatePlayer();
	static void setMovementFlags();
};

DWORD GetCInputControl();

VOID CInputControlUpdatePlayer();