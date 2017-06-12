#include "NetEditor.h"

#include <vector>
#include <stack>

#include "Drawing.h"
#include "RenderObject.h"
#include "File\NetIO.h"
#include "Lib\poly2tri\poly2tri.h"
#include "Edit.h"

Geometry::Net NetEditor::net;

using namespace std;
using namespace Radar;
using namespace Geometry;

typedef vector<p2t::Point *> NetHole;
typedef vector<p2t::Point *> NetContour;

// This defines the hull of the net.
NetContour contour;

// Vector of holes.
vector<NetHole> holes;
NetHole currentHole;

// Last contour point.
WOWPOS lastContourPoint = { 0, 0, 0, 0 };

// Last hole point.
WOWPOS lastHolePoint = { 0, 0, 0, 0 };

// Net Editor mode
// Contour defines a walkable region.
// Hole defines a restricted region that is unwalkable.
NetEditor::eEditMode NetEditor::editMode = NetEditor::Contour;

stack<NetEditor::Edit> edits;

vector<RenderObject *> radarPathNodes;
vector<RenderObject *> radarPathLines;

// Undo vector

bool NetEditor::SelectPoint(const Gdiplus::Point &p);
VOID cdtToTriangles(const vector<p2t::Triangle*> &cdtTriangles,  vector<Triangle> *t);

void NetEditor::Undo()
{
	Edit *top = NULL;
	
	if (edits.empty()) return;
	top = &edits.top();

	if (!top) return;

	// Remove all associated render objects.
	for (auto &each : top->renders)
		Radar::Remove(each);

	// Deallocate memory.
	top->freePoint();
	edits.pop();

	p2t::Point *pt = edits.top().point();

	// Reconnect last point.
	switch (top->type())
	{
	case Edit::Contour:
		lastContourPoint = WoWPos(pt->x, pt->y);
		NetEditor::PopContour();
		break;

	case Edit::Hole:
		lastHolePoint = WoWPos(pt->x, pt->y);
		NetEditor::PopHole();
		break;
	}
}

bool NetEditor::AddPoint(p2t::Point *pt)
{
	WOWPOS to;

	to.X = pt->x;
	to.Y = pt->y;

	Edit addPoint;

	uint index = 0;

	switch (editMode)
	{
		// Defines a new contour point.
		case eEditMode::Contour:
			addPoint = Edit(Edit::Add, Edit::Contour, pt);

			contour.push_back(pt);
			if (lastContourPoint.X != 0 || lastContourPoint.Y != 0)
			{
				index = Radar::AddLine(lastContourPoint, to, Gdiplus::Color::Yellow);
				addPoint.renders.push_back(Radar::RenderList[index]);
			}

			index = Radar::AddWorldPoint(LocalPlayer.pos(), Gdiplus::Color::White);
			addPoint.renders.push_back(Radar::RenderList[index]);

			lastContourPoint.X = pt->x;
			lastContourPoint.Y = pt->y;
			log("Added contour point");
			edits.push(addPoint);
			return true;

		// Defines a new hole point.
		case eEditMode::Hole:
			addPoint = Edit(Edit::Add, Edit::Contour, pt);

			// Push pt onto the current hole.
			currentHole.push_back(pt);
			if (lastHolePoint.X != 0 || lastHolePoint.Y != 0)
			{
				index = Radar::AddLine(lastHolePoint, to, Gdiplus::Color::PaleVioletRed);
				addPoint.renders.push_back(Radar::RenderList[index]);
			}

			index = Radar::AddWorldPoint(LocalPlayer.pos(), Gdiplus::Color::Red);
			addPoint.renders.push_back(Radar::RenderList[index]);

			lastHolePoint.X = pt->x;
			lastHolePoint.Y = pt->y;
			log("Added hole point");
			edits.push(addPoint);
			return true;
	}

	return false;
}

void NetEditor::PopContour()
{
	contour.pop_back();
}

void NetEditor::PopHole()
{
	holes.front().pop_back();
}

bool NetEditor::CloseHole()
{
	WOWPOS first;
	p2t::Point *pt;

	uint index = 0;

	// Close the current hole and begin editing the next hole.
	if (currentHole.size() <= 2) return false;

	// Add last hole and draw line from last hole point to the first.
	pt = currentHole[0];
	first.X = pt->x;
	first.Y = pt->y;

	holes.push_back(currentHole);
	index = Radar::AddLine(lastHolePoint, first, Gdiplus::Color::PaleVioletRed);
	edits.top().renders.push_back(Radar::RenderList[index]);

	// Clear current hole and last hole point
	memset(&lastHolePoint, 0, sizeof(lastHolePoint));
	currentHole.clear();

	vlog("Closed current hole");
	return true;
}

bool NetEditor::CloseContour()
{
	WOWPOS first;
	p2t::Point *pt;

	uint index = 0;

	if (contour.size() <= 2) return false;

	pt = contour[0];
	first.X = pt->x;
	first.Y = pt->y;

	index = Radar::AddLine(lastContourPoint, first, Gdiplus::Color::Yellow);
	edits.top().renders.push_back(Radar::RenderList[index]);

	memset(&lastContourPoint, 0, sizeof(lastContourPoint));
	vlog("Closed contour");
	return true;
}

