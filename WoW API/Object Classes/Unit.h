#pragma once

#include <Windows.h>
#include <string>
#include "Object.h"
#include "GameObject.h"

#include <WoW API\Spell.h>

enum ePowerType
{
	POWER_TYPE_MANA		= 0,
	POWER_TYPE_RAGE		= 1,
	POWER_TYPE_ENERGY	= 3,
	POWER_TYPE_RUNE		= 6,
};

class CUnit: public CObject
{
private:
	void CUnit::_readName();

public:
	CUnit(Object obj) : CObject(obj) { this->update(UnitFieldInfo); }
	CUnit()			  : CObject() { }
	~CUnit() {}
	
	// Stat functions
	DWORD baseField()		{ return this->object().UnitField.BaseAddress;			}
	virtual DWORD flags()	{ return this->object().UnitField.Unit_Flags;			}
	DWORD dynamicFlags()	{ return this->object().UnitField.Unit_Dynamic_Flags;	}
	DWORD channelSpell()	{ return this->object().UnitField.ChannelSpell;			}
	WGUID channelObject()	{ return this->object().UnitField.ChannelObject;		}
	DWORD displayId()		{ return this->object().UnitField.DisplayId;			}
	DWORD faction()			{ return this->object().UnitField.Faction;				}
	DWORD movementState()	{ return this->object().UnitField.MovementState;		}
	DWORD unitFlags()		{ return this->object().UnitField.Unit_Flags;			}
	WGUID createdBy()		{ return this->object().UnitField.CreatedBy;			}
	WGUID summonedBy()		{ return this->object().UnitField.SummonedBy;			}

	uint power()			{ return this->object().UnitField.Power;				}
	uint maxPower()			{ return this->object().UnitField.Max_Power;			}
	uint health()			{ return this->object().UnitField.Health;				}
	uint maxHealth()		{ return this->object().UnitField.Max_Health;			}
	uint level()			{ return this->object().UnitField.Level;				}
	uint attackPower()		{ return this->object().UnitField.Attack_Power;			}
	byte powerType()		{ return this->object().UnitField.PowerType;			}

	FLOAT currentSpeed()	{ return this->object().UnitField.CurrentSpeed;			}
	FLOAT activeSpeed()		{ return this->object().UnitField.ActiveSpeed;			}
	FLOAT walkSpeed()		{ return this->object().UnitField.WalkSpeed;			}
	FLOAT swimSpeed()		{ return this->object().UnitField.SwimSpeed;			}
	FLOAT flightSpeed()		{ return this->object().UnitField.FlightSpeed;			}

	FLOAT minDamage()		{ return this->object().UnitField.Min_Damage;			}
	FLOAT maxDamage()		{ return this->object().UnitField.Max_Damage;			}
	FLOAT combatReach()		{ return this->object().UnitField.CombatReach;			}
	FLOAT boundingRadius()	{ return this->object().UnitField.Bounding_Radius;		}
	
	// Update functions.
	CUnit getTarget();

	DWORD updateHealth();
	DWORD updateFaction();
	DWORD updateFieldBase();
	DWORD updateMovementPtr();


	void setDisplayId(uint id);
	void setMountDisplayId(uint id);

	void UpdateDisplayInfo();
	void CUnit::UpdateMountDisplayInfo();
	std::string getPowerString();

	// Target Functions
	bool hasTarget(WGUID t);
	bool hasTarget(Object const &t);
	bool hasTarget(CObject &t);

	// Aura Functions
	bool hasAura(UINT spellId);
	bool hasAura(std::string spellname);
	bool hasAura(CSpell &spell);

	// Synonyms
	bool hasBuff(UINT spellId);
	bool hasBuff(std::string spellName);
	bool hasBuff(CSpell &spell);

	uint auraCount();
	uint auraSpellId(UINT aura);

	// 
	bool owns(CUnit *);
	bool owns(CGameObject *);
	bool owns(CGameObject &o);

	// Status functions
	bool isDead();
	bool isAlive();
	bool inCombat();

	bool isFlying();
	bool isMounted();

	bool isFishing();
	bool isCasting();
	bool isStealthed();
	bool isChanneling();

	bool hasNoTarget();
	bool hasLoot();

	// Faction Functions
	bool isFriendly(CUnit *other);
	int  getFactionGroup();

	virtual bool isType() { return this->isUnit() || this->isPlayer(); }
};

struct LessThanByLocalDistance
{
	bool operator() (CUnit &lhs, CUnit &rhs)
	{
		return lhs.distance() < rhs.distance();
	}
};

