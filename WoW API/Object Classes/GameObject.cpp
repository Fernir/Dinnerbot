#include "GameObject.h"
#include "..\..\Main\Constants.h"

bool CGameObject::isFishingBobber()
{
	return this->displayId() == FISHING_BOBBER_DISPLAYID;
}

bool CGameObject::isFishingBobberSet()
{
	return this->isFishingBobber() && this->otherFlags() != 1;
}