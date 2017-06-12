#include "Camera.h"
#include "..\Geometry\Coordinate.h"
/*
CGCamera GetActiveCamera()
{
	CGCamera ActiveCamera;

	ReadProcessMemory(WoW::handle, (LPVOID)(Camera_Pointer), &ActiveCamera.ActiveCamera, sizeof(ActiveCamera.ActiveCamera), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_Offset), &ActiveCamera.ActiveCamera, sizeof(ActiveCamera.ActiveCamera), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_X), &ActiveCamera.Pos.X, sizeof(ActiveCamera.Pos.X), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_Y), &ActiveCamera.Pos.Y, sizeof(ActiveCamera.Pos.Y), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_Z), &ActiveCamera.Pos.Z, sizeof(ActiveCamera.Pos.Z), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_Follow_GUID), &ActiveCamera.FollowGUID, sizeof(ActiveCamera.FollowGUID), 0);
	return ActiveCamera;
}

/*
BOOL WtS(float x, float y, float z, POINT *pt)
{
	SCameraInfo camera;
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	CHAR szBuffer[64];

	GetWindowRect(hDesktop, &desktop);

	ReadProcessMemory(WoW::handle, (LPVOID)(GetActiveCamera().ActiveCamera), &camera, sizeof(camera), NULL);
	RECT rc = {0, 0, desktop.right, desktop.bottom};
	sprintf(szBuffer, "%d, %d\n", desktop.right, desktop.bottom);
	OutputDebugString(szBuffer);
	
	sprintf(szBuffer, "%f, %f, %f\n", camera.fPos[0], camera.fPos[1], camera.fPos[2]);
	OutputDebugString(szBuffer);
	//CVec3 vDiff = vWoWPos - camera.vPos;
	float fDiff[3];
	fDiff[0] = x-camera.fPos[0];
	fDiff[1] = y-camera.fPos[1];
	fDiff[2] = z-camera.fPos[2];

	//float fProd = vDiff*camera.matView[0];
	float fProd = 
		fDiff[0]*camera.fViewMat[0][0] +
		fDiff[1]*camera.fViewMat[0][1] +
		fDiff[2]*camera.fViewMat[0][2];

	if( fProd < 0 )
		return FALSE;
	OutputDebugString("sexing");

	//CVec3 vView = vDiff*!camera.matView;
	float fInv[3][3];
	fInv[0][0] = camera.fViewMat[1][1]*camera.fViewMat[2][2]-camera.fViewMat[1][2]*camera.fViewMat[2][1];
	fInv[1][0] = camera.fViewMat[1][2]*camera.fViewMat[2][0]-camera.fViewMat[1][0]*camera.fViewMat[2][2];
	fInv[2][0] = camera.fViewMat[1][0]*camera.fViewMat[2][1]-camera.fViewMat[1][1]*camera.fViewMat[2][0];
	float fDet = camera.fViewMat[0][0]*fInv[0][0]+camera.fViewMat[0][1]*fInv[1][0]+camera.fViewMat[0][2]*fInv[2][0];

	float fInvDet = 1.0f / fDet;
	fInv[0][1] = camera.fViewMat[0][2]*camera.fViewMat[2][1]-camera.fViewMat[0][1]*camera.fViewMat[2][2];
	fInv[0][2] = camera.fViewMat[0][1]*camera.fViewMat[1][2]-camera.fViewMat[0][2]*camera.fViewMat[1][1];
	fInv[1][1] = camera.fViewMat[0][0]*camera.fViewMat[2][2]-camera.fViewMat[0][2]*camera.fViewMat[2][0];
	fInv[1][2] = camera.fViewMat[0][2]*camera.fViewMat[1][0]-camera.fViewMat[0][0]*camera.fViewMat[1][2];
	fInv[2][1] = camera.fViewMat[0][1]*camera.fViewMat[2][0]-camera.fViewMat[0][0]*camera.fViewMat[2][1];
	fInv[2][2] = camera.fViewMat[0][0]*camera.fViewMat[1][1]-camera.fViewMat[0][1]*camera.fViewMat[1][0];
	camera.fViewMat[0][0] = fInv[0][0]*fInvDet;
	camera.fViewMat[0][1] = fInv[0][1]*fInvDet;
	camera.fViewMat[0][2] = fInv[0][2]*fInvDet;
	camera.fViewMat[1][0] = fInv[1][0]*fInvDet;
	camera.fViewMat[1][1] = fInv[1][1]*fInvDet;
	camera.fViewMat[1][2] = fInv[1][2]*fInvDet;
	camera.fViewMat[2][0] = fInv[2][0]*fInvDet;
	camera.fViewMat[2][1] = fInv[2][1]*fInvDet;
	camera.fViewMat[2][2] = fInv[2][2]*fInvDet;

	float fView[3];
	fView[0] = fInv[0][0]*fDiff[0]+fInv[1][0]*fDiff[1]+fInv[2][0]*fDiff[2];
	fView[1] = fInv[0][1]*fDiff[0]+fInv[1][1]*fDiff[1]+fInv[2][1]*fDiff[2];
	fView[2] = fInv[0][2]*fDiff[0]+fInv[1][2]*fDiff[1]+fInv[2][2]*fDiff[2];

	//CVec3 vCam( -vView.fY,-vView.fZ,vView.fX );
	float fCam[3];
	fCam[0] = -fView[1];
	fCam[1] = -fView[2];
	fCam[2] =  fView[0];

	float    fScreenX = (rc.right-rc.left)/2.0f;
	float    fScreenY = (rc.bottom-rc.top)/2.0f;
	// Thanks pat0! Aspect ratio fix
	float    fTmpX    = fScreenX / tan(((camera.fFov*44.0f)/2.0f)*(PI/180));
	float    fTmpY    = fScreenY / tan(((camera.fFov*35.0f)/2.0f)*(PI/180));

	POINT pctMouse;
	//pctMouse.x = fScreenX + vCam.fX*fTmpX/vCam.fZ;
	pctMouse.x = fScreenX + fCam[0]*fTmpX/fCam[2];
	//pctMouse.y = fScreenY + vCam.fY*fTmpY/vCam.fZ;
	pctMouse.y = fScreenY + fCam[1]*fTmpY/fCam[2];

	if( pctMouse.x < 0 || pctMouse.y < 0 || pctMouse.x > rc.right || pctMouse.y > rc.bottom )
		return FALSE;

	pt->x = pctMouse.x;
	pt->y = pctMouse.y;

	return TRUE;
}*/

