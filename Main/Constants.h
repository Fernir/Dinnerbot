#pragma once 

// Good pastebin: http://pastebin.com/iBBc6dEn

#define PI 3.141592653589793238462643383279502884

//Player Information
#define PLAYER_NAME 0x00C79D18 // 3.3.5a Tested
#define PLAYER_BASE 0xCD87A8  // 3.3.5a 
#define CURRENT_TARGET_GUID 0x00BD07B0 //3.3.5a Tested
#define LAST_TARGET_GUID 0x00BD07B8 //3.3.5a 
#define ACCOUNT_NAME 0x00D43128 // 2.4.3 Tested

//#define INGAME 0xBEBA40 // 3.3.5a Tested

// Fishing Bot
#define FISHING_BOBBER_DISPLAYID 0x29C	// 3.3.5a Tested
#define FISHING_CASTING_SPELL    0xC85E	// 3.3.5a Tested

// Build Info
#define WOW_NAME    0x00CABBB8	// 3.3.5a 12340
#define BUILD_MAJOR 0x009F5208	// 3.3.5a 12340
#define BUILD_MINOR 0x009F5200	// 3.3.5a 12340
#define SUPPORTED_BUILD "World of Warcraft 3.3.5 (12340)"

#define UNIT_CASTING_SPELL 0xA60 // 3.3.5a 12340

#define GAMEOBJECT_X 0xE8	// 3.3.5a 12340
#define GAMEOBJECT_Y 0xEC	// 3.3.5a 12340
#define GAMEOBJECT_Z 0xF0	// 3.3.5a 12340

#define UNIT_NAME_OFFSET1 0x1A4	// 3.3.5a 
#define UNIT_NAME_OFFSET2 0x90	// 3.3.5a

#define PONT_MODE g_chatLog && g_updateWoWChat && g_rosterMode
#define PONT_DANGER_MESSAGE "Help, I'm in danger! http://i.imgur.com/ARQmKnk.jpg"

namespace Offsets
{
	enum eLogin
	{
		QueuePosition = 0x00B6A9BC,
		LoginScreenStatus = 0xD41660,
		AccountName = 0x00B6AA40,                // 3.3.5a 12340
		RealmName = 0x00C79B9E,                  // 3.3.5a 12340
	};

	enum eGameStates
	{

		InGame = 0xBEBA40,
		IsLoadingOrConnecting = 0x00B6AA38,         // 3.3.5a 12340

		// **Test this address for detecing if dinnerchat is active.
		IsLoading = 0x00BD0793,					// 3.3.5a 12340
	};

	enum eMapInfo
	{
		MapID = 0xBE8F0C,

		ZoneID = 0x00BD080C,						// 3.3.5a 12340 or 00ADF318 
		MapName = 0xCE06D0,
	};

	enum eLocalPlayer
	{
		PlayerName = 0x00C79D18,    // 3.3.5a 12340
		PlayerBase = 0xCD87A8,		// 3.3.5a 
	};

	enum eObjectManager
	{
		ClntObjMgrObjectPtr = 0x004D4DB0,           // 3.3.5a 12340
		ClntObjMgrGetActivePlayer = 0x004D3790,     // 3.3.5a 12340
		ClntObjMgrGetActivePlayerObj = 0x4038F0,    // 3.3.5a 12340

		ClientConnection = 0x00C79CE0,	// 3.3.5a 12340
		CurrentManager = 0x2ED0,
		TlsIndex = 0x2C,
		PlayerGUID = 0xC0,

		// Should be under object descriptors
		FirstEntry = 0xAC,
		ObjectType = 0x14,
		ObjectGUID = 0x30,
		NextObject = 0x3C,
		ObjectField = 0x08,
		PlayerField = 0x1198,

		XCoordinate = 0x798,
		YCoordinate = 0x79C,
		ZCoordinate = 0x7A0,
		Rotation = 0x7A8,

#define OBJECT_GUID  0x30 // 3.3.5a Tested
#define OBJECT_NEXT  0x3C // 3.3.5a Tested
#define OBJECT_UNIT_FIELD 0x08 // 3.3.5a Tested
#define OBJECT_GAMEOBJECT_FIELD 0x08 // 3.3.5a Tested
#define OBJECT_ITEM_FIELD 0x08 // 3.3.5a Tested
#define OBJECT_PLAYER_FIELD 0x1198 // ?
#define OBJECT_X 0x798 // 3.3.5a Tested
#define OBJECT_Y 0x79C // 3.3.5a Tested
#define OBJECT_Z 0x7A0 // 3.3.5a Tested
#define OBJECT_ROT 0x7A8 // 3.3.5a Tested
	};

	enum eBuildInfo
	{
		BuildName = 0x00CABBB8,		// 3.3.5a 12340
		BuildMajor = 0x009F5208,	// 3.3.5a 12340
		BuildMinor = 0x009F5200,	// 3.3.5a 12340
	};

	enum eGlobals
	{
		BattlegroundStatus = 0x00BEA4D0,            // 3.3.5a 12340
		CurrentTargetGUID = 0x00BD07B0,             // 3.3.5a 12340
		LastTargetGUID = 0x00BD07B8,                // 3.3.5a 12340
		MouseOverGUID = 0x00BD07A0,                 // 3.3.5a 12340
		FollowGUID = 0x00CA11F8,                    // 3.3.5a 12340
		ComboPoints = 0x00BD084D,                    // 3.3.5a 12340
		LootWindow = 0x00BFA8D8,                    // 3.3.5a 12340
		KnownSpell = 0x00BE5D88,                    // 3.3.5a 12340
		Movement_Field = 0xD8,                      // 3.3.5a 12340
		SpellCooldownPtr = 0x00D3F5AC,              // 3.3.5a 12340
		
		// Character select
		CharSelectIndex = 0x00AC436C,				// 3.3.5a 12340
		CharSelectMaxIndex = 0x00B6B23C,			// 3.3.5a 12340

		Timestamp = 0x00B1D618,                     // 3.3.5a 12340
		LastHardwareAction = 0x00B499A4,            // 3.3.5a 12340

		UnitName1 = 0x964,                          // 3.3.5a 12340
		UnitName2 = 0x5C,                           // 3.3.5a 12340
		nbItemsSellByMerchant = 0x00BFA3F0,         // 3.3.5a 12340
		ZoneTextPtr = 0x00BD077C,					// 3.3.5a 12340
		GetZoneText = 0x00BD0788,                 // 3.3.5a 12340
		GetSubZoneText = 0x00BD0784,              // 3.3.5a 12340
		GetInternalMapName = 0x00CE06D0,            // 3.3.5a 12340
		LocalGUID = 0x00CA1238,                     // 3.3.5a 12340
		IsBobbingOffset = 0xBC,                     // 3.3.5a 12340
		ChatboxIsOpen = 0x00D41660,                 // 3.3.5a 12340
		M2Model__IsOutdoors = 0x0077FBF0,           // 3.3.5a 12340
		CVar_MaxFPS = 0x00C5DF7C,                   // 3.3.5a 12340

		CInputControl = 0x00C24954,                 // 3.3.5a 12340
		isDBCCompressed = 0x00C5DEA0,
	};

