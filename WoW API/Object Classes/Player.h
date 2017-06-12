#pragma once

#include <Windows.h>

#include "Object.h"
#include "Unit.h"

BOOL ChangeDinnerState(DWORD dwState);

class CPlayer: public CUnit
{
public:
	CPlayer();
	~CPlayer();

	virtual bool isType() { return this->isPlayer(); }
};
