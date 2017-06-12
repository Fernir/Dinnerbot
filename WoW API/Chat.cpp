#include "Chat.h"
#include "Lua.h"

#include "Game\Game.h"
#include "Utilities\Thread.h"
#include "Utilities\Utilities.h"

#include "Memory\Endscene.h"
#include "Memory\Memory.h"

#include "Dialog\Resource.h"
#include "Dialog\Dialog.h"

#include "Main\Dinnerbot.h"
#include <Main\Command.h>

#include <mutex>

using namespace std;
using namespace Memory;
using namespace Offsets;

std::string DinnerChat::frameName = "DinnerFrame";
std::string DinnerChat::lastWhisperName = "";

std::mutex setupLock;

std::vector<std::string> DinnerChat::grandmothers = { "Or", "Horsebreed", "Shrek", "Luckypunk", "Senpai", "Priestiality", "Lumipants", "Lumiere", "Algebra", "Act", "Melancholy", "Adidi", "Lumzie", "Lumpypunk", "Lumz", "Horyshet", "Unhoryshet", };

typedef unsigned int uint;

BOOL AddChatMessage(CHAR *szMessage, BYTE Type)
{
	return AddChatMessage(szMessage, "D", Type);
}

// Guild: 4

/*
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
	DarkBrownMessage = 0x14,
	LightPurpleMessage = 0x23,
	TurqoiseMessage = 0x25,
	RedWarningMessage = 0x26,
	RaidWarningMessage = 0x28,
	GreenMessage = 0x31*/


/******** DinnerChat namespace start ********/
VOID DinnerChat::PlayerCount()
{
	DWORD check = NULL;
	
	if (!Endscene.IsHooked()) return;
	if (!Endscene.CanExecute()) return;
	check = GetMagicCheck(IDC_MAIN_TOOLS_CHECK_INGAMEPLAYERCOUNT);

	if (check)
	{
		// It doesn't matter very much if ES times out here, so make it nonblocking.
		Endscene.setWait(0);
		Lua::vDo("%s:Show() DinnerDank.players = %d", frameName.c_str(), PlayersInArea());

		// Reset the timeout.
		Endscene.setWait(1000);
	}
}

BOOL DinnerChat::SetMask(BYTE mask)
{
	BYTE buffer = mask;
	return wpm(Offsets::consoleKey, &buffer, sizeof(BYTE));
}

BOOL DinnerChat::FlagOff(DWORD flag)
{
	BYTE mask = DinnerChat::GetMask();

	mask &= ~flag;
	return DinnerChat::SetMask(mask);
}

BOOL DinnerChat::FlagOn(DWORD flag)
{
	BYTE mask = DinnerChat::GetMask();

	mask |= flag;
	return DinnerChat::SetMask(mask);
}

BYTE DinnerChat::GetMask()
{
	BYTE buffer;

	rpm(Offsets::consoleKey, &buffer, sizeof(BYTE));
	return buffer;
}

BOOL DinnerChat::MaskHasFlag(DWORD flag)
{
	BYTE mask = DinnerChat::GetMask();

	return (mask & flag) == flag;
}

// I kept hearing her voice when I was tired
VOID DinnerChat::Setup()
{
	if (!Endscene.Start()) 
	{
		velog("Unable to hook endscene (ptr: 0x%x)", CEndscene::Get());
		Thread::DeleteCurrent("DinnerChat::Setup");
		return;
	}

	if (Endscene.CanExecute() /*setupLock.try_lock()*/)
	{
		// Delete any previous instances of DinnerChat.
		DinnerChat::Destroy();

		// Setup DinnerChat's frames and functions.
		DinnerChat::SendScripts();

		// If DinnerChat was set up properly, it will have set this flag
		if (DinnerChat::MaskHasFlag(TOGGLE_CHAT))
		{
			Lua::vDo("print(\"|cFFFF00FFDinner|cFF0000FFLua |cFF00FF00%s|cFFFF0000 loaded.\")", Dinner::versionName.c_str());
			vlog("DinnerChat loaded");
		}
		else
			velog("Chat flag (0x%x) not set", DinnerChat::GetMask());

		setupLock.unlock();
	}
	else
		velog("Unable to execute on endscene");

	//setupLock.unlock();
	Thread::DeleteCurrent("DinnerChat::Setup");
}