	enum eDBC
	{
		AreaTableDB = 0x00AD3134,				// 3.3.5a 12340
		ChrClassesDB = 0x00AD3404,				// 3.3.5a 12340
		PowerDisplayDB = 0x00AD43A0,
	};

	enum eCVars
	{
		threatWarning = 0xBD08C0,					// 3.3.5a 12340
		consoleKey = 0x00ADBAC4,					// 3.3.5a 12340
	};

	enum eGuild
	{
		rosterList = 0x00C22AE0,					// 3.3.5a 12340
		numGuildMembers = 0x00C22AB0,				// 3.3.5a 12340
		numOnlineGuildMembers = 0xC22AB4,			// 3.3.5a 12340

		sub_GuildRoster = 0x005CA9B0,				// 3.3.5a 12340
	};

	enum eFunctions
	{
		CInputControl__UpdatePlayer = 0x005FBBC0,
		CInputControl__SetControlBit = 0x005FA170,

		CGWorldFrame__OnFrameRender = 0x004FB080,		// 3.3.5a 12340
		CGWorldFrame__RenderWorld = 0x004FAF90,			// 3.3.5a 12340
		CGWorldFrame__Intersect = 0x0077F310,			// 3.3.5a 12340
		CGWorldFrame__Render = 0x004F8EA0,				// 3.3.5a 12340

		CGGameUI__Target = 0x00524BF0,					// 3.3.5a 12340 - Crashes on occasion.
		CGGameUI__Reload = 0x00513530,					// 3.3.5a 12340
		CGGameUI__PlaySound = 0x004C74B3,				// 3.3.5a 12340 - Usage: CGameUI__PlaySound("igCharacterSelect", 0, 0, 0);
		CGGameUI__CheckPermissions = 0x005191C0,		// 3.3.5a 12340

		CGGameChat__AddChatMessage = 0x00509DD0,		// 3.3.5a 12340

		ObjectBeingLooted = 0xBFA8D8,

		CGLootWindow__LootSlot = 0x00589140,				// 3.3.5a 12340
		CGLootWindow__GetLootSlotName = 0x00588430,		// 3.3.5a 12340

		CGLootWindow__CloseLoot = 0x00523640,			// 3.3.5a 12340

		EnumVisibleObjects = 0x004D4B30,				// 3.3.5a 12340

		Warden__ShutdownAndUnload = 0x007DA420,			// 3.3.5a 12340
		WardenClient_Initialize = 0x007DA8C0,			// 3.3.5a 12340
		Warden__Data = 0x007DAAB0,						// 3.3.5a 12340

		Lua__DefaultServerLogin = 0x004DC260,			// 3.3.5a 12340
		Lua__EnterWorld = 0x004DC510,					// 3.3.5a 12340
		CGlueMgr__DefaultServerLogin = 0x004D8A30,		// 3.3.5a 12340
		CGlueMgr__CharacterSelect = 0x4E3CD0,			// 3.3.5a 12340

		ConsoleWrite = 0x00765270,						// 3.3.5a 12340 - BANNED after setting bp at beginning of function.

		CWorld__UnloadMap = 0x004F7020,					// 3.3.5a 12340
		LoadNewWorld = 0x00403B70,						// 3.3.5a 12340

		Traceline = 0x007A3B70, 						// 3.3.5a 12340

		CGGuildRegistrar__BuyGuildCharter = 0x006DD3A0,	// 3.3.5a 12340

		Lua_Logout = 0x00510430,						// 3.3.5a 12340
		Lua_QuitGame = 0x004DC050,
		Lua_ReloadUI = 0x005222E0,						// 3.3.5a 12340			
		Lua_AcceptTrade = 0x005866E0,					// 3.3.5a 12340
		Lua_CancelTrade = 0x005109E0,					// 3.3.5a 12340
		Lua_GetSpellName = 0x005407F0,					// 3.3.5a 12340
		Lua_CastSpellByID = 0x0053E060,					// 3.3.5a 12340 
		Lua_SendChatMessage = 0x0050D170,				// 3.3.5a 12340 - BANNED after setting bp at beginning of function.

		FrameScript_GetTop = 0x0084DBD0,				// 3.3.5a 12340
		FrameScript_GetText = 0x00819D40,
		FrameScript_Execute = 0x00819210,
		FrameScript_SignalEvent = 0x0081B530,			// 3.3.5a 12340

		CharSelectIndexAddress = 0xAC436C,

		FrameScript__RegisterFunction = 0x00817F90,
		FrameScript__UnregisterFunction = 0x00817FD0,

		CGUnit_C__GetCreatureType = 0x0071F300,			// 3.3.5a 12340
		CGUnit_C__GetFrameScriptText = 0x007225E0,		// 3.3.5a 12340
		CGUnit_C__TryChangeStandState = 0x006DCB40,		// 3.3.5a 12340
		CGUnit_C__ChatBubbleShow = 0x00720010,			// 3.3.5a 12340 push string then push type, 
		CGUnit_C__UpdateDisplayInfo = 0x0073E410,
		CGUnit_C__OnMountDisplayChanged = 0x00740450,
		
		CGUnit_C__WriteMovementPacketWithTransport = 0x0071F0C0,

		CGPlayer_C__TryChangeStandState = 0x006DCB40,

		Spell_C__CastSpell = 0x0080DA40,				// 3.3.5a 12340
		Spell_C__HandleTerrainClick = 0x0080C350,		// 3.3.5a 12340
		HandleTerrainClick = 0x00527830,				// 3.3.5a 12340
	};

	enum eCGWorldFrameHitFlags
	{
		HitTestNothing = 0x0,

		/// Models' bounding, ie. where you can't walk on a model. (Trees, smaller structures etc.)
		HitTestBoundingModels = 0x1,

		/// Structures like big buildings, Orgrimmar etc.
		HitTestWMO = 0x10,

		/// Used in ClickTerrain.
		HitTestUnknown = 0x40,

		/// The terrain.
		HitTestGround = 0x100,

		/// Tested on water - should work on lava and other liquid as well.
		HitTestLiquid = 0x10000,

		/// This flag works for liquid as well, but it also works for something else that I don't know (this hit while the liquid flag didn't hit) - called constantly by WoW.
		HitTestUnknown2 = 0x20000,

		/// Hits on movable objects - tested on UC elevator doors.
		HitTestMovableObjects = 0x100000,

		HitTestLOS = HitTestWMO | HitTestBoundingModels | HitTestMovableObjects,
		HitTestGroundAndStructures = HitTestLOS | HitTestGround
	};


	/*
	int __cdecl ClntObjMgrLinkInNewObject(CGObject_C *a1) = 0x004D6BC0,
	signed int __usercall CreateObject<eax>(int ebx0<ebx>, CDataStore *edi0<edi>, int a3<esi>, int a1) = 0x004D6C00

	*/


	enum eHacks
	{
		PlayerWidthOffset = 0x850,

		// In warden_checks
		CollisionM2S = 0x007A52EC,
		CollisionM2C = 0x007A50CF,
		CollisionWMO = 0x007AE7EA,

