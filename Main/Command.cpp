#include "Command.h"

#include "WoW API\Chat.h"
#include "WoW API\Lua.h"
#include "WoW API\Warden.h"
#include "WoW API\CGLoot.h"
#include "WoW API\Environment.h"
#include "Game\Interactions.h"

#include "Radar\NetEditor.h"
#include "Memory\Endscene.h"

#include "Dialog\ObjectWatch.h"
#include "Geometry\Coordinate.h"

#include "Memory\Hooks.h"
#include "Memory\Hacks.h"

#include "Dialog\Dialog.h"
#include "Radar\Drawing.h"
	
#include "Main\Debug.h"
	
#include "Pont\HTML.h"

#include "Game\Multibox.h"

#include <algorithm>

using namespace std;

INT CommandCmp(vector<string> &t, size_t n, const char *str)
{
	if (t.size() <= n) return false;
	return !_stricmp(t[n].c_str(), str);
}

namespace Commands
{
	vector<Command *> commandList;
	vector<string>	  commandHistory;
	int commandIndex = 0;

	void setupHandler()
	{
		ArgList args[] = {

			/* ArgList struct has the format:
				{ List of command string names },
				Help text,
				Pointer to command function

				<0> Denotes the empty parameter
			*/

			// !relay command: relays output to log, ingame, chat, whisper, etc
			{ { "bot" }, 
				"Bot commands", Commands::StopBot 
			},

			{ { "cd" }, "Displays/Changes the current directory on the local machine.", Commands::CD },

			{ { "debug" }, "Executes Dinner's Debug function", Commands::Debug },

			{ { "fun" }, "Some fun features", Commands::Fun },

			{ { "ss" }, "Automatically clicks any nearby summoning portal.\r\n" "on/off: Turn on/off the ss clicker.", Commands::Summon },

			{ { "help", "cmds", "commands" },
				"Displays help text:\r\n"
					"name: Displays the help text of the specified command\r\n"
					"<0>: Displays list of commands",

				Commands::Help,
			},
			{ { "radar" }, 
				"Opens the Dinner radar", 
				Commands::Radar,
			},

			{ { "version" }, 
				"Returns version of Dinnerbot\r\n"
					"name: Attempts to return version of specified player\r\n"
					"<0>: Returns local dinnerbot version", 

				Commands::Version 
			},

			{ { "pont" }, 
				"Open up the links to the heavens to interface with Pont\r\n"
					"on/off: turns Pont mode on/off\r\n"
					"fixchat: attempts to fix the chatlog",  
				Commands::Pont 
			},

			{ { "relogger" },
				"Toggle relogger:\r\n"
					"on: Enables the relogger\r\n"
					"off: Disables the relogger",

				Commands::Relogger,
			},

			{ { "chat" }, "Displays in-game chat to Dinner's log", Commands::Chat },
			{ { "luacvar", "luaparse" }, "Toggles in-game lua parsing via /drun", Commands::LuaCVar },
			{ { "verbose", "vv" }, "Outputs Dinner log to in-game chat", Commands::Verbose },
			{ { "logout" }, "Executes in-game logout via lua", Commands::Logout },
			{ { "morph", "shift" },
				"Invokes the model morpher:\r\n"
					"display Id: morphs local player into specified display Id\r\n"
					"<0>: morphs local player into target model",

				Commands::Morph,
			},

			{ { "net" }, 
				"Invokes the net editor:\r\n"
					"hole: Notifies the net editor that the next node will be a hole node\r\n"
					"close: Closes the currently edited net object\r\n"
					"contour: Notifies the net editor that the next node will be a contour node\r\n"
					"triangulate: Convert all defined contours and hols into a triangulated net\r\n"
					"clear: Clears the current net being edited\r\n"
					"save: Saves the current net to the given file path\r\n"
					"load: Loads the net from the specified file",
			
				Commands::NetEditor 
			},


			{ { "follow" }, "Follows the local player's target", Commands::Follow },
			{ { "playercount", "area", "pc" }, "Displays number of players in the area of the local player", Commands::PlayerCount },

			{ { "metarget" }, "Lists players in area currently targetting the local player", Commands::MeTarget },
			{ { "callback" }, "In-game callback which procs on every unit/player encountered", Commands::Callback },
			{ { "interact" }, "Interacts with the target, or given object name", Commands::Interact },

			{ { "turn", "face" },
				"Turns local player according to the following commands:\r\n"
					"name: Faces the object with the specified name\r\n"
					"<0>: Faces the local player's target",

				Commands::Turn,
			},

			{ { "warden" },
				"Warden functions:\r\n"
					"unload / disable / off: Disables warding by calling Warden::Unload()\r\n" 
					"load / on / enable: Enables warden by calling Warden::Load()\r\n"
					"info: Returns information about warden such as base, isloaded, etc",

				Commands::Warden 
			},

			{ { "movespicy" },
				"Moves Spices",
				Commands::MoveSpicy
			},

			{ { "teleport", "tele" }, "Teleports to specified x, y, z coordinates", Commands::Teleport },
			{ { "walk", "tele" }, "Walks to specified x, y, z coordinates", Commands::Walk},

			{ { "dump", "info" }, 
				"Display information about an object:\r\n"
					"address: Displays Dinner's address table\r\n"
					"local: Displays the local player's information\r\n"
					"base: Displays the base object of the local player's target\r\n"
					"location: Displays the location of the local player's target\r\n"
					"unit: Displays the unit field of the local player's target\r\n"
					"gameobject: Displays the game object field of the local player's target",

				Commands::Dump,			
			},

			{ { "fakemsg", "fake" },
				"Displays a fake /s message in-game (Client-side):\r\n"
					"name: Creates the message with specified sender name\r\n"
					"target: Creates the message with local player's target's name",

				Commands::FakeMsg,
			},

			{ { "cast" },
				"Casts the specified spell:\r\n"
					"spellId: Casts spell by its spellId\r\n"
					"name: Uses its name to attempt to cast it",

				Commands::Cast, 
			},

			{ { "multibox" }, "Multibox with given list of player names", Commands::Multibox, },

			{ { "party" },
				"Send specified message to party chat",

				Commands::SendParty,
			},

			{ { "guild" }, "Send specified message to guild chat", Commands::SendGuild },

			{ { "whisper", "w" }, 
				"Syntax: !w <name> <message>\r\n"
					"Example: !w Mouthman Hello, how are you mouth man?", 
				Commands::SendWhisper
			},
					
			{ { "render", "rendering" },
				"Toggles in-game rendering:\r\n"
					"object: Object\r\n"
					"player: Player\r\n"
					"water: Water\r\n"
					"wmo: WMO - Buildings, etc\r\n"
					"world: Turn off the world\r\n"
					"m2:  M2 - Trees, rocks, etc\r\n"
					"horizon: Horizon - background mountains\r\n"
					"terrain: Terrain - floor, ground\r\n"
					"wireframe: Outlines of objects only, i.e., surfaces of objects removed",

				Commands::Render,
			},

			{ { "tar", "target" }, "Target object with specified name", Commands::Target },
			{ { "luaget" }, "Displays return of specified lua variable from in-game", Commands::LuaGet },

			{ { "loot", "lootall" }, 
				"Loot functions:\r\n"
					"local: Loots all corpses near the local player\r\n"
					"<0>: loots local", 

				Commands::Loot,
			},

			{ { "hack" }, 
				"Hack functions:\r\n"
					"ctt (on/off): Click to teleport hack\r\n"
					"vfly (on/off): Vertical nudge hack\r\n"
					"fall (on/off): Turn on/off fall damage\r\n"
					"fix (on/off): Fix local player's Z coordinate",

				Commands::Hack,
			},

			{ { "fog" }, 
				"In-game fog manipulator:\r\n"
					"on: Removes all fog\r\n"
					"off: Restores fog to default state",

				Commands::Fog,
			},

			{ { "lua" }, "Runs the specified lua string in-game", Commands::Lua },
			{ { "map" }, 
				"World map functions:\r\n"
					"unload (mapId): Calls unload world map on the specified Id",

				Commands::Map,
			},

			{ { "gandalf", "gandelf" }, "Turns the local player into gandalf the gay", Commands::Gandalf },

			{ { "goto", "carbonite", }, 
				"Goto location:\r\n"
					"walk: Walks to the goto point created on Carbonite's world map\r\n"
					"net: Uses current net to navigate to Carbonite's goto\r\n"
					"teleport: Teleports to the goto point created on Carbonite's world map\r\n"
					"off: Turn carbonite goto handler off",

				Commands::Goto,
			},

			{ { "atme", "to-me", "to_me" }, "Teleports the local player's target to the local player's position", Commands::To_me },
			{ { "wsg", "flag", "bgflag", }, 
				"on/off: Automatically grabs bg flag when in range", 
				Commands::Flag,
			},

			//{ { "watch" }, "Put object on 'watch' list?", Commands::Watch },

			{ { "search", "objsearch", "objectsearch" }, 
				"Search for an object:\r\n"
					"find (name): Begins Searching for an object with the specified name\r\n"
					"walk: Walks to the point where the last object was found\r\n"
					"off: Stops object search",

				Commands::ObjectSearch, 
			},

			{ { "hide" }, 
				"<0>: Toggles the hiding of WoW's window",
				Commands::Hide,
			},

			{ { "log" },
				"Dinner GUI log functions:\r\n"
					"clear: Clears the dinner GUI log",

				Commands::Log,
			},

			{ { "key" }, 
				"Send keyboard input to WoW:\r\n"
					"enter\r\n"
					"up\r\n"
					"down",

				Commands::Key,
			},

			{ { "desync" },
				"Turn off movement synch with the server:\r\n"
					"on: Player can run around, but sends no movement packets to the server\r\n"
					"on: Synch with the server again",

				Commands::Desync,
			},

			{ { "collision" }, 
				"Player collision with things in-game:\r\n"
					"wmo (state): Change collision with buildings, etc\r\n"
					"m2 (state): Change collision with trees, etc",

				Commands::Collision,
			},
		};

		// Add all hardcoded commands to command list.
		for (int x = 0; x < sizeof(args) / sizeof(ArgList); x++)
			commandList.push_back(new Command(args[x].names, args[x].help, args[x].f));

	}