// Send DinnerChat Lua to WoW.
// Orange House
BOOL DinnerChat::SendScripts()
{
	CHAR script[10000];
	
	Lua::vDo(
		"if %s ~= nil then %s:Hide() end "
		"DinnerDank = {} ", frameName.c_str(), frameName.c_str()
	);

	// Create UI for dinner. 
	sprintf_s(script, 
		// Initialize DinnerDank namespace and register our CVar for communcation between WoW and Dinnerbot.

		"RegisterCVar(\"DE\") "
		"SetCVar(\"DE\", \"\") "
		"DinnerDank.players = 0 "

		// 
		"%s = CreateFrame(\"frame\", \"DinnerFrame\") "
		"%s:SetParent(UIParent) "
		"%stext = %s:CreateFontString(nil, \"OVERLAY\", \"GameFontNormal\") "
		"%stext:SetPoint(\"CENTER\") "

		// On player frame update count
		"function DinnerFrameUpdate() "
			"%stext:SetText(\"Players in area: \" .. DinnerDank.players) "
		"end "

		// Set the frame's dimensions and behaviours.
		"%s:SetWidth(120) "
		"%s:SetHeight(42) "
		"%s:SetPoint(\"TOP\", UIParent) "
		"%s:EnableMouse(true) "
		"%s:SetMovable(true) "
		"%s:RegisterForDrag(\"LeftButton\") "
		"%s:SetScript(\"OnDragStart\", "
			"function(self) "
				"self:StartMoving() "
			"end"
		") "

		// Allow the frame to be dragged.
		"%s:SetScript(\"OnDragStart\", "
			"function(self) "
				"self:StartMoving() "
			"end"
		") "

	"%s:SetScript(\"OnDragStop\", "
		"function(self) "
			"self:StopMovingOrSizing() "
		"end"
	") "

	"%s:SetScript(\"OnUpdate\", DinnerFrameUpdate) "
	"%s:Hide()", 
	frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), 
	frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str(), frameName.c_str());
	Lua::DoString(script);

	// Register events and event handlers.
	sprintf_s(script, 
	"%s:RegisterEvent(\"CHAT_MSG_ADDON\") "

	"function DinnerDank.Version(target) "
		"SendAddonMessage(\"DVersion_Req\", \"\", \"WHISPER\", target) "
	"end "

	"function DinnerDank.Print(text) "
		"DEFAULT_CHAT_FRAME:AddMessage(text, 255, 0, 0, 53, 5)  "
	"end "

	"function DinnerDank.Ping(target) "
		"DinnerDank.Print(\"[DD]: DPing sent to \" .. target) "
		"SendAddonMessage(\"DPing\", GetUnitName(\"player\"), \"WHISPER\", target) "
	"end "
	
	"function DinnerDank.Pong(target) "
		"SendAddonMessage(\"DPong\", GetUnitName(\"player\"), \"WHISPER\", target) "
	"end "
	
	"function DinnerDank.Check() "
		"if %s:IsEventRegistered(\"CHAT_MSG_ADDON\") then "
			"print(\"[DD]: Dinner is currently being served.\") "
		"end "
	"end "

	"function DinnerDank.Send(command, target) "
		"SendAddonMessage(\'DD\', command, \'WHISPER\', target) "
	"end "

	"function DinnerDank.GetRuntime(target) "
		"DinnerDank.Send(\"dgrstring = DinnerDank.GetRuntimeString()\", target) " 
		"return DinnerDank.GetReturn(\"dgrstring\", target)"
	"end "

	"function DinnerDank.GetReturn(variable, target) "
		"DSend([[DSend(\'print(\"\' .. ]] .. variable .. [[ .. \'\")\', \']] .. UnitName(\"player\") .. [[\')]], target) "
	"end "

	"function DinnerDank.Whisper(message, target) "
		"if target == nil then return end "
		"DEFAULT_CHAT_FRAME:AddMessage(\"[DW: To] [\" .. target .. \"]: \" .. message, 255, 0, 255, 53, 5) "
		"SendAddonMessage(\"DWhisper\", message, \"WHISPER\", target) "
	"end "

	"function DinnerDank.GetRuntimeString() "
		"if DinnerDank.start == nil then "
			"return 'No DG session.' "
		"end "
   
		"local diff = GetTime() - DinnerDank.start "
		"local mins = diff / 60 "
		"local hrs = mins / 60 "
   
		"return tostring(math.floor(hrs %% 24) .. ':' .. math.floor(mins %% 60) .. ':' .. math.floor(diff %% 60)) "
	"end "
	
	"g_dconsolekey = 0 "
	"SetConsoleKey(g_dconsolekey) "

	"function DinnerDank.SetFlag() "
		"SetConsoleKey(g_dconsolekey) "
	"end "

	"function DinnerDank.Run(arg) "
		"SetCVar(\"DE\", arg) "
		// Turn off map and parse flags.
		"DinnerDank.FlagOff(1) "
		"DinnerDank.FlagOff(2) "

		// Turn on the parse flag to allow Dinnerbot to execute our lua.
		"DinnerDank.FlagOn(1) "
		"DinnerDank.SetFlag() "
	"end "

	"function DinnerDank.String(name, code, target) "
		"DinnerDank.Send(name .. ' = ' .. ('%%q'):format(code), target) "
	"end "

	"function DinnerDank.Lua(arg) "
		"DinnerDank.Run(\"lua \" .. arg) "
	"end "
	
	"function DinnerDank.Import(page) "
		"if WowLua ~= nil then "
			"local code = WowLua:GetPageText(page) "
			"DinnerDank.Lua(code) "
		"end "
	"end "

	"function DinnerDank.Reply(msg, editbox) "
		"DWhisper(msg, DinnerDank.lastfrom) "
	"end "

	"function DinnerDank.FlagOff(flag) "
		"g_dconsolekey = bit.band(bit.bnot(flag), g_dconsolekey) "
	"end "

	"function DinnerDank.FlagOn(flag) "
		"g_dconsolekey = bit.bor(g_dconsolekey, flag) "
	"end "

	// DinnerDank.TargetCallback = function(name) "something here" end
	"function DinnerDank.TargetCallback(name) "
		"print(\"Callback function not yet defined. Specify the function's use by defining DTargetCallback.\") "
	"end "
	
	"SLASH_DRUN1 = '/drun' "
	"SlashCmdList[\"DRUN\"] = DinnerDank.Run "

	"SLASH_DLUA1 = '/dlua' "
	"SlashCmdList[\"DLUA\"] = DinnerDank.Lua "

	"SLASH_DSEND1 = '/dsend' "
	"SlashCmdList[\"DSEND\"] = function(args) if args then target, cmd = string.split(\" \", args, 2) DinnerDank.Send(cmd, target) end end "

	"SLASH_DREPLY1 = '/dr' "
	"SlashCmdList[\"DREPLY\"] = DinnerDank.Reply "

	"SLASH_DWHISPER1 = '/dw' "
	"SlashCmdList[\"DWHISPER\"] = function(args) if args then target, msg = string.split(\" \", args, 2) DinnerDank.Whisper(msg, target) end end", 
	frameName.c_str(), frameName.c_str());
	Lua::DoString(script);

	// /run SendAddonMessage("HealBot", ">> Request <<=>> Horsebreed", "WHISPER", "Horsebreed")
	// /run SLASH_DREPLY1 = '/dr' SlashCmdList["DREPLY"] = DReply

	/*"elseif string.find(prefix, 'DGGRT' then "
		"DSend(\"print('\" .. DGGetRuntimeString() .. \"')\", from)"*/

	// Parse function.
	sprintf_s(script, 
	"function DinnerDank.Parse(self, event, prefix, message, type, from, ...) "
		"if string.find(prefix, \"DD\") then "
			"if message ~= nil then "
				"RunScript(message) "
			"end "

		"elseif string.find(prefix, \"DPing\") then "
			"if message ~= nil then "
				"DinnerDank.Pong(message) "
			"end "
				
		"elseif string.find(prefix, \"DPong\") then "
			"DEFAULT_CHAT_FRAME:AddMessage(\"[DD]: DPong received from \" .. from, 255, 0, 0, 53, 5) "

		"elseif string.find(prefix, \"DVersion_Req\") then "
			"SendAddonMessage(\"DVersion_Reply\", \"%s\", \"WHISPER\", from) "

		"elseif string.find(prefix, \"DVersion_Reply\") then "
			"DEFAULT_CHAT_FRAME:AddMessage(\"[DD]: \" .. from .. \" has dinner version \" .. message .. \".\", 255, 255, 0, 53, 5) "

		"elseif string.find(prefix, \"DWhisper\") then "
			"DEFAULT_CHAT_FRAME:AddMessage(\"[DW: From] [\" .. from .. \"]: \" .. message, 255, 0, 255, 53, 5) "
			"DinnerDank.lastfrom = from "
		"end "
		"end ", Dinner::versionName.c_str());
	Lua::DoString(script);

	// Register Dinnerchat hook.
	sprintf_s(script, 
		"%s:SetScript(\"OnEvent\", DinnerDank.Parse) "

		// Notify Dinnerbot that DinnerChat has been initialized properly, provided everything has been loaded.
		"if %s ~= nil and g_dconsolekey ~= nil and DinnerDank ~= nil and SLASH_DRUN1 ~= nil and SLASH_DLUA1 ~= nil and SLASH_DSEND1 ~= nil and SLASH_DREPLY1 ~= nil and SLASH_DWHISPER1 ~= nil then "
			"DinnerDank.FlagOn(4); "
			"DinnerDank.SetFlag(); "
		"end "

		"Dinner = DinnerDank; "
		"Dank = Dinner; "
		"DD = Dank;", 
		frameName.c_str(), frameName.c_str()
	);

	Lua::DoString(script);

	/*local f = CreateFrame("BUTTON", nil, UIParent, "UIPanelButtonTemplate")
f:SetWidth(100)
f:SetHeight(22)
f:SetText("Click!")
f:SetPoint("CENTER", UIParent, "CENTER")
f:SetScript("OnClick", function() ChatFrame1:AddMessage("Hello World") end)
f:SetAlpha(1)
f:Show()*/

	// Useful if we need a global channel.
	/*sprintf_s(script, "function DinnerChatParse(self, event, ...) print(self) end");
	Lua::DoString(script);

	sprintf_s(script, "function DinnerChatLove() dinnerChannel = 0 for i = 1,20 do if dinnerChannel ~= i then local Channeltext = getglobal(\"ChannelButton\"..i):GetText() if Channeltext and string.find(Channeltext, \"%s\") ~= nil then getglobal(\"ChannelButton\"..i):SetText(\"Other Channels\") getglobal(\"ChannelButton\"..i):Disable() dinnerChannel = i end end end ChatConfigCategoryFrameButton3:Hide() ChatConfigCategoryFrameButton3:Disable() end", channelName);
	Lua::DoString(script);

	sprintf_s(script, "JoinTemporaryChannel(\"%s\", \"%s\", ChatFrame1:GetID(), 0) %s = CreateFrame(\"Frame\", \"%s\") %s:Hide()", channelName, channelPass, g_frameName, g_frameName, g_frameName);
	Lua::DoString(script);

	sprintf_s(script, "%s:RegisterEvent(\"CHAT_MSG_CHANNEL\") %s:SetScript(\"OnEvent\", DinnerChatParse) %s:SetScript(\"OnUpdate\", DinnerChatLove) for i = 1,20 do RemoveChatWindowChannel(i, \"%s\") end", g_frameName, g_frameName, g_frameName, channelName);
	Lua::DoString(script);*/
	
	return true;
}

