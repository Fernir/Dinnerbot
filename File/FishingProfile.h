#pragma once

#include <Main\Structures.h>

#include <string>
#include <vector>

class FishingProfile
{
public:
	std::string name;
	std::string zoneName;

	std::vector<WOWPOS>      spots;
	std::vector<std::string> poolNames;

	bool isLoaded() { return this->_loaded; }
	void clear();

	int mapID;
	int zoneID;

	bool loadProfile(const std::string &fileName);
	bool saveProfile(std::string fileName);

	FishingProfile() {}
	FishingProfile(const std::string &fileName);
	~FishingProfile() {}

private:
	bool _loaded;
};