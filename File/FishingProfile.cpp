#include "FishingProfile.h"

#include <Utilities\Utilities.h>
#include <Lib\pugixml\pugixml.hpp>

bool FishingProfile::loadProfile(const std::string &fileName)
{
	pugi::xml_document doc;

	// Load xml file with data.
	pugi::xml_parse_result result = doc.load_file(fileName.c_str());

	WOWPOS pos;

	if (!result)
	{
		vlog("Unable to load fishing profile %s: %s", fileName.c_str(), result.description());
		return false;
	}

	vlog("Fishing profile loaded from: %s, with result: %s", fileName.c_str(), result.description());
	this->clear();

	pugi::xml_node profile = doc.child("Profile");
	pugi::xml_node spotsNode = profile.child("FishingSpots");
	pugi::xml_node poolsNode = profile.child("PoolNames");

	// Get the name of the fishing profile.
	this->name = profile.child_value("Name");

	char *buffer = NULL;
	
	// Retrieve Map id.
	buffer = (char *)profile.child("Map").child_value("Id");
	if (isinteger(buffer))
		this->mapID = atoi(buffer);

	// Retrieve zone id and name.
	this->zoneName = profile.child("Zone").child_value("Name");
	buffer = (char *)profile.child("Zone").child_value("Id");
	if (isinteger(buffer))
		this->zoneID = atoi(buffer);

	// Load all the fishing spots.
	for (pugi::xml_node spot = spotsNode.first_child(); spot; spot = spot.next_sibling("Spot"))
	{
		sscanf_s(spot.child_value(), "%f %f %f", &pos.X, &pos.Y, &pos.Z);
		this->spots.push_back(pos);
	}

	// Load all the pool names.
	for (pugi::xml_node pool = poolsNode.first_child(); pool; pool = pool.next_sibling("Pool"))
		this->poolNames.push_back(pool.child_value());

	this->_loaded = true;
	return true;
}

bool FishingProfile::saveProfile(std::string fileName)
{
	// Create an empty XML document.
	pugi::xml_document doc;

	// Write buffer.
	char buffer[512];

	// Create root Mesh node.
	pugi::xml_node profileNode	 = doc.append_child("Profile");
	pugi::xml_node nameNode		 = profileNode.append_child("Name");
	pugi::xml_node mapNode		 = profileNode.append_child("Map");
	pugi::xml_node zoneNode		 = profileNode.append_child("Zone");
	pugi::xml_node spotsNode	 = profileNode.append_child("FishingSpots");
	pugi::xml_node poolNamesNode = profileNode.append_child("PoolNames");
	pugi::xml_node spotNode;
	pugi::xml_node poolNode;

	// Set profile name.
	nameNode.append_child(pugi::node_pcdata).set_value(this->name.c_str());

	// Set profile map Id.
	sprintf_s(buffer, "%d", this->mapID);
	mapNode.append_child("Id").append_child(pugi::node_pcdata).set_value(buffer);

	// Set profile zone Id.
	sprintf_s(buffer, "%d", this->zoneID);
	zoneNode.append_child("Id").append_child(pugi::node_pcdata).set_value(buffer);
	zoneNode.append_child("Name").append_child(pugi::node_pcdata).set_value(this->zoneName.c_str());

	for (auto &entry : this->spots)
	{
		spotNode = spotsNode.append_child("Spot");
		sprintf_s(buffer, "%f %f %f", entry.X, entry.Y, entry.Z);
		spotNode.append_child(pugi::node_pcdata).set_value(buffer);
	}

	for (auto &entry : this->poolNames)
	{
		poolNode = poolNamesNode.append_child("Pool");
		poolNode.append_child(pugi::node_pcdata).set_value(entry.c_str());
	}

	return doc.save_file(fileName.c_str());
}

FishingProfile::FishingProfile(const std::string &fileName)
{
	this->_loaded = this->loadProfile(fileName);
}

void FishingProfile::clear()
{
	this->poolNames.clear();
	this->spots.clear();

	this->zoneID = 0;
	this->mapID = 0;
	this->name = "";
	this->_loaded = false;
}