BOOL DinnerChat::SendCommand(CONST CHAR *message, CONST CHAR *target)
{
	CHAR script[4068];
	sprintf_s(script, "SendAddonMessage(\"DD\", \"%s\", \"WHISPER\", \"%s\")", message, target);
	return Lua::DoString(script);
}

BOOL DinnerChat::SendWhisper(std::string message, std::string name)
{
	return Lua::vDo("DinnerDank.Whisper(\"%s\", \"%s\")", message.c_str(), name.c_str());
}

BOOL DinnerChat::Ping(std::string playerName)
{
	//DinnerChat::SendCommand(, playerName)
	return true;
}

BOOL DinnerChat::Destroy()
{
	DinnerChat::SetMask(0);
	Lua::vDo("g_dconsolekey = 0;  if %s ~= nil then %s:Hide() %s:UnregisterAllEvents(\"CHAT_MSG_ADDON\") end", frameName.c_str(), frameName.c_str(), frameName.c_str());
	return true;
}

VOID ReplyOutputFunction(char *text)
{
	vector<string> tokens;
	Tokenize(text, tokens, "\r\n");

	for (auto &token : tokens)
		Lua::vDo("SendChatMessage(\"%s\", \"WHISPER\", nil, \"%s\")", token.c_str(), DinnerChat::lastWhisperName.c_str());
}