		MovementSpeedCurrent = 0x814,
		MovementSpeedWalking = 0x818,
		MovementSpeedActive = 0x81C,
		MovementSpeedSwimming = 0x824,
		MovementSpeedFlying = 0x82C,
		MovementState = 0x7CC, // 3.3.5a 12340 Offset from player base.
		// (0 0 0 3) flying
		// (0 0 0 128) default - stop fall
		// (1 0 0 128) run forward
		// (0 0 0 96) levitate
		// (0 1 0 128) walking

		NoFallDamagePatch1 = 0x0073D4BF, // jnz short loc_73D4D9 (0x75, 0x18) -> nopslide (0x90, 0x90).
		NoFallDamagePatch2 = 0x0073D4D7, // jnz short loc_73D503 (0x75, 0x2A) -> jmp loc_73D503 (0xEB, 0x2A).

		FogColorPatch = 0x7F172B,
		FogColor = 0xD38B8C,
		FogDensity = FogColor + 12,
		RenderFlag = 0xCD774C,	// 3.3.5a 12340

		CGPlayer_C__ShouldRender_Patch = 0x006E0863,
		CGPlayer__C__PreAnimatePatch = 0x006D5EB0, // 3.3.5a 12340 - Note: just retn.

		RenderPlayerPatch = 0x006E0863, // 3.3.5a 12340 - nopslide 2 bytes. (0x74, 0x0C)
		RenderObjectPatch = 0x00743307, // 3.3.5a 12340 - nopslide 2 bytes. (0x75, 0x09)
		WOW_PI = 0x009E8D34, // Change value of PI to negative number for spinning.

		PlayerDesyncPatch = CGUnit_C__WriteMovementPacketWithTransport + 0x108, // call SendPacket -> nop slide (5 bytes)

		WaterWalkPatch = 0x0075E439, // In warden_checks
		NameFilterPatch = 0x004E0484,
		RenameFilterPatch = 0x004D8D58 // JE (0x74) to JMP (0xEB) to patch.
	};

	enum eCorpse
	{
		X = 0x00BD0A58,                              // 3.3.5a 12340
		Y = X + 0x4,                                 // 3.3.5a 12340
		Z = X + 0x8,                                 // 3.3.5a 12340
	};

	enum eCameraOffsets
	{
		CGWorldFrame__GetActiveCamera = 0x4F5960,   // 3.3.5a 12340
		Camera_Pointer = 0x00B7436C,                // 3.3.5a 12340
		Camera_Offset = 0x7E20,                     // 3.3.5a 12340
		Camera_X = 0x8,                             // 3.3.5a 12340
		Camera_Y = 0xC,                             // 3.3.5a 12340
		Camera_Z = 0x10,                            // 3.3.5a 12340
		Camera_Follow_GUID = 0x88,                  // 3.3.5a 12340
	};

	enum eLua
	{
		/*Lua_State = 0x00D3F78C,                     // 3.3.5a 12340
		Lua_DoString = 0x00819210,                  // 3.3.5a 12340
		Lua_GetLocalizedText = 0x007225E0,          // 3.3.5a 12340
		Lua_SetTop = 0x000084DBF0,                  // 3.3.5a 12340*/
	};

	enum eMovements
	{
		MoveForwardStart = 0x005FC200,              // 3.3.5a 12340
		MoveForwardStop = 0x005FC250,               // 3.3.5a 12340
		MoveBackwardStart = 0x005FC290,             // 3.3.5a 12340
		MoveBackwardStop = 0x005FC2E0,              // 3.3.5a 12340
		TurnLeftStart = 0x005FC320,                 // 3.3.5a 12340
		TurnLeftStop = 0x005FC360,                  // 3.3.5a 12340
		TurnRightStart = 0x005FC3B0,                // 3.3.5a 12340
		TurnRightStop = 0x005FC3F0,                 // 3.3.5a 12340
		JumpOrAscendStart = 0x005FBF80,             // 3.3.5a 12340
		AscendStop = 0x005FC0A0,                    // 3.3.5a 12340
	};

	enum eParty
	{
		LeaderGUID = 0x00BD1968,                  // 3.3.5a 12340
		sMember1GUID = 0x00BD1948,                 // 3.3.5a 12340
		sMember2GUID = sMember1GUID + 0x8,        // 3.3.5a 12340
		sMember3GUID = sMember2GUID + 0x8,        // 3.3.5a 12340
		sMember4GUID = sMember3GUID + 0x8,        // 3.3.5a 12340
	};

	enum eDirect3D9
	{
		pDevicePtr_1 = 0x00C5DF88,                  // 3.3.5a 12340
		pDevicePtr_2 = 0x397C,                      // 3.3.5a 12340
		oBeginScene = 0xA4,                         // 3.3.5a 12340
		oEndScene = 0xA8,                           // 3.3.5a 12340
		oClear = 0xAC,                              // 3.3.5a 12340
	};

	enum eUnitBaseGetUnitAura
	{
		CGUnit_Aura = 0x00556E10,                   // 3.3.5a 12340
		AURA_COUNT_1 = 0xDD0,                       // 3.3.5a 12340
		AURA_COUNT_2 = 0xC54,                       // 3.3.5a 12340
		AURA_TABLE_1 = 0xC50,                       // 3.3.5a 12340
		AURA_TABLE_2 = 0xC58,                       // 3.3.5a 12340
		AURA_SIZE = 0x18,                           // 3.3.5a 12340
		AURA_SPELL_ID = 0x8                         // 3.3.5a 12340
	};

	enum eClickToMove
	{
		CGPlayer_C__ClickToMove = 0x00727400,       // 3.3.5a 12340 Molten

		CTM_Activate_Start = 0x00CA19FC,			// 3.3.5a 12340 Molten
		CTM_Activate_Offset = 0x30,                 // 3.3.5a 12340 Molten
		CTM_Constant = 0xE4DB642D,	                // 3.3.5a 12340 Molten

		CTM_Base = 0x00CA11D8,                      // 3.3.5a 12340 Molten
		CTM_X = 0x8C,                               // 3.3.5a 12340 Molten
		CTM_Y = 0x90,                               // 3.3.5a 12340 Molten
		CTM_Z = 0x94,                               // 3.3.5a 12340 Molten

		CTM_TurnSpeed = 0x4,                        // 3.3.5a 12340
		CTM_Distance = 0xC,                         // 3.3.5a 12340
		CTM_Action = 0x1C,                          // 3.3.5a 12340
		CTM_GUID = 0x20,                            // 3.3.5a 12340 
		CTM_Tolerance = 0xADAAAC
	};

	enum eAutoLoot
	{
		AutoLoot_Activate_Pointer = 0x00BD0914,     // 3.3.5a 12340
		AutoLoot_Activate_Offset = 0x30,            // 3.3.5a 12340
	};

	enum eSpectatorOffsets
	{
		Spectator_Player_Base = 0x1008, /* + PlayerBase */
		Spectator_Player_IsSpectating = 0x0A, /* + [PlayerBase + Spectator_Player_Base]. byte => 255 => IsCommentator returns true */
		Spectator_Base = 0xACE4A8,
		Spectator_X = 0x0C,
		Spectator_Y = 0x10,
		Spectator_Z = 0x14,
		Spectator_FollowGUID = 0x20, /* Set to own GUID */
		Spectator_CamZoom = 0x40,
		Spectator_CamSpeed = 0x44,
		Spectator_Collision = 0x48,
	};

