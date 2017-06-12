#include "ObjectList.h"
#include "Resource.h"

#include "Geometry\Coordinate.h"
#include "WoW API\Object Classes\Object.h"
#include "Memory\Hacks.h"
#include "Main\Debug.h"


#define IDT_TIMER_REFRESH 10001

Radar_Players PlayerList;
Radar_Players LastPlayerList;

HWND RadarDialog;
HWND TreeView;

TV_ITEM tvi;
TV_INSERTSTRUCT tvinsert; 

HTREEITEM Selected; 

HTREEITEM Objects;        
HTREEITEM Players;        
HTREEITEM Units;           
HTREEITEM GameObjects;    

TVI_Object wGameObjectList[500];
TVI_Object wPlayerList[500];
TVI_Object wUnitList[500];

HTREEITEM AddItemToTree(HTREEITEM, HTREEITEM, LPTSTR);
HTREEITEM GetCorrespondingTreeItem(TVI_Object ObjectList[500], INT nIndex, WGUID Object);

BOOL m_nTimer = FALSE;

BOOL PListRefresh = FALSE;
BOOL UListRefresh = FALSE;
BOOL GListRefresh = FALSE;
BOOL OListRefresh = FALSE;

INT nPListIndex, nUListIndex, nGListIndex;

INT GetObjectTypeCount(DWORD ObjectType)
{
	register Object CurrentObject, NextObject;

	INT nCount = 0;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject, sizeof(DWORD), NULL)))
		return nCount;

	CurrentObject = DGetObjectEx(CurrentObject.BaseAddress, BaseObjectInfo);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		if (CurrentObject.Type_ID == ObjectType || ObjectType == NULL) nCount++;
		NextObject = DGetObject(CurrentObject.Next_Ptr);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	return nCount;
}

HTREEITEM AddItemToTree(HTREEITEM Parent, HTREEITEM InsertAfter, LPTSTR lpszItem)
{ 
	if (InsertAfter == NULL) InsertAfter = TVI_LAST;

	tvinsert.hParent = Parent;        
	tvinsert.hInsertAfter = InsertAfter; 
	tvinsert.item.pszText = lpszItem;
	return (HTREEITEM)SendDlgItemMessage(RadarDialog, IDC_MAIN_RADAR_TREE_OBJECTS, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
} 

INT PopulateObjectList(TVI_Object ObjectsList[500], HTREEITEM Parent, DWORD dwObjectType)
{
	register Object CurrentObject, NextObject;

	INT nIndex = 0;

	if (!(ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + Offsets::FirstEntry), &CurrentObject, sizeof(DWORD), NULL)))
		return 0;

	CurrentObject = DGetObject(CurrentObject.BaseAddress);
	while (CurrentObject.BaseAddress != 0 && (CurrentObject.BaseAddress & 1) == 0)
	{
		if (nIndex > 499) return 499;
		if (CurrentObject.Type_ID == dwObjectType)
		{
			ObjectsList[nIndex].Parent = Parent;
			ObjectsList[nIndex].TreeObject.low = CurrentObject.GUID.low;
			ObjectsList[nIndex].TreeObject.high = CurrentObject.GUID.high;
			ObjectsList[nIndex++].nChildIndex = nIndex;
		}

		NextObject = DGetObject(CurrentObject.Next_Ptr);
		if (NextObject.BaseAddress == CurrentObject.BaseAddress)
			break;
		else
			CurrentObject = NextObject;
	}
	
	return nIndex;
}