VOID ParseWhisperCommand(std::string name, std::string command)
{
	vlog("Command from %s: %s", name.c_str(), command.c_str());
	if (vectorHas<std::string>(name, DinnerChat::grandmothers))
	{
		vlog("Command accepted");
		DinnerChat::lastWhisperName = name;
		Commands::handler(ReplyOutputFunction, command);
	}
	else
		vlog("Command denied");
}

/******** DinnerChat namespace end ********/

VOID LogChatMessage(DWORD message)
{
	char buffer[3500];
	char szBuffer[3000];
	char name[256];
	char sType[64];

	int type = GetMessageType(message);
	
	GetChatMessage(message, szBuffer, 3000, name, 256);
	switch (type)
	{
	case SystemMessage:
		sprintf_s(sType, "System");
		break;

	case SayMessage:
		sprintf_s(sType, "Say");
		break;

	case PartyMessage:
		sprintf_s(sType, "Party");
		break;

	case RaidMessage:
		sprintf_s(sType, "Raid");
		break;

	case GuildMessage:
		sprintf_s(sType, "Guild");
		//ParseGuildChat(szBuffer, name);
		UpdateGuildChat(szBuffer, name);
		break;

	case OfficerMessage:
		sprintf_s(sType, "Officer");
		break;

	case YellMessage:
		sprintf_s(sType, "Yell");
		break;

	case FromWhisper:
	case FromWhisper2:
		sprintf_s(sType, "WFrom");
		if (PONT_MODE)
			ParseWhisperCommand(name, szBuffer);
		break;

	case ToWhisper:
		sprintf_s(sType, "WTo");
		break;

	case ChannelMessage:
		sprintf_s(sType, "Channel");
		break;

	case EmoteMessage:
		sprintf_s(sType, "Emote");
		break;

	case NPCMessage:
		sprintf_s(sType, "NPC");
		break;

	case PartyMessage2:
		sprintf_s(sType, "Party Leader");
		break;

	case RaidWarningMessage:
		sprintf_s(sType, "Raid Warning");
		break;

	default:
		sprintf_s(sType, "%d", type);
		break;

	}
	
	sprintf_s(buffer, "[%s:%d] [%s]: %s", sType, type, name, szBuffer);
	log(buffer);
}