	enum eAutoSelfCast
	{
		AutoSelfCast_Activate_Pointer = 0xBD0920,   // 3.3.5a 12340
		AutoSelfCast_Activate_Offset = 0x30,        // 3.3.5a 12340
	};

	enum eWoWChat
	{
		ChatBufferStart = 0x00B75A60,               // 3.3.5a 12340
		CurrentChatPosition = 0x00B67F24,			// 3.3.5a 12340
		MessageSender = 0xC,						// 3.3.5a 12340
		MessageSequence = 0x17B4,
		FormattedMessage = 0x3C,					// 3.3.5a 12340
		RawMessage = 0xBF4,							// 3.3.5a 12340
		MessageType = 0x17AC,						// 3.3.5a 12340
		NextMessage = 0x17C0,                       // 3.3.5a 12340
	};

	enum ePlayerNameOffsets
	{
		NameCache = 0x00C5D938 + 0x8,	// 3.3.5a Tested Molten
		NameMask = 0x24,				// 3.3.5a Tested Molten
		NameBase = 0x1C,				// 3.3.5a Tested Molten
		NameString = 0x20				// 3.3.5a Tested Molten
	};

	enum eCTM_States
	{
		CTM_Moving = 0x4,	// 3.3.5a Tested Molten
		CTM_Stopped = 0xD,	// 3.3.5a Tested Molten
		CTM_FaceTarget = 0x1,
		CTM_Face = 0x2,
		CTM_Stop_ThrowsException = 0x3,
		CTM_Move = 0x4,
		CTM_NpcInteract = 0x5,
		CTM_Loot = 0x6,
		CTM_ObjInteract = 0x7,
		CTM_FaceOther = 0x8,
		CTM_Skin = 0x9,
		CTM_AttackPosition = 0xA,
		CTM_AttackGuid = 0xB,
		CTM_ConstantFace = 0xC,
		CTM_None = 0xD,
		CTM_Attack = 0x10,
		CTM_Idle = 0x13,
	};

	// Descriptors
	enum ePlayerStruct
	{
		PLAYER_STRUCT_X = 0xBF0, // 2.4.3 Tested
		PLAYER_STRUCT_Y = 0xBF4, // 2.4.3 Tested
		PLAYER_STRUCT_Z = 0xBF8, // 2.4.3 Tested
		PLAYER_STRUCT_FACTION = 0x26CC,
		PLAYER_STRUCT_MOVEMENT_CURRENT_SPEED = 0xC68, // 2.4.3 Tested
		PLAYER_STRUCT_MOVEMENT_MAX_FORWARD_RUNNING_SPEED = 0xC70, // 2.4.3  Tested
		PLAYER_STRUCT_ROTATION = 0xBFC, // 2.4.3 Tested
		PLAYER_STRUCT_SIZE_WIDTH = 0xCA8,
		PLAYER_STRUCT_SIZE_HEIGHT = 0xCAC,
		PLAYER_STRUCT_SPELL_CASTING = 0xF40,
		PLAYER_STRUCT_MOVEMENT_STATE = 0xC20,
		PLAYER_STRUCT_CURRENT_TARGET = 0x2680 // 2.4.3 Tested
	};


	// WoW Enums
	/*enum eObjectFields
	{
		OBJECT_FIELD_GUID = 0x0,
		OBJECT_FIELD_TYPE = 0x8,
		OBJECT_FIELD_ENTRY = 0xC,
		OBJECT_FIELD_SCALE_X = 0x10,
		OBJECT_FIELD_PADDING = 0x14,
		TOTAL_OBJECT_FIELDS = 0x5
	};*/

	// WoW Enums
	enum eObjectFields
	{
		OBJECT_FIELD_VMT = 0x0,
		OBJECT_FIELD_ENTRY = 0x8,
		OBJECT_FIELD_TYPE = 0x14,
		OBJECT_FIELD_GUID = 0x30,
		OBJECT_FIELD_NEXT = 0x3C,
		OBJECT_FIELD_SCALE_X = 0x10,
		OBJECT_FIELD_PADDING = 0x14,
		OBJECT_FIELD_MODEL = 0xB4,
		OBJECT_FIELD_X = 0x798,
		OBJECT_FIELD_Y = 0x79C,
		OBJECT_FIELD_Z = 0x7A0,
		OBJECT_FIELD_PLAYER = 0x1198,
	};

	enum eItemFields
	{
		ITEM_FIELD_OWNER = 0x0,
		ITEM_FIELD_CONTAINED = 0x8,
		ITEM_FIELD_CREATOR = 0x10,
		ITEM_FIELD_GIFTCREATOR = 0x18,
		ITEM_FIELD_STACK_COUNT = 0x20,
		ITEM_FIELD_DURATION = 0x24,
		ITEM_FIELD_SPELL_CHARGES = 0x28,
		ITEM_FIELD_FLAGS = 0x3C,
		ITEM_FIELD_ENCHANTMENT = 0x40,
		ITEM_FIELD_PROPERTY_SEED = 0xC4,
		ITEM_FIELD_RANDOM_PROPERTIES_ID = 0xC8,
		ITEM_FIELD_ITEM_TEXT_ID = 0xCC,
		ITEM_FIELD_DURABILITY = 0xD0,
		ITEM_FIELD_MAXDURABILITY = 0xD4,
		TOTAL_ITEM_FIELDS = 0xE
	};

	enum eContainerFields
	{
		CONTAINER_FIELD_NUM_SLOTS = 0x0,
		CONTAINER_ALIGN_PAD = 0x4,
		CONTAINER_FIELD_SLOT_1 = 0x8,
		TOTAL_CONTAINER_FIELDS = 0x3
	};