/*
VOID SpectateObject(Object Obj)
{
	CGCamera ActiveCamera;
	WOWPOS Dest;
	FLOAT fBuffer = 0;
	FLOAT fDistanceFromObject = 0;

	ActiveCamera = GetActiveCamera();
	WriteProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_Follow_GUID), &Obj.GUID, sizeof(Obj.GUID), 0);
	while (1)
	{
		Obj = DGetObjectEx(Obj.BaseAddress, BaseObjectInfo + LocationInfo);
		fBuffer = Obj.Rotation;
		fBuffer -= PI;
		while (fBuffer > PI) fBuffer -= PI;
		while (fBuffer > PI / 2) fBuffer -= PI / 2;
		while (fBuffer < 0) fBuffer += PI ;
		if (fBuffer < PI /2 && fBuffer > 0) fBuffer -= PI/2;

		fDistanceFromObject = GetDistance(Obj.X, 0, Obj.Y, 0) - 2;
		Dest.X = cos(fBuffer)*(fDistanceFromObject);
		Dest.Y = sin(fBuffer)*(fDistanceFromObject);
		Dest.Z = Obj.Z + 2.0f;

		WriteProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_X), &Obj.X, sizeof(Obj.X), 0);
		WriteProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_Y), &Obj.Y, sizeof(Obj.Y), 0);
		WriteProcessMemory(WoW::handle, (LPVOID)(ActiveCamera.ActiveCamera + Camera_Z), &Dest.Z, sizeof(Obj.Z), 0);
	}
}

CGCamera UpdateCamera(CGCamera Camera)
{
	ReadProcessMemory(WoW::handle, (LPVOID)(Camera.ActiveCamera + Camera_X), &Camera.Pos.X, sizeof(Camera.Pos.X), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(Camera.ActiveCamera + Camera_Y), &Camera.Pos.Y, sizeof(Camera.Pos.Y), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(Camera.ActiveCamera + Camera_Z), &Camera.Pos.Z, sizeof(Camera.Pos.Z), 0);
	ReadProcessMemory(WoW::handle, (LPVOID)(Camera.ActiveCamera + Camera_Follow_GUID), &Camera.FollowGUID, sizeof(Camera.FollowGUID), 0);
	return Camera;
}*/