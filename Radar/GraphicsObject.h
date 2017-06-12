#pragma once
#include <Windows.h>
	
#include <objidl.h>
#include <gdiplus.h>

#include "Geometry\Polygon.h"
#include "Geometry\triangle.h"

#define PT2WPT(pt) {pt.X, pt.Y}
#define VECT2PT(v) Gdiplus::Point(v.x, v.y)
#define VECT2WPT(v) {v.x, v.y}

class GraphicsObject
{
public:
	// GDI graphics objects.
	Gdiplus::Graphics	*g;
	Gdiplus::Pen		*pen;
	Gdiplus::SolidBrush	*brush;

	DWORD type;
	DWORD base;

	// Dimensions
	int height;
	int width;

	// Source
	Geometry::Vec2f source;
	
	Gdiplus::GraphicsPath path;
	Gdiplus::Region *bound;

	GraphicsObject(Gdiplus::Graphics *gr, Gdiplus::Pen *p, Gdiplus::SolidBrush *b);
	GraphicsObject(HDC hdc, Gdiplus::Color color);
	GraphicsObject(void);
	~GraphicsObject(void);

	int FillEllipse();
	int DrawEllipse();

	int DrawHull(Geometry::Polygon *poly);

	int DrawCross(Gdiplus::Point &center, int length);
	int DrawPath();
	int DrawTriangle();

	int DrawLine(Geometry::Vec2f &v);
	int DrawLine(Gdiplus::Point *to);
	int DrawLine(int to_x, int to_y);

	int DrawArrow(Gdiplus::Point &center, float rot, int width, int length, bool fill);
	int DrawArrow(Gdiplus::Point *to);

	int ConnectLine(GraphicsObject *other);

	BOOL CreateRegionAroundSource();
	BOOL CreateRegionAroundLine(Gdiplus::Point to, Gdiplus::Point test);

	BOOL FillRegion();
	BOOL FillRegion(POINT vertices[], int size);
};

