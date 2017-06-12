#include "Hacks.h"
#include "Memory.h"
#include "Hooks.h"

#include "Dialog\Dialog.h"
#include "WoW API\Lua.h"
#include "WoW API\Warden.h"

#include <Game\Pointer.h>
#include <Game\Variable.h>

using namespace Offsets;

DWORD g_passStringAddress = NULL;

float g_fFlyHack = 7;

float g_playerWidth = 0;

// Patch class begin.
Patch::Patch(uint size)
{
	len = size;
	restore = new BYTE[size];
	patch = new BYTE[size];
}

uint Patch::setRestore(BYTE *bytes, uint size)
{
	memcpy(restore, bytes, size);
	return size;
}

uint Patch::setRestore(uint pos, uint count, BYTE byte)
{
	uint n = 0;

	for (uint x = pos; x < len, n < count; x++, n++)
	{
		this->restore[x] = byte;
	}

	return n;
}

uint Patch::setPatch(BYTE *bytes, uint size)
{
	memcpy(&patch, bytes, size);
	return size;
}

uint Patch::setPatch(uint pos, uint count, BYTE byte)
{
	uint n = 0;

	for (uint x = pos; x < len, n < count; x++, n++)
	{
		this->patch[x] = byte;
	}

	return n;
}

bool Patch::isNop()
{
	for (uint x = 0; x < this->len; x++)
	{
		if (*(this->restore + x) != NOP)
			return false;
	}

	return true;
}

Patch::~Patch()
{
	len = 0;
	delete patch;
	delete restore;
}

// Patch class end.

void nop(BYTE *bytes, uint len)
{
	for (uint x = 0; x < len; x++)
	{
		bytes[x] = NOP;
	}
}

bool IsNop(BYTE *bytes, uint len)
{
	for (uint x = 0; x < len; x++)
	{
		if (*(bytes + x) != NOP)
			return false;
	}

	return true;
}

// namespace Render start.

namespace Hack
{
	namespace Render
	{
		Patch objectRender(2);
		Patch playerRender(2);

		bool hasObjects()
		{
			BYTE restore[2];
			rpm(RenderObjectPatch, &restore, 2);
			return !IsNop(restore, 2);
		}

		bool hasPlayers()
		{
			BYTE restore[2];
			rpm(RenderPlayerPatch, &restore, 2);
			return !IsNop(restore, 2);
		}

		bool spectateRender(bool flag)
		{

		}

		bool Players(bool flag)
		{ 
			// Restore bytes (je 006E0871).
			BYTE restore[2] = {0x74, 0x0C};

			//rpm(RenderObjectPatch, &restore);
			playerRender.setRestore(restore, 2);
			playerRender.setPatch(0, 2, NOP); // fill patch with nops.

			if (flag)
				wpm(RenderPlayerPatch, playerRender.getRestore(), 2);
			else 
				wpm(RenderPlayerPatch, playerRender.getPatch(), 2);

			return 1;
		}

		bool Objects(bool flag)
		{ // 0x74, 0x0C
			// Restore bytes (jne 00743312).
			BYTE restore[2] = {0x75, 0x09};

			//rpm(RenderObjectPatch, &restore);
			objectRender.setRestore(restore, 2);
			objectRender.setPatch(0, 2, 0x90); // fill patch with nops.

			if (flag)
				wpm(RenderObjectPatch, objectRender.getRestore(), 2);
			else 
				wpm(RenderObjectPatch, objectRender.getPatch(), 2);

			return 1;
		}

		bool M2(bool state)
		{
			DWORD flag = NULL, newFlag = NULL;

			rpm(RenderFlag, &flag, sizeof(flag));
			if (state) // Enable M2 rendering.
				newFlag = flag | (1 >> 0);
			else // Disable M2 rendering.
				newFlag = flag ^ (1 >> 0);

			wpm(RenderFlag, &newFlag, sizeof(newFlag));
			return newFlag == flag; // If no change has occured, return false.
		}
		