	enum eUnitFlags
	{
		UNIT_FLAG_UNK_0 = 0x00000001,
		UNIT_FLAG_NON_ATTACKABLE = 0x00000002, // not attackable
		UNIT_FLAG_DISABLE_MOVE = 0x00000004,
		UNIT_FLAG_PVP_ATTACKABLE = 0x00000008, // allow apply pvp rules to attackable state in addition to faction dependent state
		UNIT_FLAG_RENAME = 0x00000010,
		UNIT_FLAG_PREPARATION = 0x00000020, // don't take reagents for spells with SPELL_ATTR_EX5_NO_REAGENT_WHILE_PREP
		UNIT_FLAG_UNK_6 = 0x00000040,
		UNIT_FLAG_NOT_ATTACKABLE_1 = 0x00000080, // ?? (UNIT_FLAG_PVP_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1) is NON_PVP_ATTACKABLE
		UNIT_FLAG_OOC_NOT_ATTACKABLE = 0x00000100, // 2.0.8 - (OOC Out Of Combat) Can not be attacked when not in combat. Removed if unit for some reason enter combat (flag probably removed for the attacked and it's party/group only)
		UNIT_FLAG_PASSIVE = 0x00000200, // makes you unable to attack everything. Almost identical to our "civilian"-term. Will ignore it's surroundings and not engage in combat unless "called upon" or engaged by another unit.
		UNIT_FLAG_LOOTING = 0x00000400, // loot animation
		UNIT_FLAG_PET_IN_COMBAT = 0x00000800, // in combat?, 2.0.8
		UNIT_FLAG_PVP = 0x00001000,
		UNIT_FLAG_SILENCED = 0x00002000, // silenced, 2.1.1
		UNIT_FLAG_UNK_14 = 0x00004000, // 2.0.8
		UNIT_FLAG_UNK_15 = 0x00008000,
		UNIT_FLAG_UNK_16 = 0x00010000, // removes attackable icon
		UNIT_FLAG_PACIFIED = 0x00020000,
		UNIT_FLAG_STUNNED = 0x00040000, // stunned, 2.1.1
		UNIT_FLAG_IN_COMBAT = 0x00080000,
		UNIT_FLAG_TAXI_FLIGHT = 0x00100000, // disable casting at client side spell not allowed by taxi flight (mounted?), probably used with 0x4 flag
		UNIT_FLAG_DISARMED = 0x00200000, // disable melee spells casting..., "Required melee weapon" added to melee spells tooltip.
		UNIT_FLAG_CONFUSED = 0x00400000,
		UNIT_FLAG_FLEEING = 0x00800000,
		UNIT_FLAG_PLAYER_CONTROLLED = 0x01000000, // used in spell Eyes of the Beast for pet... let attack by controlled creature
		UNIT_FLAG_NOT_SELECTABLE = 0x02000000,
		UNIT_FLAG_SKINNABLE = 0x04000000,
		UNIT_FLAG_MOUNT = 0x08000000,
		UNIT_FLAG_UNK_28 = 0x10000000,
		UNIT_FLAG_UNK_29 = 0x20000000, // used in Feing Death spell
		UNIT_FLAG_SHEATHE = 0x40000000
		// UNIT_FLAG_UNK_31 = 0x80000000 // no affect in 2.4.3
	};

	enum eUnitFields
	{
		UNIT_FIELD_CHARM = 0x18,
		UNIT_FIELD_SUMMON = 0x20,
		UNIT_FIELD_CRITTER = 0x28,
		UNIT_FIELD_CHARMEDBY = 0x30,
		UNIT_FIELD_SUMMONEDBY = 0x38,
		UNIT_FIELD_CREATEDBY = 0x40,
		UNIT_FIELD_TARGET = 0x48,
		UNIT_FIELD_CHANNEL_OBJECT = 0x50,
		UNIT_CHANNEL_SPELL = 0x58,
		UNIT_FIELD_BYTES_0 = 0x5C,
		UNIT_FIELD_HEALTH = 0x60,
		UNIT_FIELD_POWER1 = 0x64, // Weird
		UNIT_FIELD_POWER2 = 0x68, // Warrior Rage *10.
		UNIT_FIELD_POWER3 = 0x6C,
		UNIT_FIELD_POWER4 = 0x70, // Rogue Energy
		UNIT_FIELD_POWER5 = 0x74,
		UNIT_FIELD_POWER6 = 0x78,
		UNIT_FIELD_POWER7 = 0x7C, // Weird
		UNIT_FIELD_MAXHEALTH = 0x80,
		UNIT_FIELD_MAXPOWER1 = 0x84,
		UNIT_FIELD_MAXPOWER2 = 0x88,
		UNIT_FIELD_MAXPOWER3 = 0x8C,
		UNIT_FIELD_MAXPOWER4 = 0x90,
		UNIT_FIELD_MAXPOWER5 = 0x94,
		UNIT_FIELD_MAXPOWER6 = 0x98,
		UNIT_FIELD_MAXPOWER7 = 0x9C,
		UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER = 0xA0,
		UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER = 0xBC,
		UNIT_FIELD_LEVEL = 0xD8,
		UNIT_FIELD_FACTIONTEMPLATE = 0xDC,
		UNIT_VIRTUAL_ITEM_SLOT_ID = 0xE0,
		UNIT_FIELD_FLAGS = 0xEC,
		UNIT_FIELD_FLAGS_2 = 0xF0,
		UNIT_FIELD_AURASTATE = 0xF4,
		UNIT_FIELD_BASEATTACKTIME = 0xF8,
		UNIT_FIELD_RANGEDATTACKTIME = 0x100,
		UNIT_FIELD_BOUNDINGRADIUS = 0x104,
		UNIT_FIELD_COMBATREACH = 0x108,
		UNIT_FIELD_DISPLAYID = 0x10C,
		UNIT_FIELD_NATIVEDISPLAYID = 0x110,
		UNIT_FIELD_MOUNTDISPLAYID = 0x114,
		UNIT_FIELD_MINDAMAGE = 0x118,
		UNIT_FIELD_MAXDAMAGE = 0x11C,
		UNIT_FIELD_MINOFFHANDDAMAGE = 0x120,
		UNIT_FIELD_MAXOFFHANDDAMAGE = 0x124,
		UNIT_FIELD_BYTES_1 = 0x128,
		UNIT_FIELD_PETNUMBER = 0x12C,
		UNIT_FIELD_PET_NAME_TIMESTAMP = 0x130,
		UNIT_FIELD_PETEXPERIENCE = 0x134,
		UNIT_FIELD_PETNEXTLEVELEXP = 0x138,
		UNIT_DYNAMIC_FLAGS = 0x13C,
		UNIT_MOD_CAST_SPEED = 0x140,
		UNIT_CREATED_BY_SPELL = 0x144,
		UNIT_NPC_FLAGS = 0x148,
		UNIT_NPC_EMOTESTATE = 0x14C,
		UNIT_FIELD_STAT0 = 0x150,
		UNIT_FIELD_STAT1 = 0x154,
		UNIT_FIELD_STAT2 = 0x158,
		UNIT_FIELD_STAT3 = 0x15C,
		UNIT_FIELD_STAT4 = 0x160,
		UNIT_FIELD_POSSTAT0 = 0x164,
		UNIT_FIELD_POSSTAT1 = 0x168,
		UNIT_FIELD_POSSTAT2 = 0x16C,
		UNIT_FIELD_POSSTAT3 = 0x170,
		UNIT_FIELD_POSSTAT4 = 0x174,
		UNIT_FIELD_NEGSTAT0 = 0x178,
		UNIT_FIELD_NEGSTAT1 = 0x17C,
		UNIT_FIELD_NEGSTAT2 = 0x180,
		UNIT_FIELD_NEGSTAT3 = 0x184,
		UNIT_FIELD_NEGSTAT4 = 0x188,
		UNIT_FIELD_RESISTANCES = 0x18C,
		UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE = 0x1A8,
		UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE = 0x1C4,
		UNIT_FIELD_BASE_MANA = 0x1E0,
		UNIT_FIELD_BASE_HEALTH = 0x1E4,
		UNIT_FIELD_BYTES_2 = 0x1E8,
		UNIT_FIELD_ATTACK_POWER = 0x1EC,
		UNIT_FIELD_ATTACK_POWER_MODS = 0x1F0,
		UNIT_FIELD_ATTACK_POWER_MULTIPLIER = 0x1F4,
		UNIT_FIELD_RANGED_ATTACK_POWER = 0x1F8,
		UNIT_FIELD_RANGED_ATTACK_POWER_MODS = 0x1FC,
		UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = 0x200,
		UNIT_FIELD_MINRANGEDDAMAGE = 0x204,
		UNIT_FIELD_MAXRANGEDDAMAGE = 0x208,
		UNIT_FIELD_POWER_COST_MODIFIER = 0x20C,
		UNIT_FIELD_POWER_COST_MULTIPLIER = 0x228,
		UNIT_FIELD_MAXHEALTHMODIFIER = 0x244,
		UNIT_FIELD_HOVERHEIGHT = 0x248,
		UNIT_FIELD_PADDING = 0x24C,
		TOTAL_UNIT_FIELDS = 0x59
	};