	Command *findCommand(const char *t)
	{
		Command *command = NULL;

		// Check command list to see if the given command is defined
		for (int x = 0; x < commandList.size(); x++)
		{
			command = commandList[x];
			// If commandName is a synonym of one of our commands, return its index
			if (*command == t)
			{
				return command;
			}
		}

		// Command not found
		return NULL;
	}

	bool handler(const std::string &buffer)
	{
		return handler(LogAppend, buffer);
	}

	bool handler(OutputFunction out, const std::string &buffer)
	{
		static bool setup = false;
		vector<string> tokens;

		string *commandName = NULL;

		// Initialize hardcoded list of commands
		if (!setup)
		{
			setupHandler();
			setup = true;
		}

		// Split command string into words
		// "weed jesus" -> "weed", "jesus"
		Tokenize(buffer, tokens, " ");

		if (tokens.size() <= 0) return false;
		commandName = &tokens[0];

		// Erase '!' at beginning of string
		commandName->erase(commandName->begin());

		// If we are holding too many commands in our history, clear it
		if (commandHistory.size() > 100)
		{
			commandHistory.clear();
			commandIndex = 0;
		}

		// Add new command to history
		commandHistory.push_back(buffer);
		commandIndex = commandHistory.size();

		Command *command = findCommand(commandName->c_str());

		if (command != NULL)
			command->execute(out, tokens);

		return true;
	}

	VOID Multibox(OutputFunction out, vector<string> &t)
	{
		Thread *thread = NULL;
		vector<string> *cpy = NULL;

		thread = Thread::FindType(Thread::eType::Multibox);
		if (t.size() > 1 && !thread)
		{
			t.erase(t.begin(), t.begin() + 1);
			cpy = new vector<string>;
			if (cpy)
			{
				*cpy = t;
				thread = Thread::Create(multibox, cpy, Thread::eType::Multibox);
				vout(out, "Multiboxer started.");
			}
			else
				vout(out, "Unable to allocate memory.");
		}
		else
		{
			thread->stop();
			vout(out, "Multiboxer stopped.");
		}
	}

	VOID Teleport(OutputFunction out, vector<string> &t)
	{
		float x, y, z;

		if (t.size() < 4) return;

		x = strtod(t[1].c_str(), NULL);
		y = strtod(t[2].c_str(), NULL);
		z = strtod(t[3].c_str(), NULL);

		Hack::Movement::Teleport(x, y, z);
		vout(out, "Teleported to %0.2f, %0.2f, %0.2f", x, y, z);
	}

