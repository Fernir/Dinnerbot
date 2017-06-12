#pragma once

#include <string>

#include "..\Common\Common.h"

class GuildEntry
{
private: 

public:
	// Variables
	int entryIndex; // -1 - unknown
					//    - otherwise the entry
	int rankIndex;
	int level;
	int status; // -1 - unknown
				//  0 - invite pending, 
				//  1 - offline
				//  2 - online

	std::string name;
	std::string rank;
	std::string className;
	std::string zone;
	
	// Initialization
	GuildEntry(std::string entryText);
	GuildEntry(int entry);

	~GuildEntry(void);

	// Static
	static DWORD GetRosterAddress(void);

	// Methods
	DWORD getEntryAddress();

	void updateName();
	void updateZone();
	void updateClass();
};

