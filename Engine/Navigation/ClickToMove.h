#pragma once

#include <Windows.h>
#include "Main\Structures.h"

namespace Engine
{
	namespace Navigation
	{
		enum eCTM_States
		{
			CTM_Moving = 0x4,
			CTM_Stopped = 0xD,
			CTM_FaceTarget = 0x1,
			CTM_Face = 0x2, // Just turns north, not good.
			CTM_Stop_ThrowsException = 0x3,
			CTM_Move = 0x4,
			CTM_NpcInteract = 0x5,
			CTM_Loot = 0x6,
			CTM_ObjInteract = 0x7,
			CTM_FaceOther = 0x8,
			CTM_Skin = 0x9,
			CTM_AttackPosition = 0xA,
			CTM_AttackGuid = 0xB,
			CTM_ConstantFace = 0xC,
			CTM_None = 0xD,
			CTM_Attack = 0x10,
			CTM_Idle = 0x13,
		};

		bool ctm(float X, float Y, float Z, byte action);

		bool face(float x, float y, float z);
		bool face(const WOWPOS &p);

		bool move(float X, float Y, float Z);
		bool move(const WOWPOS &p);

		bool setTolerance(float tol);

		bool idle();
		DWORD GetAction();
	}
}