	VOID Walk(OutputFunction out, vector<string> &t)
	{
		float x, y, z;

		if (t.size() < 4) return;

		x = strtof(t[1].c_str(), NULL);
		y = strtof(t[2].c_str(), NULL);
		z = strtof(t[3].c_str(), NULL);

		ClickToMove(x, y, z);
		vout(out, "Walking to %0.2f, %0.2f, %0.2f", x, y, z);
	}

	VOID Help(OutputFunction out, vector<string> &t)
	{
		string *commandString = NULL;
		Command *command = NULL;

		// Get help text of command
		if (t.size() > 1)
		{
			// Name of command  to get help from
			commandString = &t[1];
			command = findCommand(commandString->c_str());

			// Display help text
			if (command != NULL)
			{
				vout(out, "Help text for %s:", commandString->c_str());
				vout(out, command->helpText.c_str());
			}
		}

		// List commands
		else
		{
			string full;
			string synonyms;

			vout(out, "Dinner commands:");
			for (int x = 0; x < commandList.size(); x++)
			{
				command = commandList[x];
				synonyms.clear();
				
				synonyms = "!";
				synonyms += command->commandSynonyms[0];

				// Get all command synonyms
				for (int y = 1; y < command->commandSynonyms.size(); y++)
				{
					synonyms += ", ";
					synonyms += "!";
					synonyms += command->commandSynonyms[y];
				}

				full += synonyms;
				full += "\r\n";
			}

			vout(out, full.c_str());
		}


	}

	VOID Gandalf(OutputFunction out, vector<string> &)
	{
		Hack::Morph::Morph(390);
		AddChatMessage("Handelf Mode Activate	[Dank]", "Dank", RaidWarningMessage);
	}

	VOID Logout(OutputFunction out, vector<string> &)
	{
		Lua::Run("Logout()");
	}

	VOID MoveSpicy(OutputFunction out, vector<string> &)
	{
		/// dlua g_dankmove = true CreateFrame("Frame") :SetScript("OnUpdate", function() if g_dankmove then MoveForwardStop() MoveForwardStart() StrafeLeftStart() StrafeLeftStop()  StrafeRightStart()  StrafeRightStop() end end)

		//Lua::Run("CreateFrame("Frame"):SetScript("OnUpdate", function() MoveForwardStop() MoveForwardStart() end)")
		//Lua::Run("CreateFrame("Frame"):SetScript("OnUpdate", function() MoveForwardStop() MoveForwardStart() end)")
	}

	VOID Chat(OutputFunction out, vector<string> &)
	{
		if (!g_updateWoWChat) vout(out, "Logging in-game chat");
		else vout(out, "Stopped in-game chat logger");

		g_updateWoWChat = !g_updateWoWChat;
	}

	VOID Warden(OutputFunction out, vector<string> &t)
	{
		CHAR buffer[256];

		if (CommandCmp(t, 1, "unload") || CommandCmp(t, 1, "disable") || CommandCmp(t, 1, "off"))
		{
			Warden::Unload();
			if (Warden::IsLoaded())
			{
				vout(out, "Warden disabled");
			}
			else
			{
				vout(out, "Unable to unload warden");
			}
		}
		else if (CommandCmp(t, 1, "load") || CommandCmp(t, 1, "on") || CommandCmp(t, 1, "enable"))
		{
			Warden::Load();
			if (Warden::IsLoaded())
			{
				vout(out, "Warden enabled");
			}
			else
			{
				vout(out, "Warden disabled");
			}
		}
		else if (CommandCmp(t, 1, "info"))
		{
			vout(out, "Warden information:");
			sprintf_s(buffer, "Base: 0x%x", Warden::GetBase());
			vout(out, buffer);

			sprintf_s(buffer, "Class: 0x%x", Warden::GetClass());
			vout(out, buffer);

			sprintf_s(buffer, "ScanModule: 0x%x", Warden::GetScanModule());
			vout(out, buffer);
		}
	}

	VOID LuaCVar(OutputFunction out, vector<string> &)
	{
		g_LuaCVar = !g_LuaCVar;
		if (!g_LuaCVar)
		{
			vout(out, "Lua CVar exectuion disabled");
		}
		else
		{
			DinnerChat::FlagOff(TOGGLE_PARSE);
			vout(out, "Lua CVar exectuion enabled");
		}
	}

	VOID FakeMsg(OutputFunction out, vector<string> &tokens)
	{
		string arg;

		for (int x = 2; x < tokens.size(); x++)
		{
			arg.append(tokens[x]);
			arg.append(" ");
		}

		if (!_stricmp(tokens[1].c_str(), "target"))
		{
			FakeTargetMessage(arg.c_str());
		}
		else
		{
			FakeMessageByName(arg.c_str(), tokens[1].c_str());
		}
	}

	VOID Chatlog(OutputFunction out, vector<string> &)
	{
		g_chatLog = !g_chatLog;
		LogStatus(g_chatLog);
		if (!g_chatLog)
		{
			vout(out, "Chat logging turned off");
		}
		else
		{
			vout(out, "Chat logging turned on. Guild chat is outputted to the file chatlog.txt");
		}
	}

	VOID Cast(OutputFunction out, vector<string> &t)
	{
		int spellId = 0;

		if (t.size() > 1)
		{
			if (isinteger(t[1].c_str()))
			{
				spellId = atoi(t[1].c_str());
				CastSpellByID(spellId);

				vout(out, "Casted spell %d", spellId);
			}
		}
	}

	VOID CD(OutputFunction out, vector<string> &t)
	{
		char buffer[MAX_PATH];

		// If there is an argument, we will use that to change the directory.
		// NOTE: Doesn't work for some reason. SetCurrentDirectory has problems with the '\' format?
		if (t.size() > 1)
		{
			string newCD = "";

			// Join the tokens back together.
			for (int x = 0; x < t.size(); x++)
			{
				if (x != 0)
				{
					newCD += t[x];
					newCD += " ";
				}
			}

			// Remove the trailing space.
			newCD = newCD.substr(0, newCD.size() - 1);
			
			// Replace all \ characters with \\.
			std::size_t pos = newCD.find('\\');
			while (pos != string::npos)
			{
				newCD.replace(pos, pos + 1, "\\\\");
				pos = newCD.find('\\', pos + 2);
			}

			// Change directory.
			if (SetCurrentDirectory(newCD.c_str()))
				vout(out, "Changed the current directory to %s", newCD.c_str());
			else
			{
				vout(out, "Unable to change directory (Error %d)", GetLastError());
				vout(out, "Directory: %s", newCD.c_str());
			}
		}

		// Otherwise, just display the current directory.
		else
		{
			GetCurrentDirectory(MAX_PATH, buffer);
			vout(out, "Current Directory: %s", string(buffer).c_str());
		}
	}