	enum ePlayerFields
	{
		PLAYER_DUEL_ARBITER = 0x0,
		PLAYER_FLAGS = 0x8,
		PLAYER_GUILDID = 0xC,
		PLAYER_GUILDRANK = 0x10,
		PLAYER_BYTES = 0x14,
		PLAYER_BYTES_2 = 0x18,
		PLAYER_BYTES_3 = 0x1C,
		PLAYER_DUEL_TEAM = 0x20,
		PLAYER_GUILD_TIMESTAMP = 0x24,
		PLAYER_QUEST_LOG_1_1 = 0x28,
		PLAYER_QUEST_LOG_1_2 = 0x2C,
		PLAYER_QUEST_LOG_1_3 = 0x30,
		PLAYER_QUEST_LOG_1_4 = 0x34,
		PLAYER_QUEST_LOG_2_1 = 0x38,
		PLAYER_QUEST_LOG_2_2 = 0x3C,
		PLAYER_QUEST_LOG_2_3 = 0x40,
		PLAYER_QUEST_LOG_2_4 = 0x44,
		PLAYER_QUEST_LOG_3_1 = 0x48,
		PLAYER_QUEST_LOG_3_2 = 0x4C,
		PLAYER_QUEST_LOG_3_3 = 0x50,
		PLAYER_QUEST_LOG_3_4 = 0x54,
		PLAYER_QUEST_LOG_4_1 = 0x58,
		PLAYER_QUEST_LOG_4_2 = 0x5C,
		PLAYER_QUEST_LOG_4_3 = 0x60,
		PLAYER_QUEST_LOG_4_4 = 0x64,
		PLAYER_QUEST_LOG_5_1 = 0x68,
		PLAYER_QUEST_LOG_5_2 = 0x6C,
		PLAYER_QUEST_LOG_5_3 = 0x70,
		PLAYER_QUEST_LOG_5_4 = 0x74,
		PLAYER_QUEST_LOG_6_1 = 0x78,
		PLAYER_QUEST_LOG_6_2 = 0x7C,
		PLAYER_QUEST_LOG_6_3 = 0x80,
		PLAYER_QUEST_LOG_6_4 = 0x84,
		PLAYER_QUEST_LOG_7_1 = 0x88,
		PLAYER_QUEST_LOG_7_2 = 0x8C,
		PLAYER_QUEST_LOG_7_3 = 0x90,
		PLAYER_QUEST_LOG_7_4 = 0x94,
		PLAYER_QUEST_LOG_8_1 = 0x98,
		PLAYER_QUEST_LOG_8_2 = 0x9C,
		PLAYER_QUEST_LOG_8_3 = 0xA0,
		PLAYER_QUEST_LOG_8_4 = 0xA4,
		PLAYER_QUEST_LOG_9_1 = 0xA8,
		PLAYER_QUEST_LOG_9_2 = 0xAC,
		PLAYER_QUEST_LOG_9_3 = 0xB0,
		PLAYER_QUEST_LOG_9_4 = 0xB4,
		PLAYER_QUEST_LOG_10_1 = 0xB8,
		PLAYER_QUEST_LOG_10_2 = 0xBC,
		PLAYER_QUEST_LOG_10_3 = 0xC0,
		PLAYER_QUEST_LOG_10_4 = 0xC4,
		PLAYER_QUEST_LOG_11_1 = 0xC8,
		PLAYER_QUEST_LOG_11_2 = 0xCC,
		PLAYER_QUEST_LOG_11_3 = 0xD0,
		PLAYER_QUEST_LOG_11_4 = 0xD4,
		PLAYER_QUEST_LOG_12_1 = 0xD8,
		PLAYER_QUEST_LOG_12_2 = 0xDC,
		PLAYER_QUEST_LOG_12_3 = 0xE0,
		PLAYER_QUEST_LOG_12_4 = 0xE4,
		PLAYER_QUEST_LOG_13_1 = 0xE8,
		PLAYER_QUEST_LOG_13_2 = 0xEC,
		PLAYER_QUEST_LOG_13_3 = 0xF0,
		PLAYER_QUEST_LOG_13_4 = 0xF4,
		PLAYER_QUEST_LOG_14_1 = 0xF8,
		PLAYER_QUEST_LOG_14_2 = 0xFC,
		PLAYER_QUEST_LOG_14_3 = 0x100,
		PLAYER_QUEST_LOG_14_4 = 0x104,
		PLAYER_QUEST_LOG_15_1 = 0x108,
		PLAYER_QUEST_LOG_15_2 = 0x10C,
		PLAYER_QUEST_LOG_15_3 = 0x110,
		PLAYER_QUEST_LOG_15_4 = 0x114,
		PLAYER_QUEST_LOG_16_1 = 0x118,
		PLAYER_QUEST_LOG_16_2 = 0x11C,
		PLAYER_QUEST_LOG_16_3 = 0x120,
		PLAYER_QUEST_LOG_16_4 = 0x124,
		PLAYER_QUEST_LOG_17_1 = 0x128,
		PLAYER_QUEST_LOG_17_2 = 0x12C,
		PLAYER_QUEST_LOG_17_3 = 0x130,
		PLAYER_QUEST_LOG_17_4 = 0x134,
		PLAYER_QUEST_LOG_18_1 = 0x138,
		PLAYER_QUEST_LOG_18_2 = 0x13C,
		PLAYER_QUEST_LOG_18_3 = 0x140,
		PLAYER_QUEST_LOG_18_4 = 0x144,
		PLAYER_QUEST_LOG_19_1 = 0x148,
		PLAYER_QUEST_LOG_19_2 = 0x14C,
		PLAYER_QUEST_LOG_19_3 = 0x150,
		PLAYER_QUEST_LOG_19_4 = 0x154,
		PLAYER_QUEST_LOG_20_1 = 0x158,
		PLAYER_QUEST_LOG_20_2 = 0x15C,
		PLAYER_QUEST_LOG_20_3 = 0x160,
		PLAYER_QUEST_LOG_20_4 = 0x164,
		PLAYER_QUEST_LOG_21_1 = 0x168,
		PLAYER_QUEST_LOG_21_2 = 0x16C,
		PLAYER_QUEST_LOG_21_3 = 0x170,
		PLAYER_QUEST_LOG_21_4 = 0x174,
		PLAYER_QUEST_LOG_22_1 = 0x178,
		PLAYER_QUEST_LOG_22_2 = 0x17C,
		PLAYER_QUEST_LOG_22_3 = 0x180,
		PLAYER_QUEST_LOG_22_4 = 0x184,
		PLAYER_QUEST_LOG_23_1 = 0x188,
		PLAYER_QUEST_LOG_23_2 = 0x18C,
		PLAYER_QUEST_LOG_23_3 = 0x190,
		PLAYER_QUEST_LOG_23_4 = 0x194,
		PLAYER_QUEST_LOG_24_1 = 0x198,
		PLAYER_QUEST_LOG_24_2 = 0x19C,
		PLAYER_QUEST_LOG_24_3 = 0x1A0,
		PLAYER_QUEST_LOG_24_4 = 0x1A4,
		PLAYER_QUEST_LOG_25_1 = 0x1A8,
		PLAYER_QUEST_LOG_25_2 = 0x1AC,
		PLAYER_QUEST_LOG_25_3 = 0x1B0,
		PLAYER_QUEST_LOG_25_4 = 0x1B4,
		PLAYER_VISIBLE_ITEM_1_CREATOR = 0x1B8,
		PLAYER_VISIBLE_ITEM_1_0 = 0x1C0,
		PLAYER_VISIBLE_ITEM_1_PROPERTIES = 0x1F0,
		PLAYER_VISIBLE_ITEM_1_PAD = 0x1F4,
		PLAYER_VISIBLE_ITEM_2_CREATOR = 0x1F8,
		PLAYER_VISIBLE_ITEM_2_0 = 0x200,
		PLAYER_VISIBLE_ITEM_2_PROPERTIES = 0x230,
		PLAYER_VISIBLE_ITEM_2_PAD = 0x234,
		PLAYER_VISIBLE_ITEM_3_CREATOR = 0x238,
		PLAYER_VISIBLE_ITEM_3_0 = 0x240,
		PLAYER_VISIBLE_ITEM_3_PROPERTIES = 0x270,
		PLAYER_VISIBLE_ITEM_3_PAD = 0x274,
		PLAYER_VISIBLE_ITEM_4_CREATOR = 0x278,
		PLAYER_VISIBLE_ITEM_4_0 = 0x280,
		PLAYER_VISIBLE_ITEM_4_PROPERTIES = 0x2B0,
		PLAYER_VISIBLE_ITEM_4_PAD = 0x2B4,
		PLAYER_VISIBLE_ITEM_5_CREATOR = 0x2B8,
		PLAYER_VISIBLE_ITEM_5_0 = 0x2C0,
		PLAYER_VISIBLE_ITEM_5_PROPERTIES = 0x2F0,
		PLAYER_VISIBLE_ITEM_5_PAD = 0x2F4,
		PLAYER_VISIBLE_ITEM_6_CREATOR = 0x2F8,
		PLAYER_VISIBLE_ITEM_6_0 = 0x300,
		PLAYER_VISIBLE_ITEM_6_PROPERTIES = 0x330,
		PLAYER_VISIBLE_ITEM_6_PAD = 0x334,
		PLAYER_VISIBLE_ITEM_7_CREATOR = 0x338,
		PLAYER_VISIBLE_ITEM_7_0 = 0x340,
		PLAYER_VISIBLE_ITEM_7_PROPERTIES = 0x370,
		PLAYER_VISIBLE_ITEM_7_PAD = 0x374,
		PLAYER_VISIBLE_ITEM_8_CREATOR = 0x378,
		PLAYER_VISIBLE_ITEM_8_0 = 0x380,
		PLAYER_VISIBLE_ITEM_8_PROPERTIES = 0x3B0,
		PLAYER_VISIBLE_ITEM_8_PAD = 0x3B4,
		PLAYER_VISIBLE_ITEM_9_CREATOR = 0x3B8,
		PLAYER_VISIBLE_ITEM_9_0 = 0x3C0,
		PLAYER_VISIBLE_ITEM_9_PROPERTIES = 0x3F0,
		PLAYER_VISIBLE_ITEM_9_PAD = 0x3F4,
		PLAYER_VISIBLE_ITEM_10_CREATOR = 0x3F8,
		PLAYER_VISIBLE_ITEM_10_0 = 0x400,
		PLAYER_VISIBLE_ITEM_10_PROPERTIES = 0x430,
		PLAYER_VISIBLE_ITEM_10_PAD = 0x434,
		PLAYER_VISIBLE_ITEM_11_CREATOR = 0x438,
		PLAYER_VISIBLE_ITEM_11_0 = 0x440,
		PLAYER_VISIBLE_ITEM_11_PROPERTIES = 0x470,
		PLAYER_VISIBLE_ITEM_11_PAD = 0x474,
		PLAYER_VISIBLE_ITEM_12_CREATOR = 0x478,
		PLAYER_VISIBLE_ITEM_12_0 = 0x480,
		PLAYER_VISIBLE_ITEM_12_PROPERTIES = 0x4B0,
		PLAYER_VISIBLE_ITEM_12_PAD = 0x4B4,
		PLAYER_VISIBLE_ITEM_13_CREATOR = 0x4B8,
		PLAYER_VISIBLE_ITEM_13_0 = 0x4C0,
		PLAYER_VISIBLE_ITEM_13_PROPERTIES = 0x4F0,
		PLAYER_VISIBLE_ITEM_13_PAD = 0x4F4,
		PLAYER_VISIBLE_ITEM_14_CREATOR = 0x4F8,
		PLAYER_VISIBLE_ITEM_14_0 = 0x500,
		PLAYER_VISIBLE_ITEM_14_PROPERTIES = 0x530,
		PLAYER_VISIBLE_ITEM_14_PAD = 0x534,
		PLAYER_VISIBLE_ITEM_15_CREATOR = 0x538,
		PLAYER_VISIBLE_ITEM_15_0 = 0x540,
		PLAYER_VISIBLE_ITEM_15_PROPERTIES = 0x570,
		PLAYER_VISIBLE_ITEM_15_PAD = 0x574,
		PLAYER_VISIBLE_ITEM_16_CREATOR = 0x578,
		PLAYER_VISIBLE_ITEM_16_0 = 0x580,
		PLAYER_VISIBLE_ITEM_16_PROPERTIES = 0x5B0,
		PLAYER_VISIBLE_ITEM_16_PAD = 0x5B4,
		PLAYER_VISIBLE_ITEM_17_CREATOR = 0x5B8,
		PLAYER_VISIBLE_ITEM_17_0 = 0x5C0,
		PLAYER_VISIBLE_ITEM_17_PROPERTIES = 0x5F0,
		PLAYER_VISIBLE_ITEM_17_PAD = 0x5F4,
		PLAYER_VISIBLE_ITEM_18_CREATOR = 0x5F8,
		PLAYER_VISIBLE_ITEM_18_0 = 0x600,
		PLAYER_VISIBLE_ITEM_18_PROPERTIES = 0x630,
		PLAYER_VISIBLE_ITEM_18_PAD = 0x634,
		PLAYER_VISIBLE_ITEM_19_CREATOR = 0x638,
		PLAYER_VISIBLE_ITEM_19_0 = 0x640,
		PLAYER_VISIBLE_ITEM_19_PROPERTIES = 0x670,
		PLAYER_VISIBLE_ITEM_19_PAD = 0x674,
		PLAYER_CHOSEN_TITLE = 0x678,
		PLAYER_FIELD_PAD_0 = 0x67C,
		PLAYER_FIELD_INV_SLOT_HEAD = 0x680,
		PLAYER_FIELD_PACK_SLOT_1 = 0x738,
		PLAYER_FIELD_BANK_SLOT_1 = 0x7B8,
		PLAYER_FIELD_BANKBAG_SLOT_1 = 0x898,
		PLAYER_FIELD_VENDORBUYBACK_SLOT_1 = 0x8D0,
		PLAYER_FIELD_KEYRING_SLOT_1 = 0x930,
		PLAYER_FIELD_VANITYPET_SLOT_1 = 0xA30,
		PLAYER_FARSIGHT = 0xAC0,
		PLAYER_FIELD_KNOWN_TITLES = 0xAC8,
		PLAYER_XP = 0xAD0,
		PLAYER_NEXT_LEVEL_XP = 0xAD4,
		PLAYER_SKILL_INFO_1_1 = 0xAD8,