		bool WMO(bool state)
		{
			DWORD currentFlag = NULL, newFlag = NULL;

			//if (IsWardenLoaded()) return FALSE;
			rpm(RenderFlag, &currentFlag, sizeof(currentFlag));
	
			if (state) // Enable WMO rendering.
				newFlag = currentFlag | (1 << 8);
			else // Disable WMO rendering.
				newFlag = currentFlag ^ (1 << 8);
	
			wpm(RenderFlag, &newFlag, sizeof(newFlag));
			return newFlag == currentFlag; // If no change has occured, return false.
		}

		bool Terrain(bool state)
		{
			DWORD dwCurrentFlag = NULL, dwNewFlag = NULL;

			//if (IsWardenLoaded()) return FALSE;
			rpm(RenderFlag, &dwCurrentFlag, sizeof(dwCurrentFlag));
	
			if (state) // Enable Terrain rendering.
				dwNewFlag = dwCurrentFlag | (1 << 1);
			else // Disable Terrain rendering.
				dwNewFlag = dwCurrentFlag ^ (1 << 1);
	
			wpm(RenderFlag, &dwNewFlag, sizeof(dwNewFlag));
			return dwNewFlag == dwCurrentFlag; // If no change has occured, return false.
		}
	
		bool hasWorld()
		{
			byte buffer;

			if (!rpm(CGWorldFrame__Render, &buffer, sizeof(buffer)))
				return true;

			if (buffer == 0xC3)
				return true;

			return false;
		}

		bool World(bool state)
		{
			byte patch = 0xC3;
			byte restore = 0x55;
			
			if (state)
				return wpm(CGWorldFrame__Render, &patch, sizeof(patch));
			else
				return wpm(CGWorldFrame__Render, &restore, sizeof(restore));
		}

		bool Water(bool state)
		{
			DWORD dwCurrentFlag = NULL, dwNewFlag = NULL;

			//if (IsWardenLoaded()) return FALSE;
			rpm(RenderFlag, &dwCurrentFlag, sizeof(dwCurrentFlag));
	
			if (state) // Enable Water rendering.
				dwNewFlag = dwCurrentFlag | (1 << 24);
			else // Disable Water rendering.
				dwNewFlag = dwCurrentFlag ^ (1 << 24);
	
			wpm(RenderFlag, &dwNewFlag, sizeof(dwNewFlag));
			return dwNewFlag == dwCurrentFlag; // If no change has occured, return false.
		}
	
		bool HorizonMountains(bool state)
		{
			DWORD currentFlag = NULL, newFlag = NULL;

			//if (IsWardenLoaded()) return FALSE;
			rpm(RenderFlag, &currentFlag, sizeof(currentFlag));
	
			if (state) // Enable HorizonMountain rendering.
				newFlag = currentFlag | (1 << 26);
			else // Disable HorizonMountain rendering.
				newFlag = currentFlag ^ (1 << 26);
	
			wpm(RenderFlag, &newFlag, sizeof(newFlag));
			return newFlag == currentFlag; // If no change has occured, return false.
		}

		bool Wireframe(bool state)
		{
			DWORD currentFlag = NULL, newFlag = NULL;

			//if (IsWardenLoaded()) return FALSE;
			rpm(RenderFlag, &currentFlag, sizeof(currentFlag));
	
			if (state) // Enable Wireframe rendering.
			{
				newFlag = currentFlag | (1 << 29);
				newFlag = newFlag ^ (1 << 26);
			}

			else // Disable Wireframe rendering.
			{
				newFlag = currentFlag ^ (1 << 29);
				newFlag = newFlag | (1 << 26);
			}
	
			wpm(RenderFlag, &newFlag, sizeof(newFlag));
			return newFlag == currentFlag; // If no change has occured, return false.
		}
	};

	namespace Tracking
	{

		DWORD GetState()
		{
			DWORD buffer = NULL;

			rpm(LocalPlayer.unitField().BaseAddress + PLAYER_TRACK_CREATURES, &buffer, sizeof(buffer));
			return buffer;
		}
		
