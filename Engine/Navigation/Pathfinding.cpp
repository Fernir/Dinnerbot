#include "Pathfinding.h"
#include "Main\Main.h"
#include "Utilities\Utilities.h"
#include <Geometry\ConvexCone.h>

using namespace Engine;
using namespace Navigation;
using namespace Geometry;
using namespace std;

vector<Triangle *>::iterator lowestFcost(vector<Triangle *> *t)
{
	float minCost = 99999;

	Triangle *current = NULL;
	vector<Triangle *>::iterator result;

	for (vector<Triangle *>::iterator it = t->begin(); it != t->end(); ++it)
	{
		current = *it;
		if (current->Fcost < minCost)
		{
			minCost = current->Fcost;
			result = it;
		}
	}

	return result;
}

uint Pathfinding::getPathNodeSize()
{
	return this->_path.size();
}

// Eventually replace this with path funnelling.
vector<WoWPos> Pathfinding::getPath()
{
	Triangle *current = NULL;
	Triangle *next    = NULL;
	Vec2f c;

	ConvexCone currentCone;
	vector<ConvexCone> portals;
	vector<WoWPos> path;

	while (!this->_path.empty())
	{
		current = _path.top();
		_path.pop();

		if (_path.empty()) break;

		next = _path.top();
		if (next == NULL) break;

		vector<Vec2f> &commonVertices = current->getCommonVertices(next);

		// Get v1 and v2 for ConvexCone
		// We use the cones for funnelling a smoother path
		if (commonVertices.size() > 0)
		{
			currentCone.v1 = commonVertices[0];
			currentCone.v2 = commonVertices[1];
		}

		// Midpoint of edge of adjacent triangles
		c = current->edgeMidpointBetween(next);

		// Path along apex of cones is the original kinked path
		currentCone.apex = c;
		portals.push_back(currentCone);
	}

	currentCone.apex = _end;
	currentCone.v1 = Vec2f(0, 0);
	currentCone.v2 = currentCone.v2;
	portals.push_back(currentCone);

	return this->smoothPath(portals);
}

/*
void Pathfinding::ayy(Vec2f start, Vec2f end)
{
	// Abort if list is empty.
	if (_path.size() <= 0)
	{
		return; //new List<Vector2>() { start };
	}
 
	// Return immediately if list only has one entry.
	if (_path.size() == 1)
	{
		return; //new List<Vector2>() { end };
	}

	vector<WOWPOS> path;

	// _path.size() + 1
	Vec2f leftVertices[];
	Vec2f rightVertices[];
	Vec2f apex = start;

	int left = 1;
	int right = 1;
	
	for (int i = 0; i < _path.size() - 1; i++)
	{
		for (int j = 0; j < nodeList[i].LocalNodes.Length; j++)
		{
			if (net->net[i].
		}

	}


	List<Vector2> path = new List<Vector2>() { start };
	Vector2[] leftVertices = new Vector2[nodeList.Count + 1];
	Vector2[] rightVertices = new Vector2[nodeList.Count + 1];
	Vector2 apex = start;
	int left = 1;
	int right = 1;
 
	// Initialise portal vertices.
	for (int i = 0; i < nodeList.Count - 1; i++)
	{
		for (int j = 0; j < nodeList[i].LocalNodes.Length; j++)
		{
			if (nodeList[i].LocalNodes[j] == nodeList[i + 1])
			{
				int k = j + 1 >= nodeList[i].LocalNodes.Length ? 0 : j + 1;
 
				leftVertices[i + 1] = ((NavTriangle)nodeList[i]).Points[j];
				rightVertices[i + 1] = ((NavTriangle)nodeList[i]).Points[k];
				break;
			}
		}
	}
 
	// Initialise portal vertices first point.
	for (int j = 0; j < ((NavTriangle)nodeList[0]).Points.Length; j++)
	{
		if (((NavTriangle)nodeList[0]).Points[j] != leftVertices[1]
			&& ((NavTriangle)nodeList[0]).Points[j] != rightVertices[1])
		{
			leftVertices[0] = ((NavTriangle)nodeList[0]).Points[j];
			rightVertices[0] = ((NavTriangle)nodeList[0]).Points[j];
		}
	}
 
	// Initialise portal vertices last point.
	for (int j = 0;
		j < ((NavTriangle)nodeList[nodeList.Count - 1]).Points.Length; j++)
	{
		if (((NavTriangle)nodeList[nodeList.Count - 1]).Points[j]
			!= leftVertices[nodeList.Count - 1]
			&& ((NavTriangle)nodeList[nodeList.Count - 1]).Points[j]
			!= rightVertices[nodeList.Count - 1])
		{
			leftVertices[nodeList.Count]
				= ((NavTriangle)nodeList[nodeList.Count - 1]).Points[j];
			rightVertices[nodeList.Count]
				= ((NavTriangle)nodeList[nodeList.Count - 1]).Points[j];
		}
	}// Step through channel.
	for (int i = 2; i <= nodeList.Count - 1; i++)
	{
		// If new left vertex is different, process.
		if (leftVertices[i] != leftVertices[left]
			&& i > left)
		{
			Vector2 newSide = leftVertices[i] - apex;
 
			// If new side does not widen funnel, update.
			if (GeometryHelper.VectorSign(newSide,
					leftVertices[left] - apex) > 0)
			{
				// If new side crosses other side, update apex.
				if (GeometryHelper.VectorSign(newSide,
					rightVertices[right] - apex) > 0)
				{
					// Find next vertex.
					for (int j = next; j <= nodeList.Count; j++)
					{
						if (rightVertices[j] != rightVertices[next])
						{
							next = j;
							break;
						}
					}
 
					path.Add(rightVertices[right]);
					apex = rightVertices[right];
					right = next;
				}
				else
				{
					left = i;
				}
			}
		}
 
		// If new right vertex is different, process.
  
}
*/

