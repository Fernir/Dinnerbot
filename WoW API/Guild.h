#pragma once
#include <vector>

#include "GuildEntry.h"

using namespace std;

class Guild
{
private:

public:
	// variables
	int total;
	int online;

	string name;
		
	vector<GuildEntry> roster;
	
	// Construction
	Guild(string guildName);
	~Guild(void);
	
	// statics
	static bool UpdateGuildRoster();

	// Methods

	bool CompareIngameRoster();

	int getNumGuildMembers();
	int getNumOnlineGuildMembers();


	void addMember(GuildEntry member);
	void addEntry(int index);
	void buildRoster();
	void updateInfo();
	void clear();
};