		bool Humanoids(bool state)
		{
			DWORD buffer = 0;

			if (GetState() <= 0 && !state) return true;

			buffer = (state)?(64):(-64) + GetState();
			return WriteProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.unitField().BaseAddress + PLAYER_TRACK_CREATURES), &buffer, sizeof(buffer), NULL);
		}

		bool Clear()
		{
			DWORD buffer = 0;

			return WriteProcessMemory(WoW::handle, (LPVOID)(LocalPlayer.unitField().BaseAddress + PLAYER_TRACK_CREATURES), &buffer, sizeof(buffer), NULL);
		}

	};

	namespace Collision
	{
		float playerWidth;

		float GetPlayerWidth()
		{
			float buffer = 0;

			rpm(LocalPlayer.base() + PlayerWidthOffset, &buffer, sizeof(buffer));
			return buffer;
		}

		bool SetPlayerWidth(float size)
		{
			if (g_playerWidth == 0 && GetPlayerWidth() > 0)
				g_playerWidth = GetPlayerWidth();

			return wpm(LocalPlayer.base() + PlayerWidthOffset, &size, sizeof(size));
		}

		bool isM2Enabled()
		{
			DWORD buffer1 = NULL;
			DWORD buffer2 = NULL;

			rpm(CollisionM2S, &buffer1, sizeof(buffer1));
			rpm(CollisionM2C, &buffer2, sizeof(buffer2));

			return buffer1 != 0x458B1774 || buffer2 != 0x0162840F;
		}

		bool isWMOEnabled()
		{
			DWORD buffer = NULL;

			rpm(CollisionWMO, &buffer, sizeof(buffer));
			return buffer != 0xC0320675;
		}

		bool M2(bool state)
		{
			DWORD buffer1 = NULL;
			DWORD buffer2 = NULL;

			if (state)
			{
				// Collision on M2 bytes.
				buffer1 = 0x458B17EB; // M2S
				buffer2 = 0x0162054A; // M2C
			}
			else
			{
				// Collision off M2 bytes.
				buffer1 = 0x458B1774; // M2S
				buffer2 = 0x0162840F; // M2C
			}

			// Patch M2 collision hack.
			return wpm(CollisionM2S, &buffer1, sizeof(buffer1))
				&& wpm(CollisionM2C, &buffer2, sizeof(buffer2));
		}

		bool WMO(bool state)
		{
			DWORD buffer3 = NULL;

			if (state)
			{
				// Collision on WMO bytes.
				buffer3 = 0xC0329090; // WMO
			}
			else
			{
				// Collision off WMO bytes.
				buffer3 = 0xC0320675; // WMO
			}

			// Patch WMO collision hack.
			return wpm(CollisionWMO, &buffer3, sizeof(buffer3));
		}

		bool All(bool state)
		{
			return M2(state) && WMO(state);
		}
	};

	namespace NameFilter
	{
		// Determines if the name/rename filter were patched.
		bool isPatched()
		{
			BYTE bufferName[2];
			BYTE bufferRename;

			rpm(NameFilterPatch, &bufferName, sizeof(bufferName));
			rpm(RenameFilterPatch, &bufferRename, sizeof(bufferRename));

			// Checks both rename and name creation patches.
			return (bufferName[0] != 0x0F && bufferName[1] != 0x84) || (bufferRename != 0x74);
		}

		bool Restore()
		{
			BYTE restoreName[] = {
				0x0F, 0x84, 0x55, 0xFF, 0xFF, 0xFF
			};

			BYTE restoreRename = 0x74;
	
			return wpm(RenameFilterPatch, &restoreRename, sizeof(restoreRename))
				&& wpm(NameFilterPatch, &restoreName, sizeof(restoreName));
		}

		bool Patch()
		{
			// 004E0484 - 0F84 55FFFFFF - je 004E03DF Change into
			// 004E0484 - E9 56FFFFFF   - jmp 004E03DF
			// 004E0489 - 90
	
			BYTE patchRename = 0xEB;
			BYTE nopSlide[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
			BYTE patch[] = {
				0xE9, 0x56, 0xFF, 0xFF, 0xFF
			};

			if (isPatched()) return false; 
	
			return wpm(NameFilterPatch, &nopSlide, sizeof(nopSlide))
				&& wpm(NameFilterPatch, &patch, sizeof(patch))
				&& wpm(RenameFilterPatch, &patchRename, sizeof(patchRename));
		}
	};

	namespace Movement
	{
		DWORD GetState()
		{
			return *WoW::Pointer<DWORD>(LocalPlayer.base() + Offsets::MovementState);
		}

		DWORD GetState(DWORD base)
		{
			return *WoW::Pointer<DWORD>(base + Offsets::MovementState);
		}
		
		bool SetState(DWORD state)
		{
			WoW::Variable<DWORD> write(LocalPlayer.base() + MovementState);

			// Set write = state
			write << (DWORD)state;
			return write == state;
		}

		bool SetState(DWORD base, DWORD state)
		{
			WoW::Variable<DWORD> write(base + MovementState);

			// Set write = state
			write << (DWORD)state;
			return write == state;
		}

		bool Teleport(WOWPOS Pos)
		{
			return wpm(LocalPlayer.base() + OBJECT_X, &Pos, sizeof(Pos) - sizeof(float));
		}
		
		bool Teleport(Object *object, WOWPOS pos)
		{
			return wpm(object->BaseAddress + OBJECT_X, &pos, sizeof(pos) - sizeof(float));
		}
		
		bool Teleport(float X, float Y, float Z)
		{
			WOWPOS wBuff;

			wBuff.X = X;
			wBuff.Y = Y;
			wBuff.Z = Z;
			return Teleport(wBuff);
		}

		bool Desync(bool state)
		{
			// call 006B0B50
			BYTE restore[] = { 0xE8, 0x83, 0x19, 0xF9, 0xFF };
			BYTE nopSlide[5];

			// Set all bytes to NOP
			memset(nopSlide, 0x90, 5);

			// Desync on
			if (state)
				return wpm(PlayerDesyncPatch, nopSlide, 5);
			else
				return wpm(PlayerDesyncPatch, restore, 5);

		}

		bool isFlying(DWORD base)
		{
			DWORD   buffer;
			CObject obj(base);

			rpm(obj.unitField().BaseAddress + 0x36, &buffer, sizeof(buffer));
			rpm(buffer + 0x44, &buffer, sizeof(buffer));
			return buffer & 0x2000000;
		}

		bool SetSpeed(float speed)
		{
			return wpm(LocalPlayer.base() + MovementSpeedActive, &speed, sizeof(speed));
		}

		bool SetSwimSpeed(float speed)
		{
			return wpm(LocalPlayer.base() + MovementSpeedSwimming, &speed, sizeof(speed));
		}
		
		bool SetFlightSpeed(float flightSpeed)
		{
			return wpm(LocalPlayer.base() + MovementSpeedFlying, &flightSpeed, sizeof(flightSpeed));
		}

		bool Fly(bool state, float flightSpeed)
		{
			DWORD dwFlying = 0x83000000; 
			DWORD dwDefault = 0x8000000;

			//return false;
			if (!WoW::InGame()) return FALSE;
			//if (Warden::IsLoaded()) return FALSE;

			if (state)
			{
				SetFlightSpeed(flightSpeed);
				SetState(LocalPlayer.base(), dwFlying);
			}
			else
			{
				flightSpeed = 7;
				SetFlightSpeed(flightSpeed);
				return StopFall();
			}

			return SetMagicCheck(IDC_MAIN_HACKS_CHECK_FLY, state) == true;
		}

		bool NoFallDamage(bool state)
		{
			// Patch1: jnz short loc_73D4D9(0x75, 0x18)->nopslide(0x90, 0x90)
			// Patch2: jnz short loc_73D503 (0x75, 0x2A) -> jmp loc_73D503 (0xEB, 0x2A)

			BYTE restore1[] = { 0x75, 0x18 };
			BYTE restore2[] = { 0x75, 0x2A };

			BYTE patch1[] = { 0x90, 0x90 };
			BYTE patch2[] = { 0xEB, 0x2A };

			if (state)
			{
				return wpm(NoFallDamagePatch1, &patch1, sizeof(patch1))
					&& wpm(NoFallDamagePatch2, &patch2, sizeof(patch2));
			}
			else
			{
				return wpm(NoFallDamagePatch1, &restore1, sizeof(restore1))
					&& wpm(NoFallDamagePatch2, &restore2, sizeof(restore2));
			}
		}

		bool FreezeZ(bool state)
		{
			DWORD dwFrozen = 0x80000400;

			if (!WoW::InGame()) return FALSE;

			if (state)
			{
				return SetState(LocalPlayer.base(), dwFrozen);
			}
			else
			{
				return StopFall();
			}
		}

		bool WeirdFly(bool State, float flightSpeed)
		{
			DWORD dwFlying = 0x82000000; 
			DWORD dwDefault = 0x8000000;

			if (!WoW::InGame()) return FALSE;
			//if (Warden::IsLoaded()) return FALSE;

			if (State)
			{
				SetFlightSpeed(flightSpeed);
				SetState(LocalPlayer.base(), dwFlying);
			}
			else
			{
				flightSpeed = 7;
				SetFlightSpeed(flightSpeed);
				return StopFall();
			}

			return SetMagicCheck(IDC_MAIN_HACKS_CHECK_FLY, State) == true;
		}

		bool StopFall()
		{
			DWORD defaultState = 0x80000000;

			if (GetState() == defaultState) return true;
			return SetState(LocalPlayer.base(), defaultState);
		}
	};

	namespace Morph
	{
		std::vector<DWORD> savedIds;

		DWORD GetTargetDisplayId()
		{
			// Get local player's target.
			CUnit *target = &LocalPlayer.getTarget();
			target->update(UnitFieldInfo);

			// Return target's displayId.
			return target->displayId();
		}

		void Save(DWORD displayId)
		{
			savedIds.push_back(displayId);
		}

		void MorphTarget(uint displayId)
		{
			CUnit &target = LocalPlayer.getTarget();

			// Set the unit's display Id.
			target.setDisplayId(displayId);

			// Update unit's model.
			target.UpdateDisplayInfo();
		}

		void Morph(DWORD displayId)
		{
			// Set the local player's display Id.
			LocalPlayer.setDisplayId(displayId);

			// Update local player model.
			LocalPlayer.UpdateDisplayInfo();
		}

		void MorphMount(DWORD displayId)
		{
			// Set the local player's display Id.
			LocalPlayer.setMountDisplayId(displayId);

			// Update local player model.
			LocalPlayer.UpdateMountDisplayInfo();
		}

		void Target()
		{			
			// Get display id of the local player's target.
			DWORD displayId = GetTargetDisplayId();
			vlog("Display Id: %d", displayId);

			// Set the local player's display Id.
			Morph(displayId);
		}

	};

};

