#pragma once

#include "..\Common\Common.h"
#include "..\Common\Objects.h"

// Directions
#define FORWARDS 'W'
#define LEFTS 'Q'
#define RIGHTS 'E'
#define LEFT 'A'
#define RIGHT 'D'
#define BACKWARDS 'S'

typedef CHAR DIRECTION;

DWORD GetCTMAction();

VOID TurnToPos(WOWPOS pos);

FLOAT RotateToAngle(FLOAT fRotation);

BOOL PlayerIsFacingPos(WOWPOS Pos);
BOOL Rotate(FLOAT fRadians, BOOL bTurnRight);
BOOL ComparePositions(WOWPOS Pos1, WOWPOS Pos2);
BOOL Traceline(WOWPOS start, WOWPOS end, WOWPOS *result, UINT flags);

BOOL MoveTo(FLOAT fLocationX, FLOAT fLocationY);
BOOL MoveToPos(WOWPOS pos);

VOID SpamKey();
VOID QuickBJump();
VOID QuickJump();
VOID NudgeJump();
VOID QuickWalk();
VOID GlitchRun();
VOID FollowObject();
VOID TurnToTarget();
VOID MimicTargetActions();
VOID TurnToObject(Object);