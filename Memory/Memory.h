#pragma once

#include <Windows.h>
#include <vector>
#include <string>

#include <Main\Structures.h>
#include <Main\Constants.h>
#include <WoW API\Object Classes\ObjectManager.h>

#define NULLCALL 0xFFFFFFFE

#define CHECK_BIT(var, pos) ((var & (1 << pos)) == (1 << pos))
#define SET_BIT(var, pos) ((var) &= ~(1 << (pos)))

#define ERROR_CHECK(f) 

struct CTerrainClickEvent 
{
	WGUID guid;
	DWORD button;
	FLOAT X;
	FLOAT Y;
	FLOAT Z;
};

struct AddressSpace
{
	std::string name;
	DWORD base;
	DWORD size;
};

namespace Memory
{
	enum class Error
	{
		Safe = 1,

		Read = -1,
		Write = -2,

		Free = -3,
		Allocate = -4,
		Execute = -5,
		Unallocated = -6,
	};

	class Stub
	{
	private:
		byte *code;
		uint size;

		DWORD address;

		Error error;

		bool _allocated;
	public:
		DWORD getAddress();
		uint  getSize();
		byte *getCode();

		Error getError();

		std::string getErrorString();

		void setAddress(DWORD a);
		void setCode(byte *);
		void setSize(uint);

		bool isSafe();
		bool allocated();

		Error free();
		Error inject();
		Error execute();
		Error allocate();

		Stub(DWORD address, byte *code, uint size);
		Stub(byte *code, uint size);
		~Stub();
	};
	
	extern std::vector<AddressSpace> cache;
	extern HANDLE WoWHandle;
	extern bool cached;

	BOOL Write(DWORD address, void *buffer, size_t size);
	BOOL Write(DWORD address, void *buffer);

	BOOL Read(DWORD address, void *buffer, size_t size);
	BOOL Read(DWORD address, void *buffer);

	bool StoreTlsRetrieveStub(byte *stub, std::size_t size);

	/*template <typename T>
	bool Memory::Read(DWORD address, T *buffer)
	{
		return ReadProcessMemory(WoW::handle, (LPVOID)address, buffer, sizeof(*buffer), NULL);
	}*/

	BOOL CanRead(DWORD address);

	VOID ClearCache();
	BOOL CacheModules();
	BOOL AddressInD3D9(DWORD address);
};

#define rpm Memory::Read
#define wpm Memory::Write

unsigned long dwFindPattern(unsigned char *, char *, unsigned long, unsigned long);

bool InsertTlsStub(byte *stub, std::size_t size);

BOOL FrameAdvance();
BOOL TerrainClick(WOWPOS Pos);
BOOL ObjectInObjMgr(Object &obj);
BOOL GetLocalPlayerName(CHAR *out);
bool GetPlayerName(WGUID GUID, char *szString);

VOID TargetList();

CHAR *GetSpellName(UINT nSpellId);
CHAR *GetPlayerAccount(DWORD dwPID);
CHAR *GetGameObjectName(DWORD dwBase);
CHAR *GetUnitName(DWORD dwBaseAddress);
CHAR *GetGameObjectName(DWORD dwBase, CHAR *szString);

std::vector<std::string> GetPlayersInProximity(float radius);
uint PlayersInProximity(float radius);

UINT PlayersInArea();
UINT PlayersInArea(WGUID playerList[64]);

INT PopulateWoWList(HWND hListBox);
INT GetUnitReaction(INT nFactionID, DWORD dwObjectBase);

HANDLE ObtainWoWHandle();
HANDLE ObtainLOLHandle();

WGUID GetCurrentTargetGUID();
WGUID GetMouseOverGUID();
WGUID GetLastTarget();

FLOAT GetFramerate();

Offsets::eCTM_States GetCTMState();

DWORD GetBGQueue();
DWORD GetTimestamp();
DWORD GetCTMSwitch();
DWORD GetCurrentManager();
DWORD GetWoWMemoryUsage();
DWORD GetLastHardwareAction();
DWORD GetPlayerMovementState();

int MeTargetCallback(CObject *CurrentObject);

Object FindPlayers();
Object GetObjectByGUID(WGUID GUID);
Object GetCurrentTarget(DWORD flags);
Object GetObjectByName(CHAR *szName);
Object DGetObject(DWORD dwBaseAddress);
Object FindLocalBobber(INT *nPlayerIndex);
Object GetObjectByDisplayID(DWORD displayID);
Object GetObjectByGUIDEx(WGUID GUID, DWORD dwFlags);
Object DGetObjectEx(DWORD dwBaseAddress, DWORD dwInfoFlags);
bool DGetObjectEx_Out(DWORD dwBaseAddress, DWORD dwInfoFlags, Object &out);

BOOL Logout();
BOOL GetCTM();
BOOL AntiAFK();
BOOL ResumeWoW();
BOOL AttachWoW();
BOOL IsWalking();
BOOL SuspendWoW();
BOOL ToggleWalk();

BOOL ForceResumeWoW();
BOOL IsPlayerMoving();
BOOL ToggleCTM(BOOL nVal);
BOOL LoadWorld(UINT nMapID);
BOOL UnloadMap(UINT nMapID);

BOOL EnableDebugPrivileges();
BOOL ValidObject(Object obj);
//BOOL PlaySound(CHAR *szSoundFile);
BOOL SetPlayerRotation(FLOAT fRot);
BOOL Interact(DWORD dwBaseAddress);
BOOL TargetObject(WGUID ObjectGUID);
BOOL CastSpellByID(DWORD dwSpellID);
BOOL TargetObjectByWrite(Object WObject);
bool GetPlayerName(WGUID GUID, char *name);
BOOL CompareWGUIDS(WGUID wGuid1, WGUID wGuid2);

BOOL GetWoWBuildInfo(CHAR *szBuildReturn, UINT nPID);
BOOL GetLoginPass(CHAR *stringReturn, size_t maxSize);
BOOL GetUnitName(DWORD dwBaseAddress, CHAR *szString);
BOOL DGetObjectName(Object NewObject, CHAR *szString);
BOOL GetWoWPlayerName(CHAR *szNameReturn, DWORD dwPID);
BOOL CallWoWFunction(DWORD dwAddress, LPVOID Parameter);

BOOL ClickToMove(WOWPOS Pos);
BOOL ClickToMove(WOWPOS pos, BYTE action);
BOOL ClickToMove(FLOAT X, FLOAT Y, FLOAT Z);
BOOL ClickToMove(WOWPOS pos, WGUID guid, BYTE action);
BOOL ClickToMove(FLOAT X, FLOAT Y, FLOAT Z, BYTE action);
BOOL ClickToMove(FLOAT X, FLOAT Y, FLOAT Z, WGUID guid, BYTE action);

VOID Login(CHAR *user, CHAR *pass);
VOID StartClickToTeleport(DWORD dwObjectBase);
VOID FrameScriptSignalEvent(INT eventId, CHAR *format, INT arg);

extern BOOL g_bCurManager;