bool LuaProtection()
{
	// replace first byte at CGGameUI__CheckPermissions (push ebp) with retn (0xC3). ez. lolnope, ban.
	return TRUE;
}

bool EnableHacks()
{
	if (GetMagicCheck(IDC_MAIN_HACKS_CHECK_FLY))
		Hack::Movement::Fly(true, g_fFlyHack);

	if (GetMagicCheck(IDC_MAIN_HACKS_CHECK_ANIMATEPATCH))
		PreAnimatePatch(true);

	if (GetMagicCheck(IDC_MAIN_HACKS_CHECK_LANGUAGEHACK))
		LanguageHack();

	if (GetMagicCheck(IDC_MAIN_TAB_HACKS_CHECK_CTT))
		StartClickToTeleport(LocalPlayer.base());

	if (GetMagicCheck(IDC_MAIN_HACKS_CHECK_TRACKHUMANOIDS))
		Hack::Tracking::Humanoids(true);

	if (GetMagicCheck(IDC_MAIN_HACKS_CHECK_MAPTELEPORT))
		Lua::DoString("if Nx.Map ~= nil then hooksecurefunc(Nx.Map, \"SeT3\", function(self,typ,x1,y1,x2,y2,tex2,id,nam,kee,maI) dMapNew = 1 dMapX, dMapY = self:GZP(self.MaI, x1, y1) dMapZ = self.MWI[self.MaI].Zon end) end");

	return TRUE;
}