	VOID NetEditor(OutputFunction out, vector<string> &tokens)
	{
		if (CommandCmp(tokens, 1, "hole"))
		{
			NetEditor::ChangeEditMode(NetEditor::Hole);
			vout(out, "NetEditor: Hole mode");
		}

		else if (CommandCmp(tokens, 1, "contour"))
		{
			NetEditor::ChangeEditMode(NetEditor::Contour);
			vout(out, "NetEditor: Contour mode");
		}

		else if (CommandCmp(tokens, 1, "close"))
		{
			if (NetEditor::editMode == NetEditor::Contour)
				NetEditor::CloseContour();

			else if (NetEditor::editMode == NetEditor::Hole)
				NetEditor::CloseHole();
		}

		else if (CommandCmp(tokens, 1, "triangulate"))
		{
			NetEditor::Triangulate();
		}

		else if (CommandCmp(tokens, 1, "clear"))
		{
			NetEditor::Clear();
		}

		else if (CommandCmp(tokens, 1, "save"))
		{
			if (tokens.size() > 2)
				NetEditor::SaveNet(tokens[2]);
		}

		else if (CommandCmp(tokens, 1, "load"))
		{
			if (tokens.size() > 2)
			{
				NetEditor::LoadNet(tokens[2]);
			}
		}

	}

	VOID SendWhisper(OutputFunction out, vector<string> &tokens)
	{
		string arg;

		// Correct whitespace when gluing string back together.
		for (int x = 2; x < tokens.size(); x++)
		{
			arg.append(tokens[x]);
			if (x + 1 < tokens.size())
				arg.append(" ");
		}

		// Remove trailing whitespace.
		//arg.erase(remove_if(arg.begin(), arg.end(), isspace), arg.end());

		// Execute lua dostring.
		Lua::vDo("SendChatMessage(\"%s\", \"WHISPER\", nil, \"%s\")", arg.c_str(), tokens[1].c_str());
		vout(out, "Sent >%s< to %s", arg.c_str(), tokens[1].c_str());
	}

	VOID SendChatMessage(OutputFunction out, vector<string> &tokens, CONST CHAR *msgType)
	{
		string arg;

		for (int x = 1; x < tokens.size(); x++)
		{
			arg.append(tokens[x]);
			if (x + 1 < tokens.size())
				arg.append(" ");
		}

		//arg.erase(remove_if(arg.begin(), arg.end(), isspace), arg.end());
		Lua::vDo("SendChatMessage(\"%s\", \"%s\", nil, nil)", arg.c_str(), msgType);

		vout(out, "Sent >%s< to %s chat", arg.c_str(), msgType);
	}

	VOID SendParty(OutputFunction out, vector<string> &t)
	{
		Commands::SendChatMessage(out, t, "PARTY");
	}

	VOID SendGuild(OutputFunction out, vector<string> &t)
	{
		Commands::SendChatMessage(out, t, "GUILD");
	}

	VOID Whisper(OutputFunction out, vector<string> &t)
	{
		if (t.size() > 2)
		{
			std::string &name = t[1];
			std::string message = t[2];
			
			// Join tokens back together
			for (int x = 3; x < t.size(); x++)
			{
				message += " ";
				message += t[x];
			}

			Lua::sendWhisper(name, message);
		}
	}

	VOID Target(OutputFunction out, vector<string> &tokens)
	{
		TargetObject(GetObjectByName((char *)tokens[1].c_str()).GUID);
	}

	VOID Hide(OutputFunction out, vector<string> &tokens)
	{
		if (IsWindowVisible(g_hwWoW))
		{
			ShowWindow(g_hwWoW, SW_HIDE);
			vout(out, "WoW window hidden");
		}
		else
		{
			ShowWindow(g_hwWoW, SW_SHOW);
			vout(out, "WoW window enabled");
		}
	}

	VOID Lua(OutputFunction out, vector<string> &tokens)
	{
		string	arg;

		for (int x = 1; x < tokens.size(); x++)
		{
			arg.append(tokens[x]);
			arg.append(" ");
		}

		if (Lua::Run(arg.c_str()))
		{
			vout(out, "Ran script: %s", arg.c_str());
		}
		else
		{
			vout(out, "Unable to execute Lua");
		}
	}

	VOID LuaGet(OutputFunction out, vector<string> &t)
	{
		string arg;

		CHAR get[FUNCTIONSPACE_MAX];

		arg = t[1];
		arg.erase(remove_if(arg.begin(), arg.end(), isspace), arg.end());
		Lua::GetText(t[1].c_str(), get);

		vout(out, "Lua::GetText(%s): %s", arg.c_str(), get);
	}

	VOID StopBot(OutputFunction out, vector<string> &)
	{
		Thread *botThread = NULL;
		botThread = Thread::FindType(Thread::eType::Bot);

		if (!botThread)
		{
			vout(out, "No bots currently running");
		}
		else
		{
			botThread->stop();
			vout(out, "All bots stopped");
		}

	}

	VOID Fog(OutputFunction out, vector<string> &t)
	{
		string arg;

		if (CommandCmp(t, 1, "off"))
		{
			if (!IsFogColorPatched())
			{
				PatchFogColorProtection();

				g_fFogDensity = 0;
				SetFogDensity(g_fFogDensity);
				vout(out, "Fog turned off");
			}
		}
		else if (CommandCmp(t, 1, "on"))
		{
			if (IsFogColorPatched())
			{
				RestoreFogColorProtection();
				vout(out, "Fog turned on");
			}
		}
	}

	VOID ObjectSearch(OutputFunction out, vector<string> &t)
	{
		string arg;
		static CObject object;

		if (CommandCmp(t, 1, "find"))
		{
			// Use all names after first token in search.
			for (int x = 2; x < t.size(); x++)
			{
				arg.append(t[x]);
				if (x + 1 < t.size()) arg.append(" ");
			}

			g_Searching = true;
			strncpy_s(g_szSearch, arg.c_str(), arg.length());
			vout(out, "Searching for %s", g_szSearch);
		}
		else if (CommandCmp(t, 1, "walk"))
		{
			if (ValidCoord(g_TeleportPos))
			{
				ClickToMove(g_TeleportPos);
				vout(out, "Walking to (%0.2f, %0.2f, %0.2f)", g_TeleportPos.X, g_TeleportPos.Y, g_TeleportPos.Z);
			}
		}
		else if (CommandCmp(t, 1, "off")
			|| CommandCmp(t, 1, "stop"))
		{
			g_Searching = false;
			vout(out, "Stopped the search");
		}
	}