void ParseGuildChat(char *message, char *name)
{
	char format[3500];
	sprintf_s(format, "[Guild] [%s]: %s", name, message);
	//LogChat(format);
}

VOID UpdateWoWChat()
{
	static DWORD lastMessageAddress = GetCurrentChatMessagePointer();
	DWORD currentMessage = lastMessageAddress;
	DWORD lastMessage = NULL;

	if (!g_updateWoWChat) return;

	lastMessage = GetCurrentChatMessagePointer();
	if (lastMessageAddress == lastMessage) return;
	
	// add ingame guild roster to website
	// sometimes misses messages, idk.
	while (currentMessage < lastMessage && currentMessage != NULL)
	{
		currentMessage = GetNextChatMessagePointer(currentMessage);
		LogChatMessage(currentMessage);
	}
	
	lastMessageAddress = lastMessage;
}

BOOL FakeMessageByName(const char *message, const char *objectName)
{
	Object	obj;
	CHAR	szBuffer[4000];

	obj = GetObjectByName((char *)objectName);

	if (!ValidObject(obj)) return false;
	ChatBubbleShow(obj.BaseAddress, 0x1, (char *)message);
	sprintf_s(szBuffer, "DEFAULT_CHAT_FRAME:AddMessage(\"[%s] says: %s\", 255, 255, 255, 53, 5)", obj.Name, message);
	return Lua::DoString(szBuffer);
}

BOOL FakeTargetMessage(const char *message)
{
	Object	obj;
	CHAR	szBuffer[4000];

	obj = GetCurrentTarget(BaseObjectInfo | NameInfo);

	if (!ValidObject(obj)) return false;
	ChatBubbleShow(obj.BaseAddress, 0x1, (char *)message);
	sprintf_s(szBuffer, "DEFAULT_CHAT_FRAME:AddMessage(\"[%s] says: %s\", 255, 255, 255, 53, 5)", obj.Name, message);
	return Lua::DoString(szBuffer);
}