// Proof of concept
bool LoginHack()
{
	// To be used with GetLoginPass(buff, 256);
	BYTE loginStub[] = 
	{
		0x9C,							// pushfd
		0x60,							// pushad
		0xBA, 0x00, 0x00, 0x00, 0x00,	// mov edx,g_passStringAddress
		0xB9, 0x00, 0x00, 0x00, 0x00,	// mov ecx, 0
		0x8B, 0xFC,						// mov edi,esp - loopStart
		0x83, 0xC7, 0x2C,				// add edi,2C
		0x8B, 0x3F,						// mov edi,[edi]
		0x01, 0xCF,						// add edi,ecx
		0x8A, 0x1F,						// mov bl,[edi]
		0x88, 0x1A,						// mov [edx],bl
		0x83, 0xC1, 0x01,				// add ecx,01
		0x83, 0xC2, 0x01,				// add edx,01
		0x80, 0xFB, 0x00,				// cmp bl,00
		0x75, 0xE8,						// jne loopStart
		0x61,							// popad
		0x9D							// popfd
	};

	//if (IsWardenLoaded()) return FALSE;
	if (IsSubroutineHooked(Offsets::CGlueMgr__DefaultServerLogin)) return FALSE;

	g_passStringAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (g_passStringAddress == NULL) return FALSE;
	
	memcpy(&loginStub[3], &g_passStringAddress, sizeof(g_passStringAddress));

	SuspendWoW();
	HookDefaultServerLogin();
	ChangeHookFunction(Offsets::CGlueMgr__DefaultServerLogin, wDefaultServerLogin.Cave, loginStub, sizeof(loginStub));
	ResumeWoW();

	return TRUE;
}