	VOID Verbose(OutputFunction out, vector<string> &)
	{
		vout(out, "Log messages being outputted to %s", (g_VerboseInGameChat) ? "log only" : "log and ingame chat");
		g_VerboseInGameChat = !g_VerboseInGameChat;
	}

	VOID Loot(OutputFunction out, vector<string> &t)
	{
		/*if (CommandCmp(t, 1, "local"))
		{*/
			//ObjectManager::ObjectCallback(Loot::local);
		Loot::local();
		vout(out, "Looted local corpses.");
		//}
	}

	VOID MeTarget(OutputFunction out, vector<string> &)
	{
		ObjectManager::ObjectCallback(MeTargetCallback);
	}

	VOID CharacterSelect(OutputFunction out, vector<string> &t)
	{
		DWORD sel = 1;
		DWORD characterSelectAddress = Offsets::CharSelectIndexAddress;
		DWORD glueCharSel = Offsets::CGlueMgr__CharacterSelect;
		DWORD bytes = 0;

		if (WoW::InGame()) return;
		if (t.size() < 2) return;
		if (CommandCmp(t, 1, "enter"))
		{
			CallWoWFunction(Offsets::Lua__EnterWorld, (LPVOID)NULLCALL);
		}
		else if (CommandCmp(t, 1, "sel"))
		{
			sel = atoi(t[2].c_str());
			WriteProcessMemory(WoW::handle, (LPVOID)characterSelectAddress, (LPVOID)&sel, sizeof(--sel), &bytes);
			CallWoWFunction(glueCharSel, (LPVOID)NULLCALL);
			FrameScriptSignalEvent(8, "%d", ++sel);
		}

		//VirtualProtectEx(WoW::handle, (LPVOID)characterSelectAddress, sizeof(sel), PAGE_EXECUTE_READWRITE, &oldProtect);
		//VirtualProtectEx(WoW::handle, (LPVOID)characterSelectAddress, sizeof(sel), oldProtect, NULL);
	}

	VOID Collision(OutputFunction out, vector<string> &t)
	{
		if (CommandCmp(t, 1, "m2"))
		{
			if (CommandCmp(t, 2, "on"))
			{
				Hack::Collision::M2(true);
				vout(out, "M2 Collsion hack enabled");
			}
			else if (CommandCmp(t, 2, "off"))
			{
				Hack::Collision::M2(false);
				vout(out, "M2 Collsion hack disabled");
			}
		}

		else if (CommandCmp(t, 1, "wmo"))
		{
			if (CommandCmp(t, 2, "on"))
			{
				Hack::Collision::WMO(true);
				vout(out, "WMO Collsion hack enabled");
			}
			else if (CommandCmp(t, 2, "off"))
			{
				Hack::Collision::WMO(false);
				vout(out, "WMO Collsion hack disabled");
			}
		}
	}

	VOID Hack(OutputFunction out, vector<string> &t)
	{
		DWORD sel = 1;
		DWORD characterSelectAddress = Offsets::CharSelectIndexAddress;
		DWORD glueCharSel = Offsets::CGlueMgr__CharacterSelect;
		DWORD bytes = 0;

		// Click to teleport hack.
		if (CommandCmp(t, 1, "ctt"))
		{
			if (CommandCmp(t, 2, "on"))
			{
				StartClickToTeleport(LocalPlayer.base());
			}
			else if (CommandCmp(t, 2, "off"))
			{
				CleanPatch(Offsets::CGPlayer_C__ClickToMove);
			}
		}

		// Fix local player's Z coordinate.
		else if (CommandCmp(t, 1, "fix"))
		{
			if (CommandCmp(t, 2, "on"))
			{
				Hack::Movement::FreezeZ(true);
				vout(out, "Fixed local player's Z coordinate.");
			}

			else if (CommandCmp(t, 2, "off"))
			{
				Hack::Movement::FreezeZ(false);
				vout(out, "Unfixed local player's Z coordinate.");
			}
		}

		// No fall damage hack.
		else if (CommandCmp(t, 1, "fall"))
		{
			if (CommandCmp(t, 2, "on"))
			{
				Hack::Movement::NoFallDamage(true);
				vout(out, "No fall damage hack enabled.");
			}

			else if (CommandCmp(t, 2, "off"))
			{
				Hack::Movement::NoFallDamage(false);
				vout(out, "No fall damage hack disabled.");
			}
		}

		// Vertical nudge hack.
		else if (CommandCmp(t, 1, "vfly"))
		{
			if (CommandCmp(t, 2, "on"))
			{
				g_VFly = true;
				AddChatMessage("VFly enabled.", LightPurpleMessage);
			}
			else if (CommandCmp(t, 2, "off"))
			{
				g_VFly = false;
				AddChatMessage("VFly disabled.", LightPurpleMessage);
			}
		}
	}

	VOID Map(OutputFunction out, vector<string> &t)
	{
		int mapId;

		if (t.size() < 2) return;
		if (CommandCmp(t, 1, "unload"))
		{
			if (t.size() > 2)
			{
				mapId = atoi(t[2].c_str());
				UnloadMap(mapId);
				vout(out, "Unloaded map %d", mapId);
			}
		}
	}
	
	VOID Desync(OutputFunction out, vector<string> &t)
	{
		static WoWPos save;

		if (CommandCmp(t, 1, "on"))
		{
			Hack::Movement::Desync(true);
			vout(out, "Movement desynchronized");

			LocalPlayer.update(LocationInfo);
			save = LocalPlayer.pos();
		}
		else if (CommandCmp(t, 1, "off"))
		{
			Hack::Movement::Desync(false);
			vout(out, "Movement synchronized");
		}
		else if (CommandCmp(t, 1, "back"))
		{
			Hack::Movement::Teleport(save);
			vout(out, "Teleported to ");
		}
	}
		