		PLAYER_CHARACTER_POINTS1 = 0x10D8,
		PLAYER_CHARACTER_POINTS2 = 0x10DC,
		// PLAYER_TRACK_CREATURES=0x10E0,
		PLAYER_TRACK_CREATURES = 0x0FF8, // 3.3.5a 12340 
		PLAYER_TRACK_RESOURCES = 0x10E4,
		PLAYER_BLOCK_PERCENTAGE = 0x10E8,
		PLAYER_DODGE_PERCENTAGE = 0x10EC,
		PLAYER_PARRY_PERCENTAGE = 0x10F0,
		PLAYER_EXPERTISE = 0x10F4,
		PLAYER_OFFHAND_EXPERTISE = 0x10F8,
		PLAYER_CRIT_PERCENTAGE = 0x10FC,
		PLAYER_RANGED_CRIT_PERCENTAGE = 0x1100,
		PLAYER_OFFHAND_CRIT_PERCENTAGE = 0x1104,
		PLAYER_SPELL_CRIT_PERCENTAGE1 = 0x1108,
		PLAYER_SHIELD_BLOCK = 0x1124,
		PLAYER_EXPLORED_ZONES_1 = 0x1128,
		PLAYER_REST_STATE_EXPERIENCE = 0x1328,
		PLAYER_FIELD_COINAGE = 0x132C,
		PLAYER_FIELD_MOD_DAMAGE_DONE_POS = 0x1330,
		PLAYER_FIELD_MOD_DAMAGE_DONE_NEG = 0x134C,
		PLAYER_FIELD_MOD_DAMAGE_DONE_PCT = 0x1368,
		PLAYER_FIELD_MOD_HEALING_DONE_POS = 0x1384,
		PLAYER_FIELD_MOD_TARGET_RESISTANCE = 0x1388,
		PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE = 0x138C,
		PLAYER_FIELD_BYTES = 0x1390,
		PLAYER_AMMO_ID = 0x1394,
		PLAYER_SELF_RES_SPELL = 0x1398,
		PLAYER_FIELD_PVP_MEDALS = 0x139C,
		PLAYER_FIELD_BUYBACK_PRICE_1 = 0x13A0,
		PLAYER_FIELD_BUYBACK_TIMESTAMP_1 = 0x13D0,
		PLAYER_FIELD_KILLS = 0x1400,
		PLAYER_FIELD_TODAY_CONTRIBUTION = 0x1404,
		PLAYER_FIELD_YESTERDAY_CONTRIBUTION = 0x1408,
		PLAYER_FIELD_LIFETIME_HONORBALE_KILLS = 0x140C,
		PLAYER_FIELD_BYTES2 = 0x1410,
		PLAYER_FIELD_WATCHED_FACTION_INDEX = 0x1414,
		PLAYER_FIELD_COMBAT_RATING_1 = 0x1418,
		PLAYER_FIELD_ARENA_TEAM_INFO_1_1 = 0x1478,
		PLAYER_FIELD_HONOR_CURRENCY = 0x14C0,
		PLAYER_FIELD_ARENA_CURRENCY = 0x14C4,
		PLAYER_FIELD_MOD_MANA_REGEN = 0x14C8,
		PLAYER_FIELD_MOD_MANA_REGEN_INTERRUPT = 0x14CC,
		PLAYER_FIELD_MAX_LEVEL = 0x14D0,
		PLAYER_FIELD_DAILY_QUESTS_1 = 0x14D4,
		TOTAL_PLAYER_FIELDS = 0xF2
	};