bool LanguageHack()
{
	// Detected by warden. 
	log("Detected");
	//return false;
	if (!WoW::InGame()) return FALSE;
	if (Warden::IsLoaded() && !Warden::IsHackProtected(CGGameChat__AddChatMessage)) 
	{
		SetMagicCheck(IDC_MAIN_HACKS_CHECK_LANGUAGEHACK, false);
		log("This hack is detected and Warden is currently");
		log("enabled. Please turn on hack protection to use ");
		log("this feature.");
		return false;
	}

	if (IsSubroutineHooked(CGGameChat__AddChatMessage)) return FALSE;

	BYTE languagePatch[] = 
	{
		0x9C,											// pushfd 
		0x83, 0x7C, 0x24, 0x0C, 0x06,					// cmp dword ptr [esp+0C],06
		0x74, 0x07,										// jne here+0x7
		0x83, 0x7C, 0x24, 0x0C, 0x01,					// cmp dword ptr [esp+0C],01
		0x75, 0x08,										// jne here+0x8
		0xC7, 0x44, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00,	// mov [esp+14],00000000
		0x9D,											// popfd 
	};

	SuspendWoW();
	HookLanguageHack();
	ChangeHookFunction(CGGameChat__AddChatMessage, wLanguage.Cave, languagePatch, sizeof(languagePatch));
	ResumeWoW();

	return TRUE;
}

bool ChangePI(float value)
{
	DWORD old = PROCESS_VM_READ;
	// WOW_PI is a read-only address, so we need to change the page memory protection flags.
	VirtualProtectEx(WoW::handle, (LPVOID)WOW_PI, sizeof(float), PAGE_EXECUTE_READWRITE, &old);
	wpm(WOW_PI, &value, sizeof(value));

	return VirtualProtectEx(WoW::handle, (LPVOID)WOW_PI, sizeof(float), old, NULL);
}

bool PreAnimatePatch(bool state)
{
	WoW::Variable<byte> patch(Offsets::CGPlayer__C__PreAnimatePatch);

	if (state)
		patch << (byte)0xC3;
	else
		patch << (byte)0x55;

	return 1;
}
