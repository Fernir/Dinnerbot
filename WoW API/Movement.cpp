#include "Movement.h"

#include "Game\Interactions.h"
#include "Geometry\Coordinate.h"
#include "Object Classes\Player.h"

FLOAT RotateToAngleEx(FLOAT fRotation, FLOAT fDeviation, INT nTime);

BOOL PlayerIsFacingPos(WOWPOS Pos)
{
	return (UINT)LocalPlayer.pos().Rotation == (UINT)GetRotationToFace(Pos.X, Pos.Y);
}

VOID TurnToObject(Object Target)
{
	if (ValidCoord(Target.Pos))
		TurnToPos(Target.Pos);
}

VOID TurnToPos(WOWPOS pos)
{
	FLOAT fRotationToFace = 0;

	fRotationToFace = GetRotationToFace(pos.X, pos.Y);
	SetPlayerRotation(fRotationToFace);
}

DWORD GetPlayerMovementState()
{
	DWORD dwState = NULL;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.base() + Offsets::MovementState), &dwState, sizeof(dwState), 0))
		return NULL;

	return dwState;
}

VOID TurnToTarget()
{
	if (!WoW::InGame()) return;

	TurnToObject(GetCurrentTarget(LocationInfo));
}

VOID QuickWalk()
{
	SendKey('W', WM_KEYDOWN);
	SendKey('W', WM_KEYUP);
}

VOID SpamKey()
{
	// Get current thread.
	Thread *thread = Thread::GetCurrent();

	while (thread->running() && WoW::InGame())
	{
		SendKey(VK_UP, WM_KEYDOWN);
		SendKey(VK_UP, WM_KEYUP);
		Sleep(100);
	}
	
#ifndef RELEASE
	Thread::DeleteCurrent("Radar::CreateSettings");
#else
	Thread::DeleteCurrent();
#endif
	// Delete current thread.
#ifndef RELEASE
	thread->exit();
#else
	Thread::Delete(thread);
#endif
}


VOID QuickJump()
{
	SendKey('W', WM_KEYDOWN);
	SendKey(0x20, WM_KEYDOWN);
	SendKey(0x20, WM_KEYUP);
	SendKey('W', WM_KEYUP);
	Sleep(100);
}

VOID GlitchRun()
{
	Thread *thread = Thread::GetCurrent();
	DWORD timer = NULL;
	CHAR buff[256];

	while (thread->running() && WoW::InGame())
	{
		/*
		SendKey('W', WM_KEYDOWN);
		Sleep(20);
		SendKey('W', WM_KEYUP);*/

		timer = GetTickCount();
		SendKey('E', WM_KEYDOWN);
		for(int x = 0; x < 5; x++)
		{
			if (thread->stopped())
			{ 
				SendKey('E', WM_KEYUP);
				return;
			}

			SendKey('S', WM_KEYDOWN);
			Sleep(58);
			SendKey('S', WM_KEYUP);
			Sleep(36);
		}
		

		SendKey('E', WM_KEYUP);
		sprintf_s(buff, "time: %d", GetTickCount() - timer);
		log(buff);
		SendKey('Q', WM_KEYDOWN);
		for(int x = 0; x < 5; x++)
		{
			if (thread->stopped()) 
			{
				SendKey('Q', WM_KEYUP);
				return;
			}

			SendKey('S', WM_KEYDOWN);
			Sleep(58);
			SendKey('S', WM_KEYUP);
			Sleep(36);
		}

		SendKey('Q', WM_KEYUP);

		/*SendKey('W', WM_KEYDOWN);
		SendKey('W', WM_KEYUP);
		Sleep(20);
		SendKey(0x20, WM_KEYDOWN);
		SendKey(0x20, WM_KEYUP);s
		Sleep(20);*/
	}

	thread->exit();
}

VOID NudgeJump()
{
	SendKey(0x20, WM_KEYDOWN);
	SendKey('W', WM_KEYDOWN);
	SendKey('W', WM_KEYUP);
	SendKey(0x20, WM_KEYUP);
}

VOID QuickBJump()
{
	SendKey('S', WM_KEYDOWN);
	SendKey(0x20, WM_KEYDOWN);
	SendKey(0x20, WM_KEYUP);
	SendKey('S', WM_KEYUP);
	Sleep(100);
}