VOID InitTree(HWND hWnd)
{  
	CHAR szBuffer[64];
			
	sprintf(szBuffer, "Objects (%d)", GetObjectTypeCount(NULL));
	tvinsert.hParent = NULL;			
	tvinsert.hInsertAfter = TVI_ROOT; 
    tvinsert.item.mask = TVIF_TEXT;
	tvinsert.item.pszText = szBuffer;
	Objects = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_MAIN_RADAR_TREE_OBJECTS,TVM_INSERTITEM, 0, (LPARAM)&tvinsert);

	tvinsert.hParent = Objects;         // handle of the above data
	tvinsert.hInsertAfter = TVI_LAST;  // below parent
	tvinsert.item.pszText = "Players";
	Players = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_MAIN_RADAR_TREE_OBJECTS,TVM_INSERTITEM, 0, (LPARAM)&tvinsert);

	tvinsert.hParent = Objects;         // handle of the above data
	tvinsert.hInsertAfter = TVI_LAST;  // below parent
	tvinsert.item.pszText = "Units";
	Units = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_MAIN_RADAR_TREE_OBJECTS, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);	   

	tvinsert.hParent = Objects;         // handle of the above data
	tvinsert.hInsertAfter = TVI_LAST;  // below parent
	tvinsert.item.pszText = "Game Objects";
	GameObjects = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_MAIN_RADAR_TREE_OBJECTS, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);	   
}

BOOL IsExpanded(HTREEITEM Item)
{
	memset(&tvi, 0, sizeof(tvi));
	
	tvi.hItem = Item;
	tvi.mask = TVIF_STATE | TVIF_HANDLE;
	SendDlgItemMessage(RadarDialog, IDC_MAIN_RADAR_TREE_OBJECTS, TVM_GETITEM, 0, (WPARAM)&tvi);

	return (tvi.state & TVIS_EXPANDED) == TVIS_EXPANDED;
}

VOID ChangeNodeText(HWND hWnd, HTREEITEM Item, CHAR *Text)
{
	memset(&tvi, 0, sizeof(tvi));

	tvi.mask = TVIF_TEXT;
	tvi.hItem = Item;
	tvi.pszText = Text;
	tvi.cchTextMax = strlen(Text);
	SendDlgItemMessage(hWnd, IDC_MAIN_RADAR_TREE_OBJECTS, TVM_SETITEM, 0, (WPARAM)&tvi);
}

VOID RefreshNode(HWND hWnd, INT nNode)
{
	CHAR szBuffer[256];
	
	switch (nNode)
	{
	case OBJECTS:	// Objects Node (Root)
		sprintf(szBuffer, "Objects (%d)", GetObjectTypeCount(NULL));
		ChangeNodeText(hWnd, Objects, szBuffer);
		break;

	case PLAYERS: // Players Node
		sprintf(szBuffer, "Players (%d)", GetObjectTypeCount(OT_PLAYER));
		ChangeNodeText(hWnd, Players, szBuffer);
		break;

	case UNITS: // Units Node
		sprintf(szBuffer, "Units (%d)", GetObjectTypeCount(OT_UNIT));
		ChangeNodeText(hWnd, Units, szBuffer);
		break;

	case GOBJS: // Game Objects Node
		sprintf(szBuffer, "Game Objects (%d)", GetObjectTypeCount(OT_GAMEOBJ));
		ChangeNodeText(hWnd, GameObjects, szBuffer);
		break;
	}
}

BOOL InList(TVI_Object ObjectList[500], INT UsedIndex, WGUID Value)
{
	for (int x = 0; x < UsedIndex && !CompareWGUIDS(ObjectList[x].TreeObject, Value); x++)

	if (CompareWGUIDS(ObjectList[x].TreeObject, Value)) return TRUE;
	return FALSE;
}

VOID DeleteUnderParent(TVI_Object ItemList[500], INT nIndex, HTREEITEM Parent)
{
	for (int x = 0; x < nIndex; x++)
	{
		if (ItemList[x].TreeItem != NULL)
		{
			TreeView_DeleteItem(TreeView, ItemList[x].TreeItem);
		}
	}
}

VOID FillUnderParentWithList(TVI_Object ItemList[500], INT nIndex, HTREEITEM Parent)
{
	CHAR szBuffer[256];
	
	for (int x = 0; x < nIndex; x++)
	{
		DGetObjectName(GetObjectByGUID(ItemList[x].TreeObject), szBuffer);
		ItemList[x].TreeItem = AddItemToTree(Parent, NULL, szBuffer);
	}
}