// Show path to walk on the radar.
void NetEditor::ShowPath(vector <WoWPos> &path)
{
	RenderObject *current = NULL;
	uint index = 0;

	if (!Radar::created) return;

	index = Radar::AddWorldPoint(path[0], Gdiplus::Color::OrangeRed);
	radarPathNodes.push_back(Radar::RenderList[index]);

	index = Radar::AddLine(LocalPlayer.pos(), path[0], Gdiplus::Color::Yellow);
	radarPathLines.push_back(Radar::RenderList[index]);

	for (int x = 1; x < path.size(); x++)
	{
		index = Radar::AddWorldPoint(path[x], Gdiplus::Color::OrangeRed);
		radarPathNodes.push_back(Radar::RenderList[index]);

		index = Radar::AddLine(path[x], path[x - 1], Gdiplus::Color::Yellow);
		radarPathLines.push_back(Radar::RenderList[index]);
	}
}

// Remove radar path nodes and the lines connecting them.
void NetEditor::ClearPath()
{
	if (!Radar::created) return;

	for (int x = 0; x < radarPathNodes.size(); x++)
		Radar::Remove(radarPathNodes[x]);

	for (int x = 0; x < radarPathLines.size(); x++)
		Radar::Remove(radarPathLines[x]);

	radarPathNodes.clear();
	radarPathLines.clear();
}

void NetEditor::FreeContour()
{
	// Delete each of the contour's vertices.
	for (int x = 0; x < contour.size(); x++)
	{
		if (contour[x])
			delete contour[x];
	}

	// Clear the actual vector.
	memset(&lastContourPoint, 0, sizeof(lastContourPoint));
	contour.clear();
}

void NetEditor::FreeHoles()
{
	NetHole *hole;

	// Go through each hole and delete its respective p2t points.
	for (int x = 0; x < holes.size(); x++)
	{	
		hole = &holes[x];
		for (int y = 0; y < hole->size(); y++)
		{
			if (hole->at(y))
				delete hole->at(y);
		}

		// Clear the vector AFTER all the memory has been deallocated.
		hole->clear();
	}

	// Clear the actual vector.
	holes.clear();
	currentHole.clear();
	memset(&lastHolePoint, 0, sizeof(lastHolePoint));
}

void NetEditor::Clear()
{
	net.clear();
	FreeHoles();
	FreeContour();
	Radar::Clear();

	vlog("Net cleared");
}

// Convert our region into a mesh of triangles.
bool NetEditor::Triangulate()
{
	vector<p2t::Triangle *> p2tTriangles;
	vector<Geometry::Triangle> triangles; 

	if (contour.size() <= 0) 
	{
		vlog("Error: Contour not defined");
		return false;
	}

	vlog("contour size: %d", contour.size());
	// Create new triangulation instance.
	p2t::CDT cdt(contour);

	CloseHole();
	vlog("holes: %d", holes.size());
	// Add every hole to our triangulation.
	for (int x = 0; x < holes.size(); x++)
		cdt.AddHole(holes[x]);

	// Triangulate.
	cdt.Triangulate();

	// Convert p2t triangles to Geometry triangles.
	p2tTriangles = cdt.GetTriangles();
	cdtToTriangles(p2tTriangles, &triangles);

	// Free up triangulation instance.
	//delete cdt;

	net.clear();
	
	// Import into the net and connect all adjacent triangles.
	net.setMap(LocalPlayer.mapId());
	net.import(&triangles);
	net.connectNet();

	// Add net to radar.
	Radar::Clear();
	Radar::AddNet(net, Gdiplus::Color::White);
	return true;
}

void NetEditor::ChangeEditMode(NetEditor::eEditMode newMode)
{
	editMode = newMode;
}

bool NetEditor::IsLoaded()
{
	return NetEditor::net.order() > 0 && NetEditor::net.isConnected();
}

bool NetEditor::LoadNet(const std::string &filePath)
{
	FILE *fp = fopen(filePath.c_str(), "r");

	if (fp != NULL)
	{
		// Don't have multiple copies of a net displayed on the radar.
		NetEditor::Clear();
		fclose(fp);
	}

	// Import triangles from the given file.
	if (NetIO::ImportNet(net, filePath) > 0)
	{
		// Connect triangles and add net to radar
		net.connectNet();
		Radar::AddNet(net, Gdiplus::Color::White);
	}


	return true;
}

bool NetEditor::SaveNet(const std::string &filePath)
{
	bool result = false;
	net.setMap(LocalPlayer.mapId());
	result = NetIO::OutputNet(net, filePath);

	if (result)
	{
		vlog("Saved %d triangle net to \"%s\"", net.order(), filePath.c_str());
		return true;
	}

	vlog("Error writing to file \"%s\"", filePath.c_str());
	return false;
}

/*
bool NetEditor::CreateNet()
{

}*/

// Utility function to convert between different libraries definitions of Triangle.
VOID cdtToTriangles(const vector<p2t::Triangle*> &cdtTriangles,  vector<Triangle> *t)
{
	Vec2f v1, v2, v3;
	p2t::Point *pt;
		
	for (int x = 0; x < cdtTriangles.size(); x++)
	{
		pt = cdtTriangles[x]->GetPoint(0);
		v1.x = pt->x;
		v1.y = pt->y;

		pt = cdtTriangles[x]->GetPoint(1);
		v2.x = pt->x;
		v2.y = pt->y;

		pt = cdtTriangles[x]->GetPoint(2);
		v3.x = pt->x;
		v3.y = pt->y;

		t->push_back(Triangle(v1, v2, v3));
	}
}