BOOL CompareRotations(FLOAT fRot1, FLOAT fRot2, FLOAT fExceptionRange)
{
	if (fabs((fRot2 - fRot1)) > fExceptionRange)
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL Traceline(WOWPOS start, WOWPOS end, WOWPOS *result, UINT flags)
{
	FLOAT  distance = 1.0f;

	DWORD subroutineAddress = NULL;
	DWORD tracelineAddress = Offsets::Traceline; // stdcall
	DWORD totalSize = NULL;

	DWORD distanceReturn = NULL;
	DWORD resultAddress = NULL;
	DWORD startAddress = NULL;
	DWORD endAddress = NULL;

	HANDLE thread = NULL;

	if (!WoW::InGame())
		return FALSE;

	end.Z += 1.3f;
	start.Z += 1.3f;
	// Subroutine to inject.
	BYTE Sub_Traceline[] = {
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	

			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, dwTraceline
			0x6A, 0x00,							// PUSH 0 (Optional)
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH flags
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH distance
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH result
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH start
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH end
			0xFF, 0xD7,							// CALL EDI
			0xA2, 0x00, 0x00, 0x00, 0x00,		// MOV [dwResult], AL
			0x81, 0xC4, 0x18, 0x00, 0x00, 0x00, // ADD ESP, 18       
			0xC3								// RETN
	};

	totalSize = sizeof(Sub_Traceline) + (sizeof(FLOAT) * 3) * 3 + sizeof(FLOAT); // Allocate for sub, 3 wowpos structs, and a float.
	subroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, totalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (subroutineAddress == NULL)
		return false;

	startAddress   = subroutineAddress	+ sizeof(Sub_Traceline);
	endAddress	   = startAddress		+ (sizeof(FLOAT) * 3);
	resultAddress  = endAddress			+ (sizeof(FLOAT) * 3);
	distanceReturn = resultAddress		+ (sizeof(FLOAT) * 3);

	wpm(startAddress, &start,		sizeof(FLOAT) * 3);
	wpm(distance,	  &end,			sizeof(FLOAT) * 3);
	wpm(distance,	  &distance,	sizeof(FLOAT));

	memcpy(&Sub_Traceline[24], &tracelineAddress, sizeof(tracelineAddress));
	memcpy(&Sub_Traceline[31], &flags,			  sizeof(flags));
	memcpy(&Sub_Traceline[36], &distanceReturn,	  sizeof(distanceReturn));
	memcpy(&Sub_Traceline[41], &resultAddress,	  sizeof(resultAddress));
	memcpy(&Sub_Traceline[46], &startAddress,	  sizeof(startAddress));
	memcpy(&Sub_Traceline[51], &endAddress,		  sizeof(endAddress));
	memcpy(&Sub_Traceline[58], &resultAddress,	  sizeof(resultAddress));

	if (!wpm(subroutineAddress, &Sub_Traceline, sizeof(Sub_Traceline)))
		return FALSE;

	// Run the injected subroutine.
	SuspendWoW();
	thread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)subroutineAddress, NULL, NULL, NULL);
	if (thread == NULL)
		return false;

	// Wait for thread to execute.
	WaitForSingleObject(thread, 1000);
	CloseHandle(thread);
	ResumeWoW();

	// Get return value from calling traceline.
	rpm(resultAddress, result, sizeof(FLOAT) * 3);
	
	// Free memory and return.
	if (!VirtualFreeEx(WoW::handle, (LPVOID)subroutineAddress, totalSize, MEM_DECOMMIT))
		return FALSE;

	return !ValidCoord(*result);
}

VOID ReleaseAllKeys()
{
	SendKey('W', WM_KEYUP);
	SendKey('A', WM_KEYUP);
	SendKey('S', WM_KEYUP);
	SendKey('D', WM_KEYUP);
}

VOID FollowTarget(CObject *obj)
{
	WOWPOS lastPos;

	ChangeDinnerState(D_FOLLOWING);
	while (ValidCoord(obj->pos()) && ValidGUID(GetCurrentTargetGUID()))
	{
		if (obj->distance() > 1 || obj->distance(lastPos) > 0.1)
		{
			ClickToMove(obj->pos());
		}
		
		// Is passed with Location update flags.
		obj->update();
		lastPos = obj->pos();
		Sleep(100);
	}

	ChangeDinnerState(D_IDLE);
}

BOOL MoveToPos(WOWPOS pos)
{
	if (LocalPlayer.distance(pos) > 1 && (GetCTMState() == Offsets::CTM_Stopped))
		return ClickToMove(pos);

	return FALSE;
}

BOOL SetRotation(DWORD base, float rot)
{
	return WriteProcessMemory(WoW::handle, (LPVOID)(base + OBJECT_ROT), &rot ,sizeof(rot), NULL);
}

VOID MimicTargetActions()
{
	Object target;

	while (1)
	{
		target = GetCurrentTarget(UnitFieldInfo | LocationInfo);

		if (!ValidObject(target)) break;

		ClickToMove(target.Pos);
		Sleep(50);
	}
	
	/*
	sprintf_s(szBuffer, "state: 0x%x", GetMovementState(GetCurrentTarget(UnitFieldInfo).BaseAddress));
	log(szBuffer);

	itoa(GetMovementState(GetCurrentTarget(UnitFieldInfo).BaseAddress) & 0x1000, out, 2);
	sprintf_s(szBuffer, "state op: %s", out);
	log(szBuffer);*/
	
}

VOID FollowObject()
{
	Object Target;
	WOWPOS LastPos;
	//BOOL WasWalking = FALSE;

	Target = GetCurrentTarget(LocationInfo);
	LastPos = Target.Pos;

	ChangeDinnerState(D_FOLLOWING);
	while (ValidCoord(Target.Pos) && ValidObject((Target = GetCurrentTarget(LocationInfo))))
	{
		if (LocalPlayer.distance(Target) > 1 && (GetCTMState() == Offsets::CTM_Stopped || GetDistance(Target.Pos, LastPos) > 0.1))
		{
			Target.Pos.Z += 50;
			ClickToMove(Target.Pos);
		}
		
		LastPos = Target.Pos;
		Sleep(100);
	}

	ChangeDinnerState(D_IDLE);

#ifndef RELEASE
	Thread::DeleteCurrent("FollowObject");
#else
	Thread::DeleteCurrent();
#endif
}

DWORD GetCTMAction()
{
	DWORD dwBuffer = NULL;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::CTM_Base + Offsets::CTM_Action), &dwBuffer, sizeof(dwBuffer), 0)))
		return -1;

	return dwBuffer;
}