	VOID Key(OutputFunction out, vector<string> &t)
	{
		if (CommandCmp(t, 1, "enter"))
		{
			SendKey(0x0D, WM_KEYDOWN);
			SendKey(0x0D, WM_KEYUP);
		}
		else if (CommandCmp(t, 1, "down"))
		{
			SendKey(VK_DOWN, WM_KEYDOWN);
			SendKey(VK_DOWN, WM_KEYUP);
		}
		else if (CommandCmp(t, 1, "up"))
		{
			SendKey(VK_UP, WM_KEYDOWN);
			SendKey(VK_UP, WM_KEYUP);
		}
	}

	/* dalaran pond jump
	[03:47:18] Object X: 5644.207031
	[03:47:18] Object Y: 1943.431885
	[03:47:18] Object Z: 1543.338135
	[03:47:18] Waypoint: {5644.21, 1943.43, 1543.34}
	[03:47:18] Object Rotation: 4.753315
	[03:47:18] MapID, ZoneID: (571, 571)
	[03:47:18] Distance to player: 0.000000
	*/
	VOID Relogger(OutputFunction out, vector<string> &t)
	{
		if (CommandCmp(t, 1, "on"))
		{
			if (g_relogger == -1)
			{
				vout(out, "No account and password set for relogger");
			}
			else
			{
				g_relogger = 1;
				vout(out, "Relogger turned on for account %s with password %s", g_lastAccount, g_lastPass);
			}
		}
		else if (CommandCmp(t, 1, "off"))
		{
			g_relogger = 0;
			vout(out, "Relogger turned off");
		}
	}

#define SILVERWING_FLAG 0x00001718
#define WARSONG_FLAG 0x00001719

	VOID flagDelegate()
	{
		Thread *current = Thread::GetCurrent();
		DWORD   flagId;
		CObject war, sw;

		if (current == NULL) return;

		// Allow outside threads to distinguish between bot threads.
		current->setTask(Thread::eTask::FlagGrab);

		while (current->running() && WoW::InGame())
		{
			// Find either flag on the map.
			// 
			war.update(GetObjectByDisplayID(WARSONG_FLAG));
			sw.update(GetObjectByDisplayID(SILVERWING_FLAG));

			// Update silverwing flag and test its distance.
			if (war.isValid())
			{
				war.update(BaseObjectInfo | GameObjectInfo | LocationInfo);

				// If the flag is close enough, grab it.
				if (war.distance() < 7)
					war.interact();
			}

			// Update silverwing flag and test its distance.
			if (sw.isValid())
			{
				sw.update(BaseObjectInfo | GameObjectInfo | LocationInfo);

				// If the flag is close enough, grab it.
				if (sw.distance() < 7)
					sw.interact();
			}

			Sleep(100);
		}

		// Exit thread.
		current->DeleteCurrent();
	}

	VOID Flag(OutputFunction out, vector<string> &t)
	{
		Thread *find = NULL;

		if (t.size() < 2) return;

		find = Thread::FindType(Thread::eType::Bot);
		if (CommandCmp(t, 1, "on"))
		{
			// Call flag grabber
			if (!find)
			{
				Thread::Create(flagDelegate, NULL, Thread::eType::Bot, Thread::ePriority::Medium);
				vout(out, "Warsong Gulch flag grabber activated.");
			}

		}
		else if (CommandCmp(t, 1, "off"))
		{
			if (find)
			{
				if (find->task() == Thread::eTask::FlagGrab)
				{
					find->stop();
					vout(out, "Warsong Gulch flag grabber stopped.");
				}
			}
		}
	}

	VOID SummonDelegate()
	{
		Thread *current = Thread::GetCurrent();
		DWORD   flagId;
		CGameObject ss;

		if (current == NULL) return;

		// Allow outside threads to distinguish between bot threads.
		current->setTask(Thread::eTask::SSClick);

		while (current->running() && WoW::InGame())
		{
			// Find either flag on the map.
			ss.update(GetObjectByDisplayID(0x052F));

			// Update summoning stone and test its distance.
			if (ss.isValid())
			{
				ss.update(GameObjectInfo | LocationInfo);

				// If the flag is close enough, grab it.
				if (!LocalPlayer.owns(ss) && ss.distance() < 7)
					ss.interact();
			}

			Sleep(100);
		}

		// Exit thread.
		current->DeleteCurrent();
	}

	VOID Summon(OutputFunction out, vector<string> &t)
	{
		Thread *find = NULL;

		if (t.size() < 2) return;

		find = Thread::FindType(Thread::eType::Bot);
		if (CommandCmp(t, 1, "on"))
		{
			// Call flag grabber
			if (!find)
			{
				Thread::Create(SummonDelegate, NULL, Thread::eType::Bot, Thread::ePriority::Medium);
				vout(out, "Summoning portal clicker activated.");
			}

		}
		else if (CommandCmp(t, 1, "off"))
		{
			if (find)
			{
				if (find->task() == Thread::eTask::SSClick)
				{
					find->stop();
					vout(out, "Summoning portal clicker stopped.");
				}
			}
		}
	}

	VOID Log(OutputFunction out, vector<string> &t)
	{
		if (t.size() < 2) return;

		if (CommandCmp(t, 1, "clear"))
		{
			SetWindowText(GetDlgItem(g_hwMainWindow, IDC_MAIN_TAB_EDIT_LOG), "");
			vout(out, "Dinner GUI log cleared");
		}
	}

	VOID To_me(OutputFunction out, vector<string> &t)
	{
		CUnit unit;

		/*if (CommandCmp(t, 1, "target"))
		{*/
		unit.update(GetCurrentTargetGUID(), NameInfo | LocationInfo);

		if (unit.isValid())
		{
			unit.teleport();
			vout(out, "Teleported %s to local player", unit.name());
		}
		else
		{
			vout(out, "No target selected");
		}
	}

	VOID Goto(OutputFunction out, vector<string> &t)
	{
		if (t.size() < 2) return;

		Lua::DoString(
			"if Nx.Map ~= nil then "
			"hooksecurefunc(Nx.Map, \"SeT3\", "
			"function (self,typ,x1,y1,x2,y2,tex2,id,nam,kee,maI) "
			"dMapX, dMapY = self:GZP(self.MaI, x1, y1) "
			"dMapZ = self.MWI[self.MaI].Zon "
			"DinnerDank.FlagOff(1) "
			"DinnerDank.FlagOff(2) "

			"DinnerDank.FlagOn(2) "
			"SetConsoleKey(g_dconsolekey) "
			"end "
			") "
			"end"
			);

		if (CommandCmp(t, 1, "walk"))
		{
			g_mapTeleport = 1;
			vout(out, "Map teleporting set to walk mode");
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_MAPTELEPORT, true);
		}