	enum eGameObjectFields
	{
		OBJECT_FIELD_CREATED_BY = 0x18,
		GAMEOBJECT_DISPLAYID = 0x20,
		GAMEOBJECT_FLAGS = 0x24,
		GAMEOBJECT_PARENTROTATION = 0x28,
		GAMEOBJECT_DYNAMIC = 0x38,
		GAMEOBJECT_FACTION = 0x3C,
		GAMEOBJECT_LEVEL = 0x40,
		GAMEOBJECT_OTHER_FLAGS = 0x44,
		TOTAL_GAMEOBJECT_FIELDS = 0x8
	}; // 0xD4

	enum eDynamicObjectFields
	{
		DYNAMICOBJECT_CASTER = 0x0,
		DYNAMICOBJECT_BYTES = 0x8,
		DYNAMICOBJECT_SPELLID = 0xC,
		DYNAMICOBJECT_RADIUS = 0x10,
		DYNAMICOBJECT_POS_X = 0x14,
		DYNAMICOBJECT_POS_Y = 0x18,
		DYNAMICOBJECT_POS_Z = 0x1C,
		DYNAMICOBJECT_FACING = 0x20,
		DYNAMICOBJECT_CASTTIME = 0x24,
		TOTAL_DYNAMICOBJECT_FIELDS = 0x9
	};

	enum eCorpseFields
	{
		CORPSE_FIELD_OWNER = 0x0,
		CORPSE_FIELD_PARTY = 0x8,
		CORPSE_FIELD_FACING = 0x10,
		CORPSE_FIELD_POS_X = 0x14,
		CORPSE_FIELD_POS_Y = 0x18,
		CORPSE_FIELD_POS_Z = 0x1C,
		CORPSE_FIELD_DISPLAY_ID = 0x20,
		CORPSE_FIELD_ITEM = 0x24,
		CORPSE_FIELD_BYTES_1 = 0x70,
		CORPSE_FIELD_BYTES_2 = 0x74,
		CORPSE_FIELD_GUILD = 0x78,
		CORPSE_FIELD_FLAGS = 0x7C,
		CORPSE_FIELD_DYNAMIC_FLAGS = 0x80,
		CORPSE_FIELD_PAD = 0x84,
		TOTAL_CORPSE_FIELDS = 0xE
	};
}
