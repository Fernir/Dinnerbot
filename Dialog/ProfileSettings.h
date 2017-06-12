#pragma once

#include <Windows.h>

#include <File\FishingProfile.h>

namespace Settings
{
	LRESULT CALLBACK ProfileSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	extern FishingProfile currentProfile;
}

