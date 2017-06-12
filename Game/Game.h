#pragma once

#include <Windows.h>
#include <string>
#include <Main\Structures.h>

/*
#define GAME_NOPROCESS	0x0
#define GAME_INGAME		0x1
#define GAME_LOADING	0x2
#define GAME_LOGIN		0x3
#define GAME_INQUEUE	0x4
#define GAME_CHRSEL		0x5*/

namespace WoW
{
	extern HANDLE handle;
	extern HWND   window;
	extern DWORD  state;

	enum eGameState
	{
		Crashed   = -1,
		NotLoading = 0,
		Loading	   = 1,
		InQueue    = 2,
		Unknown    = 3,
		CharacterSelect,
		LoginScreen,
	};

	bool InGame();
	bool IsLoading();
	bool isRunning();
	bool IsLoginScreen();
	bool IsCharacterSelectScreen();

	eGameState GetGameState();

	DWORD GetPlayerBase();

	bool ValidWindow();
	bool ValidHandle();
	bool ValidWoW();

	int GetComboPoints(const WGUID &guid);
	int GetCharacterSelectMaxIndex();
	int GetCharacterSelectIndex();
	int GetQueuePosition();
	int GetZoneID();
	int GetMapID();

	HWND   GetWindow();
	HANDLE GetHandle();

	std::string GetRealmName();
	std::string GetAccountName();
	std::string GetLocalPlayerName();

	DWORD GetZoneTextPtr();
	DWORD GetSubZoneTextPtr();

	std::string GetZoneText();
	std::string GetSubZoneText();
};
