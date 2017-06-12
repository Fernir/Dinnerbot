#include "Settings.h"
#include "Drawing.h"
#include "Dialog\Resource.h"
#include "Dialog\Dialog.h"
#include "Utilities\Thread.h"

void onCheckGameObjects(HWND hWnd)
{
	DWORD state = GetDlgCheck(hWnd, IDC_RADAR_CHECK_GAME_OBJECTS);

	Radar::displaySettings.gameObject = state;
}

void onCheckUnits(HWND hWnd)
{
	DWORD state = GetDlgCheck(hWnd, IDC_RADAR_CHECK_UNITS);

	Radar::displaySettings.unit = state;
}

void onCheckFriendlyPlayers(HWND hWnd)
{
	DWORD state = GetDlgCheck(hWnd, IDC_RADAR_CHECK_PLAYERS_FRIENDLY);

	Radar::displaySettings.friendly = state;
}

void onCheckEnemyPlayers(HWND hWnd)
{
	DWORD state = GetDlgCheck(hWnd, IDC_RADAR_CHECK_PLAYERS_ENEMY);

	Radar::displaySettings.enemy = state;
}

void onCheckShowNames(HWND hWnd)
{
	DWORD state = GetDlgCheck(hWnd, IDC_RADAR_CHECK_SHOW_NAMES);

	Radar::displaySettings.name = state;
}

void onInitDialog(HWND hWnd)
{
	SetDlgCheck(hWnd, IDC_RADAR_CHECK_PLAYERS_FRIENDLY, Radar::displaySettings.friendly);
	SetDlgCheck(hWnd, IDC_RADAR_CHECK_PLAYERS_ENEMY, Radar::displaySettings.enemy);
	SetDlgCheck(hWnd, IDC_RADAR_CHECK_GAME_OBJECTS, Radar::displaySettings.gameObject);
	SetDlgCheck(hWnd, IDC_RADAR_CHECK_UNITS, Radar::displaySettings.unit);
}

LRESULT CALLBACK Radar::SettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		onInitDialog(hWnd);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			// Game Objects checkbox.
			case IDC_RADAR_CHECK_GAME_OBJECTS:
				onCheckGameObjects(hWnd);
				break;

			// Enemy players checkbox.
			case IDC_RADAR_CHECK_PLAYERS_ENEMY:
				onCheckEnemyPlayers(hWnd);
				break;

			// Friendly players checkbox.
			case IDC_RADAR_CHECK_PLAYERS_FRIENDLY:
				onCheckFriendlyPlayers(hWnd);
				break;

			// Units checkbox.
			case IDC_RADAR_CHECK_UNITS:
				onCheckUnits(hWnd);
				break;

			case IDC_RADAR_CHECK_SHOW_NAMES:
				onCheckShowNames(hWnd);
				break;
		}

		break;

	case WM_CLOSE:
		Radar::settingsWnd = NULL;
		DestroyWindow(hWnd);  
		break;

	default:
		return FALSE;
	}

	return FALSE;
}

VOID Radar::ToggleSettings()
{
	if (IsWindowVisible(Radar::settingsWnd))
		ShowWindow(Radar::settingsWnd, SW_HIDE);
	else
		ShowWindow(Radar::settingsWnd, SW_SHOW);
}

BOOL Radar::CreateSettings(HWND parent)
{
	MSG		msg;
	INT		ret;
	RECT parentR;
	RECT r;

	int width, height;

	Radar::settingsWnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RADAR_SETTINGS), parent, (DLGPROC)SettingsProc);
	if (!Radar::settingsWnd)
		return false;

	// Get dimensions of parent and child window.
	GetWindowRect(parent, &parentR);
	GetWindowRect(Radar::settingsWnd, &r);

	// Calculate width and height of the child window.
	width = r.right - r.left;
	height = r.bottom - r.top;

	MoveWindow(Radar::settingsWnd, parentR.right, parentR.bottom, width, height, false);
	ShowWindow(Radar::settingsWnd, SW_SHOW); 
	return true;
}