VOID RefreshNodeChildren(INT nNode)
{
	switch (nNode)
	{
	case PLAYERS: // Players Node
		DeleteUnderParent(wPlayerList, nPListIndex, Players);
		nPListIndex = PopulateObjectList(wPlayerList, Players, OT_PLAYER);
		FillUnderParentWithList(wPlayerList, nPListIndex, Players);
		break;

	case UNITS: // Units Node
		DeleteUnderParent(wUnitList, nUListIndex, Units);
		nUListIndex = PopulateObjectList(wUnitList, Units, OT_UNIT);
		FillUnderParentWithList(wUnitList, nUListIndex, Units);
		break;

	case GOBJS: // Game Objects Node
		DeleteUnderParent(wGameObjectList, nGListIndex, GameObjects);
		nGListIndex = PopulateObjectList(wGameObjectList, GameObjects, OT_GAMEOBJ);
		FillUnderParentWithList(wGameObjectList, nGListIndex, GameObjects);
		break;
	}
}

VOID HandleNodeInteract(HTREEITEM Item)
{
	Object WObject, Target;
	CHAR szBuffer[256];

	for (int x = 0; x < 500; x++)
	{
		if (Item == wPlayerList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wPlayerList[x].TreeObject, NameInfo | UnitFieldInfo | LocationInfo | BaseObjectInfo);
			Target = GetObjectByGUIDEx(WObject.UnitField.Target, BaseObjectInfo | NameInfo);
			//TargetObject(WObject.GUID); Crashes too much

			sprintf(szBuffer, "%s: 0x%p%p (%f, %f, %f) %f", WObject.Name, WObject.GUID.high, WObject.GUID.low, WObject.X, WObject.Y, WObject.Z, WObject.Rotation);
			LogAppend(szBuffer);

			sprintf(szBuffer, "Target: %s", Target.Name);
			LogAppend(szBuffer);
		}

		else if (Item == wUnitList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wUnitList[x].TreeObject, NameInfo | UnitFieldInfo | LocationInfo | BaseObjectInfo);
			//TargetObject(WObject.GUID);
			Debug::DumpLocation(WObject);
			Debug::DumpBaseObject(WObject);
			Debug::DumpUnitField(WObject.UnitField);
		}

		else if (Item == wGameObjectList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wGameObjectList[x].TreeObject, NameInfo | GameObjectInfo | LocationInfo | BaseObjectInfo);
			Debug::DumpLocation(WObject);
			Debug::DumpBaseObject(WObject);
			Debug::DumpGameObjectField(WObject.GameObjectField);
		}
	}
}

VOID HandleNodeSelection(HTREEITEM Item)
{
	if (Item == Objects && !OListRefresh)
	{
		OListRefresh = TRUE;
		RefreshNode(RadarDialog, OBJECTS);
		RefreshNode(RadarDialog, PLAYERS);
		RefreshNode(RadarDialog, UNITS);
		RefreshNode(RadarDialog, GOBJS);
		OListRefresh = FALSE;
	}
	else if (Item == Players && !PListRefresh)
	{
		PListRefresh = TRUE;
		OListRefresh = TRUE;
		RefreshNode(RadarDialog, PLAYERS);
		if (!IsExpanded(Players))
		{
			RefreshNodeChildren(PLAYERS);
		}

		PListRefresh = FALSE;
		OListRefresh = FALSE;
	}
	else if (Item == Units && !UListRefresh)
	{
		UListRefresh = TRUE;
		OListRefresh = TRUE;
		RefreshNode(RadarDialog, UNITS);
		if (!IsExpanded(Units))
		{
			RefreshNodeChildren(UNITS);
		}

		UListRefresh = FALSE;
		OListRefresh = FALSE;
	}
	else if (Item == GameObjects && !GListRefresh)
	{
		GListRefresh = TRUE;
		OListRefresh = TRUE;
		RefreshNode(RadarDialog, GOBJS);
		if (!IsExpanded(GameObjects))
		{
			RefreshNodeChildren(GOBJS);
		}

		GListRefresh = FALSE;
		OListRefresh = FALSE;
	}
}