		else if (CommandCmp(t, 1, "teleport"))
		{
			g_mapTeleport = 2;
			vout(out, "Map teleporting set to teleport mode");
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_MAPTELEPORT, true);
		}

		else if (CommandCmp(t, 1, "net"))
		{
			g_mapTeleport = 3;
			vout(out, "Map teleporting set to net mode");
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_MAPTELEPORT, true);
		}

		else if (CommandCmp(t, 1, "off"))
		{
			g_mapTeleport = 0;
			vout(out, "Map teleporting turned off");
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_MAPTELEPORT, false);
		}
	}

	VOID Dump(OutputFunction out, vector<string> &t)
	{
		if (CommandCmp(t, 1, "local"))
		{
			onDebugPlayerDumpButton();
		}
		else if (CommandCmp(t, 1, "address") || CommandCmp(t, 1, "addresses"))
		{
			Debug::DumpAddresses();
		}
		else if (CommandCmp(t, 1, "location"))
		{
			Debug::DumpLocation(GetCurrentTarget(LocationInfo));
		}
		else if (CommandCmp(t, 1, "unit"))
		{
			Debug::DumpUnitField(GetCurrentTarget(UnitFieldInfo).UnitField);
		}
		else if (CommandCmp(t, 1, "gameobject"))
		{
			Debug::DumpGameObjectField(GetCurrentTarget(GameObjectInfo).GameObjectField);
		}
		else if (CommandCmp(t, 1, "base"))
		{
			Debug::DumpBaseObject(GetCurrentTarget(BaseObjectInfo));
		}
	}

	VOID switchAreaCount(const bool state)
	{
		SetMagicCheck(IDC_MAIN_TOOLS_CHECK_INGAMEPLAYERCOUNT, state);
		Lua::vDo("%s:%s()", DinnerChat::frameName.c_str(), (state) ? "Show" : "Hide");
	}

	VOID PlayerCount(OutputFunction out, vector<string> &t)
	{
		int check;

		if (t.size() == 1) // not 0 based
		{
			check = GetMagicCheck(IDC_MAIN_TOOLS_CHECK_INGAMEPLAYERCOUNT);
			switchAreaCount(!check);
		}
		else 
		{
			if (CommandCmp(t, 1, "on")) // 0 based index.
			{
				switchAreaCount(true);
			}
			else if (CommandCmp(t, 1, "off"))
			{
				switchAreaCount(false);
			}
		}
	}

	void MorphString(OutputFunction out, vector<string> &t)
	{
		const char *source = NULL;
		const char *dest = NULL;

		const char op = t[2].c_str()[0];

		switch (op)
		{
			case '>':
				source = t[1].c_str();
				dest = t[3].c_str();
				break;

			case '<':
				dest = t[1].c_str();
				source = t[3].c_str();
				break;
		}

		vout(out, "Source: %s, Dest: %s, Operator: %c", source, dest, op);
	}

	// Morph Dinnerbot network, (pont's idea).
	// Where everyone looks how they want.
	VOID Morph(OutputFunction out, vector<string> &t)
	{
		uint displayId = 0;

		// Morph expression.
		/*if (t.size() == 4)
			MorphString(out, t);*/

		if (t.size() > 1)
		{
			if (isInteger(t[1]))
			{
				displayId = atoi(t[1].c_str());
				Hack::Morph::Morph(displayId);
				vout(out, "Morphed into display id %d", displayId);
			}
			else
			{
				if (CommandCmp(t, 1, "mount"))
				{
					if (isInteger(t[2]))
					{
						displayId = atoi(t[2].c_str());
						Hack::Morph::MorphMount(displayId);
						vout(out, "Morphed mount into display id %d", displayId);
					}
				}
				else if (CommandCmp(t, 1, "dismount"))
				{
					LocalPlayer.UpdateMountDisplayInfo();
				}
			}
		}
		else
		{
			Hack::Morph::Target();
			vout(out, "Morphed into target");
		}
	}

	VOID Follow(OutputFunction out, vector<string> &t)
	{
		if (!Thread::FindType(Thread::eType::Navigation))
			Thread::Create(FollowObject, NULL, Thread::eType::Navigation, Thread::ePriority::Medium);

		//DCreateThread((LPTHREAD_START_ROUTINE)FollowObject, NULL, TYPE_MOVEMENT, NULL);
	}

	VOID Interact(OutputFunction out, vector<string> &t)
	{
		CObject obj;

		obj.setUpdateFlags(LocationInfo);
		if (t.size() > 1)
		{
			obj.update(GetObjectByName((char *)t[1].c_str()));
		}
		else
		{
			obj.update(GetCurrentTargetGUID());
		}

		obj.interact();
	}

	VOID Turn(OutputFunction out, vector<string> &t)
	{
		CObject obj;

		obj.setUpdateFlags(LocationInfo);
		if (t.size() > 1)
		{
			obj.update(GetObjectByName((char *)t[1].c_str()));
		}
		else
		{
			obj.update(GetCurrentTargetGUID());
		}

		TurnToObject(obj.object());
	}

	int DinnerCallback(CObject *obj)
	{
		CHAR buffer[256];

		if (obj->isPlayer() && !obj->isLocalPlayer())
		{
			obj->update(BaseObjectInfo | NameInfo);
			sprintf_s(buffer, "if DinnerDank.TargetCallback ~= nil then DinnerDank.TargetCallback(\"%s\") end", obj->name());
			Lua::DoString(buffer);
		}

		return true;
	}

	VOID Render(OutputFunction out, vector<string> &t)
	{
		if (CommandCmp(t, 1, "object"))
			Hack::Render::Objects(!Hack::Render::hasObjects());

		else if (CommandCmp(t, 1, "player"))
			Hack::Render::Players(!Hack::Render::hasPlayers());

		else if (CommandCmp(t, 1, "world"))
			Hack::Render::World(!Hack::Render::hasWorld());

		else if (CommandCmp(t, 1, "wmo"))
		{
			Hack::Render::WMO(false);
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_WMO, !GetMagicCheck(IDC_MAIN_HACKS_CHECK_WMO));
		}

		else if (CommandCmp(t, 1, "m2"))
		{
			Hack::Render::M2(false);
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_M2, !GetMagicCheck(IDC_MAIN_HACKS_CHECK_M2));
		}

		else if (CommandCmp(t, 1, "horizon"))
		{
			Hack::Render::HorizonMountains(false);
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_HORIZON, !GetMagicCheck(IDC_MAIN_HACKS_CHECK_HORIZON));
		}

		else if (CommandCmp(t, 1, "terrain"))
		{
			Hack::Render::Terrain(false);
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_TERRAIN, !GetMagicCheck(IDC_MAIN_HACKS_CHECK_TERRAIN));
		}

		else if (CommandCmp(t, 1, "wireframe"))
		{
			Hack::Render::Wireframe(false);
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_WIREFRAME, !GetMagicCheck(IDC_MAIN_HACKS_CHECK_WIREFRAME));
		}

		else if (CommandCmp(t, 1, "water"))
		{
			Hack::Render::Water(false);
			SetMagicCheck(IDC_MAIN_HACKS_CHECK_WATER, !GetMagicCheck(IDC_MAIN_HACKS_CHECK_WATER));
		}
	}

	VOID Callback(OutputFunction out, vector<string> &t)
	{
		// TODO: distinguish between object type for callback.
		ObjectManager::ObjectCallback(DinnerCallback);
	}

	VOID Watch(OutputFunction out, vector<string> &t)
	{
		struct CObjectLink
		{
			CObject node;
			CObject *next;
		};

		if (CommandCmp(t, 1, "add"))
		{

		}
		else if (CommandCmp(t, 1, "remove"))
		{

		}
		else if (CommandCmp(t, 1, "clear"))
		{

		}
	}

	VOID Fun(OutputFunction out, vector<string> &t)
	{
		static bool piMod = false;
		static bool animateMod = false;

		if (t.size() > 1)
		{
			if (CommandCmp(t, 1, "pi"))
			{
				piMod = !piMod;
				ChangePI((piMod) ? -1 : PI);
				vout(out, "Changed WoW's value of pi.");
			}
			else if (CommandCmp(t, 1, "animate"))
			{
				animateMod = !animateMod;
				PreAnimatePatch(animateMod);
				vout(out, "Toggled animation of players.");
			}
		}
	}

	VOID Roster(OutputFunction out, vector<string> &)
	{
		g_rosterMode = !g_rosterMode;
		if (g_rosterMode)
		{
			vout(out, "Guild roster mirror enabled.");
		}
		else
		{
			vout(out, "Guild roster mirror disabled.");
			BuildRosterMembersOnlineHTML(NULL);
		}
	}

	VOID Radar(OutputFunction out, vector<string> &t)
	{
		CUnit *target = NULL;
		if (CommandCmp(t, 1, "mark"))
		{
			if (t.size() > 2)
			{
				t.erase(t.begin(), t.begin() + 2);

				std::string name = t[0];
				for (int x = 1; x < t.size(); x++)
				{
					name += " ";
					name += t[x];
				}

				Radar::MarkName(name);
				vout(out, "Marked %s on radar", name.c_str());
			}
			else
			{
				target = &LocalPlayer.getTarget();
				if (target && target->isValid())
				{
					target->update(NameInfo);
					Radar::MarkName(target->name());
					vout(out, "Marked %s on radar", target->name());
				}
			}
		}
		else if (CommandCmp(t, 1, "unmark"))
		{
			Radar::UnmarkAll();
			vout(out, "Unmarked all from radar");
		}
		else
			Thread::Create(Radar::Create, NULL);
	}

	VOID Advice(OutputFunction out, vector<string> &t)
	{

		vout(out, "ayy");
		/*
#include <iostream>
#include <fstream>
		std::ifstream source("advice.txt", std::ios::binary);
		std::ofstream destination(newName.c_str(), std::ios::binary);*/
	}

	VOID Version(OutputFunction out, vector<string> &t)
	{
		if (t.size() == 1)
			vout(out, "%s version %s", Dinner::getDankName().c_str(), Dinner::versionName.c_str());
		else
		{

		}
		// TODO
	}
		
	VOID Pont(OutputFunction out, vector<string> &t)
	{
		static bool pont = false;

		if (CommandCmp(t, 1, "on"))
		{
			vout(out, "Welcome, Pont.");
			g_updateWoWChat = true;
			g_rosterMode = true;
			g_chatLog = true;

			LogStatus(g_chatLog);

		}
		else if (CommandCmp(t, 1, "off"))
		{
			vout(out, "Pont mode disabled.");
			g_updateWoWChat = false;
			g_rosterMode = false;
			g_chatLog = false;

			LogStatus(g_chatLog);
		}
		else if (CommandCmp(t, 1, "fixchat"))
		{
			std::string &fileName = ResetChatlog();
			vout(out, "Reset chatlog: Moved chatlog.txt to %s", fileName.c_str());
		}
	}

	VOID Debug(OutputFunction out, vector<string> &)
	{
		Thread::Create(Debug::Test, NULL);
	}
};

