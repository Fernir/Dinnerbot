#pragma once

#include <Windows.h>

#include "..\Common\Common.h"

struct SCameraInfo
{
	DWORD	dwFoo1[2];
	float	fPos[3];
	float	fViewMat[3][3];
	DWORD	dwFoo2[2];
	float	fFov;
};

typedef struct 
{
	WOWPOS Pos;
	WGUID FollowGUID;
	DWORD ActiveCamera;
	SCameraInfo CameraInfo;
} CGCamera;


CGCamera GetActiveCamera();
CGCamera UpdateCamera(CGCamera Camera);

VOID SpectateObject(Object Obj);

BOOL WtS(float x, float y, float z, POINT *pt);