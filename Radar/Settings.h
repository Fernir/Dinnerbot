#pragma once

#include <Windows.h>

namespace Radar
{
	VOID ToggleSettings();

	BOOL CreateSettings(HWND parent);

	LRESULT CALLBACK SettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