bool Command::hasSynonym(const char *text)
{
	vector<string>::iterator it;

	// Search for text among command synonyms.
	it = find(this->commandSynonyms.begin(), this->commandSynonyms.end(), text);

	// If we're at the end of the list, the synonym was not found.
	if (it == this->commandSynonyms.end())
		return false;
	else
		return true;
}

// Code is copied because otherwise we can't pass by reference to here
bool Command::hasSynonym(std::string &text)
{
	vector<string>::iterator it;

	// Search for text among command synonyms.
	it = find(this->commandSynonyms.begin(), this->commandSynonyms.end(), text);

	// If we're at the end of the list, the synonym was not found.
	if (it == this->commandSynonyms.end())
		return false;
	else
		return true;
}

// Execute command's function
void Command::execute(OutputFunction o, std::vector<std::string> &t)
{
	if (this->function != NULL)
		this->function(o, t);
}

/*** Multiple command synonyms ***/
Command::Command(vector<string> &names, const char *help, Command::commandPrototype *f)
{
	this->helpText = help;
	this->commandSynonyms = names;
	this->function = f;
	this->_parent = NULL;
}

Command::Command(vector<const char *> &names, const char *help, Command::commandPrototype *f)
{
	// Populate vector of command synonym strings.
	for (int x = 0; x < names.size(); x++)
		this->commandSynonyms.push_back(names[x]);

	this->helpText = help;
	this->function = f;
	this->_parent = NULL;
}

/*** Single command synonym ***/
Command::Command(const char * name, const char *help, Command::commandPrototype *f)
{
	this->commandSynonyms.push_back(name);
	this->helpText = help;
	this->function = f;
	this->_parent = NULL;
}