bool vectorContains(vector<Triangle *> *t, Triangle *test)
{
	return std::find(t->begin(), t->end(), test) != t->end();
}

bool Pathfinding::findPath(WOWPOS start, WOWPOS end)
{
	Vec2f s(start.X, start.Y), e(end.X, end.Y);

	return this->findPath(s, e);
}

int getMaxVector(vector<ConvexCone> &path, int startIndex)
{
	ConvexCone portal = path[startIndex];

	int x = startIndex + 1;

	for (; x < path.size(); x++)
	{
		// If the next cone is not a subcone of the portal
		// there is a point along the path outside of the net
		if (!portal.contains(path[x]))
			break;
	}

	if (x == startIndex + 1)
		return x;

	return x - 1;
}

vector<WoWPos> Pathfinding::smoothPath(vector<ConvexCone> &path)
{
	vector<WoWPos> smoothedPath;
	WoWPos waypoint;

	int x = 0;

	// We need a more complicated algorithm to handle holes in the net
	// Ray cast to vector and determine if any point of the line is outside the net
	vlog("Size: %d", path.size());
	while (x < path.size() - 1)
	{
		// Start of funnel
		vlog("Start %d", x);
		if (x == 0)
		{
			waypoint.x = _start.x;
			waypoint.y = _start.y;
		}
		else
		{
			waypoint.x = path[x].apex.x;
			waypoint.y = path[x].apex.y;
		}

		waypoint.z = LocalPlayer.pos().Z;
		smoothedPath.push_back(waypoint);

		// Get end of funnel
		x = getMaxVector(path, x);
		waypoint.x = path[x].apex.x;
		waypoint.y = path[x].apex.y;
		waypoint.z = LocalPlayer.pos().Z;
		vlog("End %d", x);

		smoothedPath.push_back(waypoint);
	}

	return smoothedPath;
}

// A* algorithm
bool Pathfinding::findPath(Vec2f start, Vec2f end)
{
	// Triangles that contain the start and end points, respectively.
	Triangle *endT = NULL;
	Triangle *startT = NULL;
	Triangle *current = NULL;
	Triangle *neighbor = NULL;

	vector<Triangle *>::iterator min;

	this->pathFound = false;
	if (this->net == NULL) return false;
	if (!this->net->isConnected()) return false;

	// First of all, the given net has to contain both points for us to be able
	// to find a path between them.
	startT = this->net->containedIn(&start);
	endT = this->net->containedIn(&end);

	if (!startT) return false;
	if (!endT) return false;

	_start = start;
	_end = end;

	// Initialize the open list and start triangle.
	startT->Fcost = 0;
	startT->Gcost = 0;
	startT->Hcost = 0;
	openList.push_back(startT);

	// While our list of open nodes is nonempty.
	while (openList.size() > 0)
	{
		// Current triangle is the one with the lowest Fcost
		min = lowestFcost(&openList);
		current = *min;

		closedList.push_back(current);

		// If the current triangle is the same as the end triangle, we are done.
		// BUG: stack overflow due to current->other->current->other->... when parents are set.
		if (current == endT)
		{
			while (current != NULL)
			{
				_path.push(current);
				current = current->parent;
			}

			this->pathFound = true;
			this->net->clearParents();
			return true;
		}

		openList.erase(min);
		// For every neighbor triangle
		for (int x = 0; x < current->neighbors.size(); x++)
		{
			neighbor = current->neighbors[x];

			// If neighbor is on the closed list, continue to the next neighbor.
			if (vectorContains(&closedList, neighbor)) continue;

			// If our current neighbor is not on the open list
			if (!vectorContains(&openList, neighbor))
			{
				// Add it to the open List
				openList.push_back(neighbor);

				// Set neighbor's parent to us
				neighbor->setParent(current);
			
				// Hcost: Distance between neighbor and end triangle;
				neighbor->Hcost = endT->centroidDistance(neighbor);

				// Gcost: Current Gcost + distance between current and neighbor
				neighbor->Gcost = current->Gcost + neighbor->centroidDistance(current);
				neighbor->Fcost = neighbor->Gcost + neighbor->Hcost;
			} 
			else
			{
				if (current->Gcost + neighbor->Gcost < neighbor->Gcost)
				{
					// Set neighbor's parent to us
					neighbor->setParent(current);
			
					// Hcost: Distance between neighbor and end triangle;
					neighbor->Hcost = endT->centroidDistance(neighbor);

					// Gcost: Current Gcost + distance between current and neighbor
					neighbor->Gcost = current->Gcost + neighbor->centroidDistance(current);
					neighbor->Fcost = neighbor->Gcost + neighbor->Hcost;
				}
			}
		}
	}

	return false;
}


void Pathfinding::setNet(Geometry::Net *n)
{
	this->net = n;
}

void Pathfinding::clearNet()
{
	this->net = NULL;
}

Pathfinding::Pathfinding(Geometry::Net *n)
{
	this->net = n;
	pathFound = false;
}

Pathfinding::Pathfinding(void)
{
	this->net = NULL;
	pathFound = false;
}

Pathfinding::~Pathfinding(void)
{
}