/*
007283A0    CGUnit_C__AffectedByAura
007385C0    CGUnit_C__AnimationData
007293D0    CGUnit_C__CanAssist
00729A70    CGUnit_C__CanAttack
00721F50    CGUnit_C__CanAutoInteract
007192C0    CGUnit_C__CanGossip
0073F660    CGUnit_C__CGUnit_C
00721CA0    CGUnit_C__CurrentShapeshiftForm_HasFlag_0x1
00734FD0    CGUnit_C__Disable
0072A480    CGUnit_C__DisplayInfoNeedsUpdate
00715E30    CGUnit_C__EntryHasFlag_0x80
00715F70    CGUnit_C__EntryHasFlag_0x800
006DE230    CGUnit_C__EquippedItemMeetSpellRequirements
00714AC0    CGUnit_C__Field_A30_Has_0x400
00556E10    CGUnit_C__GetAura
00723490    CGUnit_C__GetAuraCaster
004F8850    CGUnit_C__GetAuraCount
00565510    CGUnit_C__GetAuraFlags
004F8870    CGUnit_C__GetAuraSpellId
0071B770    CGUnit_C__GetBasePowerForType
00718B70    CGUnit_C__GetControllingPlayer
00717A20    CGUnit_C__GetCreatureModelDataRec
00718A00    CGUnit_C__GetCreatureRank
0071F300    CGUnit_C__GetCreatureType
004F61D0    CGUnit_C__GetDistanceToPos
006E6F60    CGUnit_C__GetFacing
00716470    CGUnit_C__GetGroundNormal
0071BA60    CGUnit_C__GetHealthRegenRateFromSpirit
0071B9F0    CGUnit_C__GetManaRegenRateFromSpirit
00717B20    CGUnit_C__GetModel
006E6EE0    CGUnit_C__GetObjectName
00754EF0    CGUnit_C__GetParryingItem
006E6F10    CGUnit_C__GetPosition
0071B960    CGUnit_C__GetPowerModifier
00744640    CGUnit_C__GetQuestInteractType
0071AF70    CGUnit_C__GetShapeshiftFormId
0071B8D0    CGUnit_C__GetStatsField_34
00734790    CGUnit_C__GetThreatEntry
00719950    CGUnit_C__GetTitleString
006E6F70    CGUnit_C__GetTransportGuid
0056C220    CGUnit_C__GetUnitF58Field_14_4F03C0
0072A000    CGUnit_C__GetUnitName
004F6210    CGUnit_C__GetVehicleRecPtr
005D3340    CGUnit_C__GetVehicleSeatRec
0071FD80    CGUnit_C__GetWorldMatrix
0071AF90    CGUnit_C__GuidA20_Is_NonZero
007282A0    CGUnit_C__HasAuraBySpellId
00718A90    CGUnit_C__HasEntryFlag_0x400_NotAPlayer
0074B9A0    CGUnit_C__HasMovementFlags2_0x1
005D3310    CGUnit_C__HasNotUnitFlags_18_22_23
00722180    CGUnit_C__HasVehicleOr_EntryFlag_0x8
00742220    CGUnit_C__Initialize
004D43C0    CGUnit_C__IsActiveMover
0071EF20    CGUnit_C__IsAllowedToSendMessage
0051A230    CGUnit_C__IsFriendlyTowards
00514080    CGUnit_C__IsHatedTowards
0071BCE0    CGUnit_C__IsInFrontOfTargetWithGuid
00718D70    CGUnit_C__IsInMyParty
00718CA0    CGUnit_C__IsInMyPartyOrRaid
0071B680    CGUnit_C__IsNotInPvP_IsNotLeader
0052C8C0    CGUnit_C__IsPartyMember
00573200    CGUnit_C__IsRaidMember
0071C260    CGUnit_C__ModPredictedHealth
007474B0    CGUnit_C__OnAiReaction
00726280    CGUnit_C__OnChannelUpdate
006F0DD0    CGUnit_C__OnJump
0072D1B0    CGUnit_C__OnKnockBackPacket
0073C8E0    CGUnit_C__OnMonsterMovePacket
00740D30    CGUnit_C__OnMovementPacket
007307A0    CGUnit_C__OnMoveModePacket
006F0CF0    CGUnit_C__OnMoveStart
006F0EB0    CGUnit_C__OnMoveStop
0072D2D0    CGUnit_C__OnMoveTeleportAck
007188F0    CGUnit_C__OnMoveUnRoot
007188C0    CGUnit_C__OnPitchStart
006EED10    CGUnit_C__OnPitchStop
00731260    CGUnit_C__OnRightClick
006F10D0    CGUnit_C__OnSetRunMode
007406A0    CGUnit_C__OnSetSpeedPacket
00740BA0    CGUnit_C__OnSplineMoveEffectPacket
00740A60    CGUnit_C__OnSplineSpeedChange
006F0D60    CGUnit_C__OnStrafeStart
006F0F10    CGUnit_C__OnStrafeStop
006F1120    CGUnit_C__OnTeleport
00718890    CGUnit_C__OnTurnStart
006F1080    CGUnit_C__OnTurnStop
0071A260    CGUnit_C__PlayEmote
0073AB90    CGUnit_C__PlayEmoteAnimation
007207C0    CGUnit_C__PlayEmotes
0073FCC0    CGUnit_C__PostInit
00724EA0    CGUnit_C__PostShutdown
00725F50    CGUnit_C__QueryModelStats
007237F0    CGUnit_C__Reenable
0072D940    CGUnit_C__RefreshDataPointers
00716F10    CGUnit_C__RequestMirrorImageData
0071F210    CGUnit_C__Send_CMSG_MOVE_SPLINE_DONE
007413F0    CGUnit_C__SendMovementPacket
00721B90    CGUnit_C__SendMovementPackets_954_958
00717D90    CGUnit_C__SendMoveTimeSkipped
0073AF00    CGUnit_C__SetAnimType
00715A80    CGUnit_C__SetCurrentSpellAndCastId
00715400    CGUnit_C__SetCurrentSpellDestLocation
00746540    CGUnit_C__SetImpactKitEffect
00716050    CGUnit_C__SetPredictedHealth
00722C50    CGUnit_C__SetPredictedPower
00742BB0    CGUnit_C__Shutdown
00715AC0    CGUnit_C__ToggleFieldB70_Flag_0x200
0074B9B0    CGUnit_C__ToggleMovementFlag2_0x40
0071A360    CGUnit_C__TryChangeStandState
0073D4A0    CGUnit_C__TrySendMSG_MOVE_FALL_LAND
006D1DE0    CGUnit_C__UnitCanGossip
00716710    CGUnit_C__UnitOrPossessorDoesNotHaveUnitFlag_0x1
007251C0    CGUnit_C__UnitReaction
006E6F40    CGUnit_C__UpdateFacing
006E6EF0    CGUnit_C__UpdatePosition
00716FD0    CGUnit_C__Uses_A30_Flag_0x40000000
0074C7F0    CGUnit_C__VehicleExit
0074C8B0    CGUnit_C__VehicleNextSeat
0074C9A0    CGUnit_C__VehiclePrevSeat
0071FEF0    CGUnit_C__virt20
00719310    CGUnit_C__virt54
007193C0    CGUnit_C__virt58
0073DAB0    CGUnit_C__virt68
00730F30    CGUnit_C__virt70
0071C0E0    CGUnit_C__virt78_0
006E6ED0    CGUnit_C__virtAC
006E6F80    CGUnit_C__virtBC
0074B830    CGUnit_C__virtC0
00715B50    CGUnit_C__virtC8
0074B810    CGUnit_C__virtD4
0073B140    CGUnit_C__virtE8
0073A6C0    CGUnit_C__virtEC
0072AF60    CGUnit_C__virtF0
0071F0C0    CGUnit_C__WriteMovementPacketWithTransport
007374C0    CGUnit_C_CalculateThreat
00737620    CGUnit_C_GetThreatEntry
00737BA0    CGUnit_C_virt00
007402B0    CGUnit_C_virt0C
007370D0    CGUnit_C_virt10
0071F440    CGUnit_C_virt108
0071AA70    CGUnit_C_virt118
0071AD20    CGUnit_C_virt124
00729F40    CGUnit_C_virt38
007192E0    CGUnit_C_virt3C
00724DF0    CGUnit_C_virt48
00724E20    CGUnit_C_virt4C
00725980    CGUnit_C_virt50
00725BF0    CGUnit_C_virt54
0072B7E0    CGUnit_C_virt58
00718AC0    CGUnit_C_virt5C
0073E840    CGUnit_C_virt64
007156A0    CGUnit_C_virt74
0073AD60    CGUnit_C_virt78
0071A390    CGUnit_C_virt84
0073C1D0    CGUnit_C_virt98
00720F80    CGUnit_C_virt9C
00722760    CGUnit_C_virtA0
00722B50    CGUnit_C_virtA4
00716650    CGUnit_C_virtA8
00715560    CGUnit_C_virtB0
006E6FA0    CGUnit_C_virtB4
00729C70    CGUnit_C_virtB8
008A1420    CGUnit_C_virtCC
006F1700    CGUnit_C_virtE4
00747310    CGUnit_C_virtF4
007464D0    CGUnit_C_virtF8
007463E0    CGUnit_C_virtFC
*/

UINT GetAuraCount(Object Unit);

bool ChangeStandState(DWORD baseAddress);
bool GetAuraName(UINT nAuraID, CHAR *szAuraName);
bool HasAuraBySpellId(Object Unit, INT nSpellId);