HTREEITEM GetParent(HTREEITEM Item)
{
	for (int x = 0; x < nPListIndex; x++)
		if (wPlayerList[x].TreeItem == Item) return wPlayerList[x].Parent;

	for (int x = 0; x < nUListIndex; x++)
		if (wUnitList[x].TreeItem == Item) return wUnitList[x].Parent;

	for (int x = 0; x < nGListIndex; x++)
		if (wGameObjectList[x].TreeItem == Item) return wGameObjectList[x].Parent;

	return NULL;
}

VOID RefreshSelected(HTREEITEM Item, TVI_Object ObjectList[500], INT nIndex)
{
	RefreshNode(RadarDialog, OBJECTS);
	RefreshNode(RadarDialog, PLAYERS);
	RefreshNode(RadarDialog, UNITS);
	RefreshNode(RadarDialog, GOBJS);

	if (Item == NULL)
	{
		PListRefresh = TRUE;
		UListRefresh = TRUE;
		OListRefresh = TRUE;
		RefreshNodeChildren(PLAYERS);
		RefreshNodeChildren(UNITS);
		RefreshNodeChildren(GOBJS);
		PListRefresh = TRUE;
		UListRefresh = TRUE;
		OListRefresh = TRUE;
	}
	if (Item == Players || GetParent(Item) == Players)
	{
		PListRefresh = TRUE;
		OListRefresh = TRUE;
		RefreshNodeChildren(PLAYERS);
		PListRefresh = TRUE;
		OListRefresh = TRUE;
	}
	else if (Item == Units || GetParent(Item) == Units)
	{
		UListRefresh = TRUE;
		OListRefresh = TRUE;
		RefreshNodeChildren(UNITS);
		UListRefresh = TRUE;
		OListRefresh = TRUE;
	}
	else if ((Item == GameObjects || GetParent(Item) == GameObjects) && !GListRefresh)
	{
		GListRefresh = TRUE;
		OListRefresh = TRUE;
		RefreshNodeChildren(GOBJS);
		GListRefresh = FALSE;
		OListRefresh = FALSE;
	}

	//Item = GetCorrespondingTreeItem(ObjectList, nIndex, Buff);
//	TreeView_EnsureVisible(TreeView, Item);
	//TreeView_SelectItem(TreeView, Item);
}

BOOL IsObjectTypeNode(HTREEITEM Item)
{
	if (Item == Objects)
	{
		return TRUE;
	}
	else if (Selected == Players)
	{
		return TRUE;
	}
	else if (Selected == Units)
	{
		return TRUE;
	}
	else if (Selected == GameObjects)
	{
		return TRUE;
	}

	return FALSE;
}

HTREEITEM GetCorrespondingTreeItem(TVI_Object ObjectList[500], INT nIndex, WGUID Object)
{
	for (int x = 0; x < nIndex; x++)
	{
		if (CompareWGUIDS(ObjectList[x].TreeObject, Object)) return ObjectList[x].TreeItem;
	}

	return NULL;
}

WGUID GetCorrespondingGUID(TVI_Object ObjectList[500], INT nIndex, HTREEITEM Item)
{
	WGUID Buff;

	memset(&Buff, 0, sizeof(Buff));
	for (int x = 0; x < nIndex; x++)
	{
		if (ObjectList[x].TreeItem = Item) return ObjectList[x].TreeObject;
	}

	return Buff;
}

VOID TargetObjectFromTree(HTREEITEM Item)
{
	HTREEITEM Parent;

	if (!IsObjectTypeNode(Item))
	{
		Parent = GetParent(Item);
	}
}

VOID HandleRefreshCheck(DWORD dwCheck, WPARAM wParam)
{
    if(HIWORD(wParam) == BN_CLICKED)
	{
		if(dwCheck == BST_CHECKED) 
		{
			m_nTimer = TRUE;
		}
		else
		{
			m_nTimer = FALSE;
		}
	}
}

