#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include "..\Main\Structures.h"

enum eToggleFlags
{
	TOGGLE_PARSE  = 0x01, // hex for 0000 0001
	TOGGLE_NEWMAP = 0x02, // hex for 0000 0010
	TOGGLE_CHAT   = 0x04, // hex for 0000 0100
	option4		  = 0x08, // hex for 0000 1000
};

namespace DinnerChat
{
	// Globals
	extern std::string frameName;
	extern std::string lastWhisperName;
	extern std::vector<std::string> grandmothers;

	// Functions
	VOID PlayerCount();
	BOOL SendWhisper(std::string message, std::string name);
	BOOL SendCommand(CONST CHAR *message, CONST CHAR *target);
	BOOL Ping(std::string playerName);

	// Bitmask to communicate with the ingame DinnerChat.
	BYTE GetMask();
	BOOL FlagOn(DWORD flag);
	BOOL FlagOff(DWORD flag);
	BOOL SetMask(BYTE toggle);
	BOOL MaskHasFlag(DWORD flag);
	
	// Construction
	VOID Setup();
	BOOL Destroy();
	BOOL SendScripts();
};

struct WoWChatMessage
{
	WGUID	guid;
    int		unknown0[2];
    char	senderName[48];
    char	formattedMsg[3000];
    char	plaintextMsg[3000];
    int		type;
    int		channelNr;
    int		sequence;
    int		time;
};

enum Chat_Type
{
	SystemMessage = 0x0,
	SayMessage = 0x1,
	PartyMessage = 0x2,
	RaidMessage = 0x3,
	GuildMessage = 0x4,
	OfficerMessage = 0x5,
	YellMessage = 0x6, 
	FromWhisper = 0x7,
	FromWhisper2 = 0x8,
	ToWhisper = 0x9,
	EmoteMessage = 0xA,
	NPCMessage = 0xC,
	PartyMessage2 = 0xD,
	YellMessage2 = 0xE, 
	WeirdWhisper = 0xF,
	EmoteMessage2 = 0x10,
	ChannelMessage = 0x11,
	DarkBrownMessage = 0x14,
	LightPurpleMessage = 0x23,
	TurqoiseMessage = 0x25,
	RedWarningMessage = 0x26,
	RaidWarningMessage = 0x28,
	GreenMessage = 0x31
};

unsigned int GetLastChatType();
unsigned int GetMessageType(DWORD message);
unsigned int GetChatMessage(DWORD message, char *out, size_t maxOut, char *sender, size_t maxSender);

DWORD GetCurrentChatMessagePointer();
DWORD GetNextChatMessagePointer(DWORD current);
DWORD GetPreviousChatMessagePointer(DWORD current);
DWORD ChatBubbleShow(DWORD base, unsigned int type, char *text);
DWORD GetLastChatMessage(char *out, size_t maxOut, char *sender, size_t maxSender);

VOID UpdateGuildChat(char *message, char *name);

BOOL FakeTargetMessage(const char *message);
BOOL AddChatMessage(CHAR *szMessage, BYTE Type);
BOOL WoWConsoleWrite(CHAR *szMessage, BYTE Colour);
BOOL AddChatMessage(CHAR *szMessage, CHAR *szModule, BYTE Type);
BOOL FakeMessageByName(const char *message, const char *objectName);
