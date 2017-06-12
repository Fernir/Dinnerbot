#include "NetIO.h"
#include "Lib\pugixml\pugixml.hpp"
#include "Utilities\Utilities.h"

using namespace Geometry;

// Returns number of triangles retrieved from file.
int NetIO::ImportNet(Geometry::Net &n, const std::string &fileName)
{
	float x, y;
	Vec2f p1, p2, p3;

	char *buffer = NULL;

	pugi::xml_document doc;

	// Load xml file with data.
	pugi::xml_parse_result result = doc.load_file(fileName.c_str());

	// Load result
	if (result)
		vlog("NetIO::ImportNet - %s loaded", fileName.c_str());
	else
	{
		velog("NetIO::ImportNet - %s", result.description());
		return 0;
	}

	// Number of triangles under Net node.
	int num = 0;

	// Retrieve mapId from file.
	buffer = (char *)doc.child("Net").child_value("Map");
	if (isinteger(buffer))
		n.setMap(atoi(buffer));

	// Iterate through the children of Mesh, which should be triangles.
	for (pugi::xml_node tri = doc.child("Net").first_child(); tri; num++, tri = tri.next_sibling("Triangle")) 
	{
		// Add the coordinates of the triangle's vertices.
		sscanf_s(tri.child_value("P1"), "%f %f", &x, &y);
		p1.set(Vec2f(x, y));
		
		sscanf_s(tri.child_value("P2"), "%f %f", &x, &y);
		p2.set(Vec2f(x, y));
		
		sscanf_s(tri.child_value("P3"), "%f %f", &x, &y);
		p3.set(Vec2f(x, y));

		// Load the newly retrieved triangle into memory.
		n.add(Triangle(p1, p2, p3));
	}

	vlog("Imported %d triangles on map %d", num, n.getMapId());
	return num;
}

bool NetIO::OutputNet(Geometry::Net &n, const std::string &fileName)
{
	char buffer[256];

	// Create an empty XML document.
	pugi::xml_document doc;

	// Create root Mesh node.
	pugi::xml_node meshNode = doc.append_child("Net");
	pugi::xml_node mapNode = meshNode.append_child("Map");
	pugi::xml_node triNode;

	std::string val;
	
	// Set net map Id.
	sprintf_s(buffer, "%d", n.getMapId());
	mapNode.append_child(pugi::node_pcdata).set_value(buffer);

	for (int x = 0; x < n.order(); x++)
	{
		Triangle *tri = &n.getTriangle(x);
		
		// For the current triangle, create a new Triangle child node.
		triNode = meshNode.append_child("Triangle");

		// Create the vertex nodes and append them as children to the current triangle node.
		sprintf_s(buffer, "%f %f", tri->p1.x, tri->p1.y);
		triNode.append_child("P1").append_child(pugi::node_pcdata).set_value(buffer);

		sprintf_s(buffer, "%f %f", tri->p2.x, tri->p2.y);
		triNode.append_child("P2").append_child(pugi::node_pcdata).set_value(buffer);

		sprintf_s(buffer, "%f %f", tri->p3.x, tri->p3.y);
		triNode.append_child("P3").append_child(pugi::node_pcdata).set_value(buffer);
	}

	// Save newly created XML to fileName.
	doc.save_file(fileName.c_str());
	return true;
}