BOOL AddChatMessage(CHAR *szMessage, CHAR *szModule, BYTE Type)
{
	DWORD	dwSendChatMessageAddress	= CGGameChat__AddChatMessage;
	DWORD	dwSubroutineAddress			= NULL;
	DWORD	dwMessageAddress			= NULL;
	DWORD	dwTotalSize					= NULL;
	HANDLE	hThread						= NULL;
	CHAR	*buffer						= NULL;

	if (!WoW::InGame() || !g_InGameChat) return FALSE;

	BYTE Sub_CallSendChatMessage[] = {
		// TLS Overhead
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, CGGameChat__AddChatMessage	
			0x6A, 0x00,							// PUSH 00 - 29
			0x6A, 0x00,							// PUSH 00 - 31
			0x6A, 0x00,							// PUSH 00 - 33
			0x6A, 0x00,							// PUSH 00 - 35
			0x6A, 0x00,							// PUSH 00 - 37 
			0x6A, 0x00,							// PUSH 00 - 39
			0x6A, 0x00,							// PUSH 00 - 41
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH 00 - sender 43 WGUID
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH 00 - chatTag 
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH 00 - player2
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH 00 - channel 49 DWORD 
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH 00 - lang 51 
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH 00 - player 53 DWORD ptr to string
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH 00 - type 55 BYTE
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH szMessage - text DWORD ptr to string		
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x3C,					// ADD ESP, 3C
			0xC3								// RETN
	};
	
	buffer = new char[strlen(szMessage) + strlen(szModule) + 4 + strlen("AddChatMessage(%s, %s, 0x%x)") + 1];

	if (!buffer) return false;
	//buffer = (CHAR *)malloc((strlen(szMessage) + strlen(szModule) + 4) + strlen("AddChatMessage(%s, %s, 0x%x)") + 1);
	sprintf(buffer, "[%s] %s", szModule, szMessage);

	dwTotalSize = sizeof(Sub_CallSendChatMessage) + strlen(buffer);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dwSubroutineAddress == NULL)
		return FALSE;

	dwMessageAddress = dwSubroutineAddress + sizeof(Sub_CallSendChatMessage);
	memcpy(&Sub_CallSendChatMessage[24], &dwSendChatMessageAddress, sizeof(dwSendChatMessageAddress));
	//memcpy(&Sub_CallSendChatMessage[43], &Type, sizeof(Type)); // Sender
	//memcpy(&Sub_CallSendChatMessage[68], &Type, sizeof(Type)); // player
	memcpy(&Sub_CallSendChatMessage[73], &Type, sizeof(Type));
	memcpy(&Sub_CallSendChatMessage[78], &dwMessageAddress, sizeof(dwMessageAddress));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallSendChatMessage, sizeof(Sub_CallSendChatMessage), 0)))
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallSendChatMessage)), buffer, strlen(buffer), 0)))
		return FALSE;
	
	SuspendWoW(); 
	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	ResumeWoW();
	
	sprintf(buffer, "AddChatMessage(%s, %s, 0x%x)", szMessage, szModule, Type);
	LogFile(buffer);

	delete[] buffer;
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_RELEASE);
	return TRUE;
}

DWORD ChatBubbleShow(DWORD base, unsigned int type, char *text)
{
	DWORD	dwSendChatMessageAddress	= CGUnit_C__ChatBubbleShow;
	DWORD	dwSubroutineAddress			= NULL;
	DWORD	dwMessageAddress			= NULL;
	DWORD	dwTotalSize					= NULL;
	HANDLE	hThread						= NULL;
	char	*buffer						= NULL;

	if (!WoW::InGame() || base == NULL || text == NULL) return FALSE;

	BYTE Sub_CallChatBubbleShow[] = {
		// TLS Overhead
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBB, 0x00, 0x00, 0x00, 0x00,		// MOV EBX, CGGameChat__AddChatMessage	
			0xB9, 0x00, 0x00, 0x00, 0x00,		// MOV ecx, base
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV edi, type
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH text
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH type			
			0xFF, 0xD3,							// CALL EDI
			0xC3								// RETN
			//cmp dword ptr [esi+00000C38],00
	};

	dwTotalSize = strlen(text) + 2;
	buffer = new char[dwTotalSize];

	memset(buffer, 0, dwTotalSize);
	strncpy(buffer, text, dwTotalSize);
	dwTotalSize = sizeof(Sub_CallChatBubbleShow) + strlen(text);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dwSubroutineAddress == NULL)
	{
		delete buffer;
		return FALSE;
	}

	dwMessageAddress = dwSubroutineAddress + sizeof(Sub_CallChatBubbleShow);
	memcpy(&Sub_CallChatBubbleShow[24], &dwSendChatMessageAddress, sizeof(dwSendChatMessageAddress));
	memcpy(&Sub_CallChatBubbleShow[29], &base, sizeof(base)); // base
	memcpy(&Sub_CallChatBubbleShow[34], &type, sizeof(type)); // type
	memcpy(&Sub_CallChatBubbleShow[39], &dwMessageAddress, sizeof(dwMessageAddress));
	memcpy(&Sub_CallChatBubbleShow[44], &type, sizeof(type));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallChatBubbleShow, sizeof(Sub_CallChatBubbleShow), 0)))
	{
		delete buffer;
		return FALSE;
	}

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress + sizeof(Sub_CallChatBubbleShow)), buffer, strlen(buffer), 0)))
	{
		delete buffer;
		return FALSE;
	}

	SuspendWoW(); 
	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
	{
		delete buffer;
		return FALSE;
	}

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	ResumeWoW();

	//return FALSE;
	delete[] buffer;
	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_RELEASE);
	return TRUE;
}