VOID CALLBACK RefreshTimerProc(HWND hWnd, UINT uMessage, UINT_PTR uEventId, DWORD dwTime)
{
	if (m_nTimer)
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RefreshSelected, Selected, NULL, NULL);
}

VOID TeleportToItem(HTREEITEM Item)
{
	Object WObject;

	for (int x = 0; x < 500; x++)
	{
		if (Item == wPlayerList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wPlayerList[x].TreeObject, LocationInfo | BaseObjectInfo);
		}

		else if (Item == wUnitList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wUnitList[x].TreeObject, LocationInfo | BaseObjectInfo);
		}

		else if (Item == wGameObjectList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wGameObjectList[x].TreeObject, LocationInfo | BaseObjectInfo);
		}
	}

	if (ValidCoord(WObject.Pos))
		Hack::Movement::Teleport(WObject.Pos);
}

VOID WalkToItem(HTREEITEM Item)
{
	Object WObject;

	for (int x = 0; x < 500; x++)
	{
		if (Item == wPlayerList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wPlayerList[x].TreeObject, LocationInfo | BaseObjectInfo);
		}

		else if (Item == wUnitList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wUnitList[x].TreeObject, LocationInfo | BaseObjectInfo);
		}

		else if (Item == wGameObjectList[x].TreeItem)
		{
			WObject = GetObjectByGUIDEx(wGameObjectList[x].TreeObject, LocationInfo | BaseObjectInfo);
		}
	}

	if (ValidCoord(WObject.Pos))
		ClickToMove(WObject.Pos);
}

LRESULT CALLBACK ObjectListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	NMHDR *nmptr;

	switch (message)
	{
	case WM_PAINT:
		break;

	case WM_INITDIALOG: 
		m_nTimer = false;
		InitTree(hWnd);
		RadarDialog = hWnd;

		nPListIndex = 0;
		nUListIndex = 0;
		nGListIndex = 0;
		memset(&wPlayerList, 0, sizeof(wPlayerList));
		memset(&wUnitList, 0, sizeof(wUnitList));
		memset(&wGameObjectList, 0, sizeof(wGameObjectList));
		TreeView = GetDlgItem(RadarDialog, IDC_MAIN_RADAR_TREE_OBJECTS);
		SetTimer(hWnd, IDT_TIMER_REFRESH, 1000, RefreshTimerProc);
		break;
	
	case WM_NOTIFY:
		{               
			nmptr = (LPNMHDR)lParam;
			switch (nmptr->code)
			{
			case TVN_SELCHANGED:
				Selected = ((LPNM_TREEVIEW)nmptr)->itemNew.hItem;
				if (Selected != NULL)
					CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)HandleNodeSelection, Selected, NULL, NULL);

				break;

			case NM_DBLCLK:
				Selected = ((LPNM_TREEVIEW)nmptr)->itemNew.hItem;
				if (Selected != NULL)
				{
					Selected = TreeView_GetSelection(TreeView);
					CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)HandleNodeSelection, Selected, NULL, NULL);
					HandleNodeInteract(Selected);
				}

				break;

			}

			break;
		}
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_MAIN_RADAR_BUTTON_REFRESH:
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RefreshSelected, Selected, NULL, NULL);
			break;

		case IDC_MAIN_RADAR_CHECK_REFRESH_TICK:
			HandleRefreshCheck(SendDlgItemMessage(hWnd, IDC_MAIN_RADAR_CHECK_REFRESH_TICK, BM_GETCHECK, 0, 0), wParam);
			break;

		case IDC_MAIN_RADAR_BUTTON_TELEPORT:
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)TeleportToItem, Selected, NULL, NULL);
			break;

		case IDC_MAIN_RADAR_BUTTON_WALKTO:
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)WalkToItem, Selected, NULL, NULL);
			break;
		}

		break;

	case WM_CLOSE:
		KillTimer(hWnd, IDT_TIMER_REFRESH);
		EndDialog(hWnd, LOWORD(wParam));
		break;

	default:
		return FALSE;
	}

	return FALSE;
}