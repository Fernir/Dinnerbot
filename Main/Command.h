#pragma once

#include <vector>
#include <string>

#include <Common\Common.h>
#include <Utilities\Utilities.h>

class Command
{
private:
	Command *_parent;

public: 
	typedef void commandPrototype(OutputFunction out, std::vector<std::string> &);

	std::string helpText;
	std::vector<std::string> commandSynonyms;
	commandPrototype *function;

	bool hasSynonym(const char *text);
	bool hasSynonym(std::string &text);
	void execute(OutputFunction, std::vector<std::string> &parameters);

	Command(std::vector<const char *> &names, const char *help, Command::commandPrototype *f);
	Command(std::vector<std::string> &names, const char *help, Command::commandPrototype *f);
	Command(const char *name, const char *help, Command::commandPrototype *f);
	~Command();

};

inline bool operator ==(Command &lhs, const char *rhs)
{
	return lhs.hasSynonym(rhs);
}

inline bool operator ==(const char *lhs, Command &rhs)
{
	return rhs.hasSynonym(lhs);
}

inline bool operator ==(std::string *lhs, Command &rhs)
{
	return rhs.hasSynonym(*lhs);
}

inline bool operator ==(Command &lhs, std::string *rhs)
{
	return lhs.hasSynonym(*rhs);
}

namespace Commands
{
	typedef struct
	{
		std::vector<const char *> names;
		const char *help;
		Command::commandPrototype *f;

	} ArgList;

	extern std::vector<std::string> commandHistory;
	extern std::vector<Command *>   commandList;
	extern int commandIndex;

	bool handler(OutputFunction out, const std::string &buffer);
	bool handler(const std::string &buffer);
	
	VOID CD(OutputFunction out, std::vector<std::string> &t);
	VOID Walk(OutputFunction out, std::vector<std::string> &t);
	VOID Chat(OutputFunction out, std::vector<std::string> &t);
	VOID Pont(OutputFunction out, std::vector<std::string> &t);
	VOID Help(OutputFunction out, std::vector<std::string> &t);
	VOID Hide(OutputFunction out, std::vector<std::string> &t);
	VOID Radar(OutputFunction out, std::vector<std::string> &t);
	VOID Debug(OutputFunction out, std::vector<std::string> &t);
	VOID Logout(OutputFunction out, std::vector<std::string> &t);
	VOID Roster(OutputFunction out, std::vector<std::string> &t);
	VOID LuaCVar(OutputFunction out, std::vector<std::string> &t);
	VOID Gandalf(OutputFunction out, std::vector<std::string> &t);
	VOID Chatlog(OutputFunction out, std::vector<std::string> &t);
	VOID StopBot(OutputFunction out, std::vector<std::string> &t);
	VOID Verbose(OutputFunction out, std::vector<std::string> &t);
	VOID Multibox(OutputFunction out, std::vector<std::string> &t);
	VOID Teleport(OutputFunction out, std::vector<std::string> &t);
	VOID MeTarget(OutputFunction out, std::vector<std::string> &t);

	VOID Log(OutputFunction out, std::vector<std::string> &t);
	VOID Fun(OutputFunction out, std::vector<std::string> &t);
	VOID Map(OutputFunction out, std::vector<std::string> &t);
	VOID Key(OutputFunction out, std::vector<std::string> &t); 
	VOID Fog(OutputFunction out, std::vector<std::string> &t);
	VOID Cast(OutputFunction out, std::vector<std::string> &t);
	VOID Goto(OutputFunction out, std::vector<std::string> &t);
	VOID Dump(OutputFunction out, std::vector<std::string> &t);
	VOID Turn(OutputFunction out, std::vector<std::string> &t);
	VOID Loot(OutputFunction out, std::vector<std::string> &t);
	VOID Hack(OutputFunction out, std::vector<std::string> &t);
	VOID Flag(OutputFunction out, std::vector<std::string> &t);
	VOID Watch(OutputFunction out, std::vector<std::string> &t);
	VOID Morph(OutputFunction out, std::vector<std::string> &t);
	VOID To_me(OutputFunction out, std::vector<std::string> &t);
	VOID Summon(OutputFunction out, std::vector<std::string> &t);
	VOID Desync(OutputFunction out, std::vector<std::string> &t);
	VOID Follow(OutputFunction out, std::vector<std::string> &t);
	VOID Render(OutputFunction out, std::vector<std::string> &t);
	VOID Warden(OutputFunction out, std::vector<std::string> &t);
	VOID LuaGet(OutputFunction out, std::vector<std::string> &t);
	VOID Whisper(OutputFunction out, std::vector<std::string> &t);
	VOID Version(OutputFunction out, std::vector<std::string> &t);
	VOID Callback(OutputFunction out, std::vector<std::string> &t);
	VOID Lua(OutputFunction out, std::vector<std::string> &tokens);
	VOID Relogger(OutputFunction out, std::vector<std::string> &t);
	VOID Interact(OutputFunction out, std::vector<std::string> &t);
	VOID Collision(OutputFunction out, std::vector<std::string> &t);
	VOID MoveSpicy(OutputFunction out, std::vector<std::string> &t);
	VOID PlayerCount(OutputFunction out, std::vector<std::string> &t);
	VOID Target(OutputFunction out, std::vector<std::string> &tokens);

	VOID NetEditor(OutputFunction out, std::vector<std::string> &tokens);
	VOID ObjectSearch(OutputFunction out, std::vector<std::string> &t);
	VOID FakeMsg(OutputFunction out, std::vector<std::string> &tokens);
	VOID CharacterSelect(OutputFunction out, std::vector<std::string> &t);

	VOID SendParty(OutputFunction out, std::vector<std::string> &tokens);
	VOID SendGuild(OutputFunction out, std::vector<std::string> &tokens);
	VOID SendWhisper(OutputFunction out, std::vector<std::string> &tokens);
	VOID SendChatMessage(OutputFunction out, std::vector<std::string> &tokens, CONST CHAR *msgType);
};