#include "ObjectWatch.h"
#include "Resource.h"
#include "..\Utilities\Utilities.h"

CHAR g_szSearch[MAX_PATH];
BOOL g_Searching = FALSE;
BOOL g_Transport = TRUE;
BOOL g_OWReport = TRUE;

HWND g_hwObjectWatch = NULL;

/*
Object *GetTransportObjects(UINT *size)
{ 
	Object CurrentObject, NextObject;
	Object *list = (Object *)malloc(sizeof(Object) * 12);

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + OBJECT_MANAGER_FIRST), &CurrentObject.BaseAddress, sizeof(CurrentObject.BaseAddress), NULL)))
	{
		memset(&CurrentObject, 0, sizeof(CurrentObject));
		return CurrentObject;
	}

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo | GameObjectInfo);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		if (CurrentObject.GameObjectField.Flags)
			return CurrentObject;

		NextObject = DGetObjectEx(CurrentObject.Next_Ptr, BaseObjectInfo | GameObjectInfo);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	memset(&CurrentObject, 0, sizeof(CurrentObject));
	return CurrentObject;
}*/

LRESULT CALLBACK ObjectWatchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHAR szBuffer[MAX_PATH];

	switch (message)
	{
	case WM_PAINT:
		break;

	case WM_INITDIALOG: 
		g_hwObjectWatch = hWnd;
		if (g_OWReport)
			SendDlgItemMessage(hWnd, IDC_OBJECTWATCH_CHECK_REPORT, BM_SETCHECK, 1, 0);

		if (g_Searching)
		{
			SetDlgItemText(hWnd, IDC_OBJECTWATCH_EDIT_NAME, g_szSearch);
			SetDlgItemText(hWnd, IDC_OBJECTWATCH_BUTTON_SEARCH, "Stop Search");
		}

		break;
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OBJECTWATCH_BUTTON_SEARCH:
			GetDlgItemTextA(hWnd, IDC_OBJECTWATCH_EDIT_NAME, szBuffer, MAX_PATH);

			if (g_Searching || !_stricmp(szBuffer, ""))
			{
				SetDlgItemText(hWnd, IDC_OBJECTWATCH_BUTTON_SEARCH, "Search");
				if (g_Searching) LogAppend("Stopped the search.");
				g_Searching = FALSE;
				break;
			}

			g_Searching = TRUE;
			strncpy_s(g_szSearch, szBuffer, strlen(szBuffer) + 1);
			SetDlgItemText(hWnd, IDC_OBJECTWATCH_BUTTON_SEARCH, "Stop Search");
			sprintf_s(szBuffer, "Searching for %s.", g_szSearch);
			LogAppend(szBuffer);
			break;
		}

	case IDC_OBJECTWATCH_CHECK_REPORT:		
		if(HIWORD(wParam) == BN_CLICKED)
		{
			if(SendDlgItemMessage(hWnd, IDC_OBJECTWATCH_CHECK_REPORT, BM_GETCHECK, 0, 0) == BST_CHECKED) 
				g_OWReport = TRUE;
			else
				g_OWReport = FALSE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, LOWORD(wParam));
		break;

	default:
		return FALSE;
	}

	return FALSE;
}