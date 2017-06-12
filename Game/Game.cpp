#include "Game.h"
#include "Main\Main.h"
#include "Memory\Memory.h"

#include <Game\Variable.h>

HANDLE	WoW::handle = NULL;
HWND	WoW::window = NULL;
DWORD	WoW::state =  NULL;

namespace WoW
{
	bool ValidWindow()
	{
		return WoW::window != NULL;
	}

	bool ValidHandle()
	{
		return WoW::handle != NULL;
	}

	bool ValidWoW()
	{
		return ValidWindow() && ValidHandle();
	}

	int GetMapID()
	{
		DWORD buffer = NULL;

		ReadProcessMemory(WoW::handle, (LPVOID)(g_dwCurrentManager + 0xCC), &buffer, sizeof(buffer), NULL);
		return buffer;
	}

	int GetZoneID()
	{
		DWORD buffer = NULL;

		ReadProcessMemory(WoW::handle, (LPVOID)(Offsets::ZoneID), &buffer, sizeof(buffer), NULL);
		return buffer;
	}

	DWORD GetPlayerBase()
	{
		DWORD buff = NULL;

		if (!rpm(PLAYER_BASE, &buff))
			return NULL;

		if (!rpm(buff + 0x34, &buff))
			return NULL;

		if (!rpm(buff + 0x24, &buff))
			return NULL;

		return buff;
	}


	bool InGame()
	{
		int buffer = 0;

		if (!WoW::isRunning())
			return false;

		if (!ValidWindow())
			GetWoWWindow();

		if (!WoW::ValidWoW())
			return false;

		if (!rpm(Offsets::InGame, &buffer))
			return false;

		return buffer && WoW::GetGameState() == eGameState::NotLoading;
	}

	int GetCharacterSelectMaxIndex()
	{
		DWORD buff = NULL;

		rpm(Offsets::CharSelectMaxIndex, &buff);
		return buff - 1;
	}

	int GetCharacterSelectIndex()
	{
		DWORD buff = NULL;

		rpm(Offsets::CharSelectIndex, &buff, sizeof(buff));
		return buff;
	}

	bool IsLoading()
	{
		bool buffer = true;

		if (!rpm(Offsets::IsLoadingOrConnecting, &buffer, sizeof(buffer)))
			return true;

		return buffer;
	}

	bool IsCharacterSelectScreen()
	{
		return !WoW::IsLoginScreen() && !WoW::InGame() && !WoW::IsLoading();
	}

	bool IsLoginScreen()
	{
		DWORD buff = NULL;

		rpm(Offsets::LoginScreenStatus, &buff);
		return buff;
	}

	int GetQueuePosition()
	{
		DWORD buff = NULL;

		if (WoW::GetGameState() != WoW::InQueue) return -2;
		if (!rpm(Offsets::QueuePosition, &buff)) return -1;

		return buff;
	}

	std::string GetRealmName()
	{
		CHAR buffer[256];

		if (!rpm(Offsets::RealmName, &buffer, sizeof(buffer)))
			return "";

		return buffer;
	}

	std::string GetAccountName()
	{
		CHAR  buffer[256];

		if (!rpm(Offsets::AccountName, &buffer, sizeof(buffer)))
			return "";

		return buffer;
	}

	std::string GetLocalPlayerName()
	{
		CHAR name[64];

		if (!rpm(Offsets::PlayerName, &name, sizeof(name)))
			return "<Unknown>";

		return name;
	}

	int GetComboPoints(const WGUID &guid)
	{
		WGUID &compare = GetCurrentTargetGUID();

		if (compare == guid)
		{
			WoW::Variable<byte> pts(Offsets::ComboPoints);
			return pts;
		}

		return 0;
	}

	bool isRunning()
	{
		DWORD code;

		if (handle == NULL)						return false;
		if (!GetExitCodeProcess(handle, &code)) return false;

		return code == STILL_ACTIVE;
	}

	WoW::eGameState GetGameState()
	{
		eGameState buff = Loading;

		if (!rpm(Offsets::IsLoadingOrConnecting, &buff))
			return buff;

		return buff;
	}

	HANDLE GetHandle(DWORD pId)
	{
		return (WoW::handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId));
	}

	HWND GetWindow()
	{
		if (handle == NULL) return NULL;

		GetWoWWindow();
		return g_hwWoW;
	}

	DWORD GetZoneTextPtr()
	{
		DWORD pointer = NULL;

		if (!rpm(Offsets::GetZoneText, &pointer, sizeof(pointer)))
			return NULL;

		return pointer;
	}

	DWORD GetSubZoneTextPtr()
	{
		DWORD pointer = NULL;

		if (!rpm(Offsets::GetSubZoneText, &pointer, sizeof(pointer)))
			return NULL;

		return pointer;
	}


	std::string GetZoneText()
	{
		char  buffer[64];
		DWORD pointer = NULL;

		if (!rpm(Offsets::GetZoneText, &pointer, sizeof(pointer)))
			return "";

		if (!rpm(pointer, &buffer, sizeof(buffer))) 
			return "";

		return buffer;
	}

	std::string GetSubZoneText()
	{
		char  buffer[64];
		DWORD pointer = NULL;
		
		if (!rpm(Offsets::GetSubZoneText, &pointer, sizeof(pointer)))
			return "";

		if (!rpm(pointer, &buffer, sizeof(buffer))) 
			return "";

		return buffer;
	}
};

