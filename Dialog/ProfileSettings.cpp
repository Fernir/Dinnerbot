#include "ProfileSettings.h"
#include "Resource.h"

#include "Common\Common.h"
#include <File\FishingProfile.h>

#include <Commdlg.h>

namespace Settings
{
	FishingProfile currentProfile;

	std::string GetSaveNameDialog(HWND hWnd)
	{
		OPENFILENAME ofn;       // common dialog box structure
		CHAR szFile[MAX_PATH];       // buffer for file name

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "XML\0*.xml\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = "New Dinnerbot Profile";
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

		GetSaveFileName(&ofn);
		return ofn.lpstrFile;
	}

	std::string GetFileNameDialog(HWND hWnd)
	{
		OPENFILENAME ofn;       // common dialog box structure
		CHAR szFile[MAX_PATH];       // buffer for file name

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "XML\0*.xml\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = "New Dinnerbot Profile";
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		GetOpenFileName(&ofn);
		return ofn.lpstrFile;
	}

	void clearListboxes(HWND hWnd)
	{
		SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_OTHER, LB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_RESETCONTENT, 0, 0);
	}

	void importProfile(HWND hWnd, FishingProfile &profile)
	{
		char buffer[1056];

		sprintf_s(buffer, "Profile: %s", profile.name.c_str());
		SetWindowText(hWnd, buffer);

		sprintf_s(buffer, "Map: %d", profile.mapID);
		SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_OTHER, LB_ADDSTRING, 0, (LPARAM)buffer);

		sprintf_s(buffer, "Zone: %s - %d", profile.zoneName.c_str(), profile.zoneID);
		SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_OTHER, LB_ADDSTRING, 0, (LPARAM)buffer);

		// Pool names
		for (auto &each : profile.poolNames)
		{
			sprintf_s(buffer, "Pool: %s", each.c_str());
			SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_ADDSTRING, 0, (LPARAM)buffer);
		}

		// Fishing spots
		for (auto &each : profile.spots)
		{
			sprintf_s(buffer, "Spot: %f %f %f", each.X, each.Y, each.Z);
			SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_ADDSTRING, 0, (LPARAM)buffer);
		}
	}

	void onProfileClear(HWND hWnd)
	{
		currentProfile.clear();
		clearListboxes(hWnd);
	}

	void onProfileLoad(HWND hWnd)
	{
		std::string fileName = GetFileNameDialog(hWnd);

		currentProfile.loadProfile(fileName);
		if (currentProfile.isLoaded())
		{
			clearListboxes(hWnd);
			importProfile(hWnd, currentProfile);
		}
	}

	void onProfileSave(HWND hWnd)
	{
		std::string fileName = GetSaveNameDialog(hWnd);

		if (currentProfile.isLoaded())
		{
			currentProfile.saveProfile(fileName);
		}
	}

	void onProfileInsert(HWND hWnd)
	{

	}

	LRESULT CALLBACK ProfileSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int buffer = 0;

		switch (message)
		{
		case WM_INITDIALOG:
			if (currentProfile.isLoaded())
				importProfile(hWnd, currentProfile);

			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_PROFILE_BUTTON_NEW:
				//if (!GetSaveNameDialog(hWnd, szBuffer)) break;
				break;

			case IDC_PROFILE_BUTTON_SAVE:
				onProfileSave(hWnd);
				//if (!GetSaveNameDialog(hWnd, szBuffer)) break;
				break;

			case IDC_PROFILE_BUTTON_EDIT:
				//buffer = SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_GETCURSEL, 0, 0);
				//SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_GETTEXT, (WPARAM)buffer, (LPARAM)&buffer);
				//Hack::Movement::Teleport(X, Y, Z);
				break;

			case IDC_PROFILE_BUTTON_LOAD:
				onProfileLoad(hWnd);
				break;

			case IDC_PROFILE_BUTTON_DELETE:
				//buffer = SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_GETCURSEL, 0, 0);
				//SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_GETTEXT, (WPARAM)nLBSel, (LPARAM)&szBuffer);

				//DeleteWaypoint(currentProfile, Waypoint.X, Waypoint.Y, Waypoint.Z);
				//SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_DELETESTRING, (WPARAM)buffer, 0);
				break;

			case IDC_PROFILE_BUTTON_CLEAR:
				onProfileClear(hWnd);
				break;

			case IDC_PROFILE_BUTTON_ADD:
				onProfileInsert(hWnd);
				//SendDlgItemMessage(hWnd, IDC_PROFILE_LIST_WAYPOINTS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
				break;

			case WM_DESTROY:
				EndDialog(hWnd, TRUE);
				return TRUE;
			}
			break;

		default:
			return FALSE;
		}

		return TRUE;
	}

}