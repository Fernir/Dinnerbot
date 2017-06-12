#pragma once

#include <string>

#include "Common\Geometry.h"
#include "Lib\poly2tri\poly2tri.h"
#include <gdiplus.h>

namespace NetEditor
{
	enum eEditMode { Contour, Hole,	};

	// Exported variables 
	extern Geometry::Net net;
	extern eEditMode editMode;

	// Variables.
	//Geometry::Polygon currentSelection;

	// Settings and initialization.
	void ChangeEditMode(eEditMode newMode);
	bool LoadNet(const std::string &filePath);
	bool SaveNet(const std::string &filePath);

	bool IsLoaded();

	// Processing.
	bool Triangulate();

	// Editing.
	bool AddPoint(p2t::Point *pt);
	bool CloseContour();
	bool CloseHole();
	void PopContour();
	void PopHole();

	// Destruction.
	void Clear();
	void FreeHoles();
	void FreeContour();

	// Interface functions
	bool SelectPoint(const Gdiplus::Point &p);
	void ShowPath(std::vector <WoWPos> &path);
	void ClearPath();
	void Undo();
};