DWORD GetNextChatMessagePointer(DWORD start)
{
	DWORD nextMessage = start;
	DWORD current = start;
	DWORD buffer = NULL;
	DWORD buff = NULL;
	
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(current + NextMessage), &buffer, sizeof(buffer), NULL)) return NULL;
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(current + MessageSequence), &buff, sizeof(buff), NULL)) return NULL;
	nextMessage = current;

	if (buffer != 0)
		current += NextMessage;
	
	return current;
}

DWORD GetPreviousChatMessagePointer(DWORD start)
{
	DWORD nextMessage = start;
	DWORD current = start;
	DWORD buffer = NULL;
	DWORD buff = NULL;

	if (!ReadProcessMemory(WoW::handle, (LPVOID)(current + NextMessage), &buffer, sizeof(buffer), NULL)) return NULL;
	if (!ReadProcessMemory(WoW::handle, (LPVOID)(current + MessageSequence), &buff, sizeof(buff), NULL)) return NULL;
	nextMessage = current;

	if (buffer != 0)
		current -= NextMessage;

	return current;
}

DWORD GetCurrentChatMessagePointer()
{
	DWORD currentChat = ChatBufferStart;
	DWORD nextMessage = ChatBufferStart;
	DWORD buffer = NULL;

	uint largest = 0;
	uint buff = 0;

	do
	{
		rpm(currentChat + NextMessage, &buffer, sizeof(buffer));
		rpm((currentChat + MessageSequence), &buff, sizeof(buff));

		if (buff > largest)
		{
			largest = buff;
			nextMessage = currentChat;
		}

		// If the current message address is valid, point to the next message.
		if (buffer != 0)
			currentChat += NextMessage;
	}
	while (buffer != NULL);

	// Return the message with the highest 
	return nextMessage;
}

unsigned int GetMessageType(DWORD message)
{
	uint buffer = 0;

	rpm(message + MessageType, &buffer, sizeof(buffer));
	return buffer;
}

unsigned int GetLastChatType()
{
	return GetMessageType(GetCurrentChatMessagePointer());
}

VOID UpdateGuildChat(char *message, char *name)
{
	char format[3500];
	sprintf_s(format, "[Guild] [%s]: %s", name, message);
	LogChat(format);
}

unsigned int GetChatMessage(DWORD message, char *out, size_t maxOut, char *sender, size_t maxSender)
{
	char buffer[3000];
	size_t read = 0;

	ReadProcessMemory(WoW::handle, (LPVOID)(message + MessageSender), &buffer, sizeof(buffer), (SIZE_T *)&read);
	strncpy(sender, buffer, (strlen(buffer) > maxSender)?(maxSender):(strlen(buffer) + 1));

	// Read raw message into buffer.
	ReadProcessMemory(WoW::handle, (LPVOID)(message + RawMessage), &buffer, sizeof(buffer), NULL);
	strncpy(out, buffer, (strlen(buffer) > maxOut)?(maxOut):(strlen(buffer) + 1));

	return strlen(out);
}

// returns pointer to last chat buffer
DWORD GetLastChatMessage(char *out, size_t maxOut, char *sender, size_t maxSender)
{
	char buffer[3000];

	uint buff = 0;
	uint currentChat = GetCurrentChatMessagePointer();

	// Read sender's name into buffer.
	ReadProcessMemory(WoW::handle, (LPVOID)(currentChat + MessageSender), &buffer, sizeof(buffer), NULL);
	strncpy(sender, buffer, (strlen(buffer) > maxSender)?(maxSender):(strlen(buffer) + 1));

	// Read raw message into buffer.
	ReadProcessMemory(WoW::handle, (LPVOID)(currentChat + RawMessage), &buffer, sizeof(buffer), NULL);
	strncpy(out, buffer, (strlen(buffer) > maxOut)?(maxOut):(strlen(buffer) + 1));

	return currentChat;
}

