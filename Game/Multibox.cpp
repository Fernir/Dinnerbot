#include "Multibox.h"

#include <Windows.h>
#include <string>
#include <vector>
#include <TlHelp32.h>

#include "Common\Common.h"

using namespace std;

typedef struct
{
	uint pid;
	HWND window;
	HANDLE handle;
	string playerName;
} WoWInstance;

bool getWoWPIDs(std::vector<string> *playerNames, std::vector<WoWInstance> &WoWs)
{
	PROCESSENTRY32 pe32;
	HANDLE hProcessSnap = NULL;
	HANDLE hProcess = NULL;

	WoWInstance wow = { 0, 0, 0, "" };

	char buffer[256];

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		log("WoWList: Invalid process snap");
		return false;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		log("PopulateWoWList: Process32First failed");
		CloseHandle(hProcessSnap);  // clean the snapshot object
		return false;
	}

	// Now walk the snapshot of processes, and
	// add to list when wow.exe is found
	do
	{
		GetWoWBuildInfo(buffer, pe32.th32ProcessID);
		if (!_stricmp(buffer, SUPPORTED_BUILD))
		{
			GetWoWPlayerName(buffer, pe32.th32ProcessID);

			// If this WoW is in our vector of player names, add the process ot the list.
			if (vectorHas<string>(buffer, *playerNames))
			{
				wow.pid = pe32.th32ProcessID;
				wow.playerName = buffer;
				WoWs.push_back(wow);
			}
		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return true;
}

vector<WoWInstance> WoWs;
uint curHandle;

WNDENUMPROC CALLBACK getWoWWindow_Proc(HWND hWnd, LPARAM lParam)
{
	DWORD pid;

	GetWindowThreadProcessId(hWnd, &pid);
	if (pid == (DWORD)lParam)
	{
		WoWs[curHandle].window = hWnd;
		return (WNDENUMPROC)false;
	}

	return (WNDENUMPROC)true;
}

bool getWoWWindow(uint pid)
{
	return !EnumWindows((WNDENUMPROC)getWoWWindow_Proc, (DWORD)pid);
}

bool PostKey(HWND handle, CHAR cKey, DWORD dwKey)
{
	return PostMessage(handle, dwKey, cKey, 0);
}

void broadcastKey(byte key, DWORD state)
{
	// Broadcast each key to all of our WoW processes.
	for (int x = 0; x < WoWs.size(); x++)
		PostKey(WoWs[x].window, key, state);
}

void checkKeys()
{
	byte keys[256];
	static bool sent[256];

	for (int x = 0; x < 256; x++)
	{
		keys[x] = (char)(GetAsyncKeyState(x) >> 8);

		if (keys[x])
		{
			broadcastKey(MapVirtualKey(x, MAPVK_VK_TO_CHAR), WM_KEYDOWN);
			sent[x] = false;
		}
		else
		{
			if (!sent[x])
			{
				broadcastKey(MapVirtualKey(x, MAPVK_VK_TO_CHAR), WM_KEYUP);
				sent[x] = true;
			}
		}
	}

}

void multibox(std::vector<std::string> *playerNames)
{
	Thread *thread = Thread::GetCurrent();
	if (!thread) return;
	if (!playerNames)
	{
		vlog("Unable to allocate memory.");
		thread->exit();
		return;
	}

	if (playerNames->size() == 0)
	{
		vlog("Enter in player names.");
		thread->exit();
		return;
	}

	WoWs.clear();
	curHandle = 0;

	// Search for WoW processes with active character in the list given above.
	getWoWPIDs(playerNames, WoWs);
	if (WoWs.size() == 0)
	{
		vlog("No WoW process found with those player names.");
		thread->exit();
		return;
	}

	// Retrieve information about each process found.
	for (int x = 0; x < WoWs.size(); x++)
	{
		WoWs[x].handle = OpenProcess(PROCESS_ALL_ACCESS, false, WoWs[x].pid);
		curHandle = x;

		getWoWWindow(WoWs[x].pid);
		/*if (getWoWWindow(WoWs[x].pid))
			vlog("Broadcasting to player %s in process %d", WoWs[x].playerName.c_str(), WoWs[x].pid);*/
	}

	// Key broadcast loop.
	while (thread->running())
	{
		if (GetForegroundWindow() == WoW::window)
			checkKeys();

		Sleep(30);
	}

	delete playerNames;
	thread->exit();
}