BOOL WoWConsoleWrite(CHAR *szMessage, BYTE Colour)
{
	DWORD  dwConsoleWriteAddress = ConsoleWrite;
	DWORD  dwSubroutineAddress = NULL;
	DWORD  dwMessageAddress = NULL;
	DWORD  dwTotalSize = NULL;
	HANDLE hThread = NULL;
	CHAR *szBuffer = NULL;



	if (!WoW::InGame())
		return FALSE;

	szBuffer = new char[(strlen(szMessage) + 3)*sizeof(CHAR)+1];
	if (!szBuffer) return false;

	BYTE Sub_CallConsoleWrite[] = {
		// TLS Overhead
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager

		// Start of functional code
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, ConsoleWriteAddress	
			0x6A, 0x00,							// PUSH 0
			0x68, 0x00, 0x00, 0x00, 0x00,		// PUSH szMessage							
			0xFF, 0xD7,							// CALL EDI
			0x83, 0xC4, 0x08,					// ADD ESP, 08
			0xC3								// RETN
	};
	
	sprintf(szBuffer, "[D] %s", szMessage);
	dwTotalSize = sizeof(Sub_CallConsoleWrite) + strlen(szBuffer);
	dwSubroutineAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, dwTotalSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dwSubroutineAddress == NULL)
		return FALSE;

	dwMessageAddress = dwSubroutineAddress + sizeof(Sub_CallConsoleWrite);
	memcpy(&Sub_CallConsoleWrite[24], &dwConsoleWriteAddress, sizeof(dwConsoleWriteAddress));
	memcpy(&Sub_CallConsoleWrite[29], &Colour, sizeof(Colour));
	memcpy(&Sub_CallConsoleWrite[31], &dwMessageAddress, sizeof(dwMessageAddress));

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)(dwSubroutineAddress), &Sub_CallConsoleWrite, sizeof(Sub_CallConsoleWrite), 0)))
		return FALSE;

	if (!(WriteProcessMemory(WoW::handle, (LPVOID)dwMessageAddress, szBuffer, strlen(szBuffer), 0)))
		return FALSE;

	hThread = CreateRemoteThread(WoW::handle, NULL, NULL, (LPTHREAD_START_ROUTINE)dwSubroutineAddress, NULL, NULL, NULL);
	if (hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);

	VirtualFreeEx(WoW::handle, (LPVOID)dwSubroutineAddress, dwTotalSize, MEM_RELEASE);
	return TRUE;
}

// <Various Lua scripts>
	// /run function DinnerChatParse(self, event, prefix, message, type, target, ...) if string.find(prefix, "DinnerDo") then RunScript(message) end end
	// /run DinnerDick = CreateFrame("Frame", "DinnerDick") DinnerDick:Hide()
	// /run DinnerDick:RegisterEvent("CHAT_MSG_ADDON") DinnerDick:SetScript("OnEvent", DinnerChatParse)

	// print(\"[\" .. event .. \"][\" .. prefix .. \"]: \" .. message) if type ~= nil then print(type) end if target ~= nil then print(target) end 

	// Set up functions. /run ChatFrame1:AddMessage("ayy", 255, 255, 255, 53, 5)
	
	// /run function dicks(event) if event == INSPECT_ACHIEVEMENT_READY then print("ayy") print(GetComparisonStatistic(334)) end end q ayet:RegisterEvent("INSPECT_ACHIEVEMENT_READY") ayet:SetScript("OnEvent", dicks)

	// Dinnerchat whisper script. 
	//sprintf_s(script, "function DWReply(message) if DW_lastsender ~= nil then DWSend(message, DW_lastsender) end end function DWRecv(message, from) ChatFrame1:AddMessage(\"[DW:From] [\" .. from .. \"]: \" .. message, 255, 0, 0, 53, 5) end function DWSend(message, target) SendAddonMessage(\"DWhisper\", message, \"WHISPER\", target) ChatFrame1:AddMessage(\"[DW:To] [\" .. target .. \"]: \" .. message, 255, 0, 0, 53, 5) end");
	// Append to DParse: "elseif string.find(prefix, \"DWhisper\") then DWRecv(message, from) DW_lastsender = from" 

	// /run function ayycb(event, arg1, arg2) print(event) end ayy = CreateFrame("frame") ayy:RegisterEvent("CHAT_MSG_CHANNEL_LIST") ayy:SetScript("OnEvent", ayycb)
	// /run ayy = CreateFrame("frame")
	// /run ayy:RegisterEvent("CHAT_MSG_CHANNEL_LIST")
	// /run ayy:SetScript("OnEvent", ayycb)

	// /dump GetSelectedDisplayChannel()
	// /dump SetSelectedDisplayChannel()
	// "/run function FuckupPunisChannel(index) for i = 1:GetNumChannelMembers(index) do local player = GetChannelRosterInfo(index, i) print(player) end end";

	// Change alpha
	// /run d = CreateFrame("Frame") d:SetScript("OnUpdate", function() frame:SetAlpha(math.random(100)/100) end)

	// /run DEFAULT_CHAT_FRAME:AddMessage("Nathale rolls 99 (1-100)", 255, 255, 0, nil, 5)
	// </Various Lua scripts>