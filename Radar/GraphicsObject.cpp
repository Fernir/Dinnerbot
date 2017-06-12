#include "GraphicsObject.h"
#include "Common\Geometry.h"
#include <Main\Constants.h>

using namespace Gdiplus;
using namespace Geometry;

int GraphicsObject::DrawLine(int to_x, int to_y)
{
	if (!g) return false;
	if (!pen) return false;
	return g->DrawLine(pen, (int)source.x, (int)source.y, to_x, to_y);
}

BOOL GraphicsObject::FillRegion()
{
	if (!g) return false;
	if (!bound) return false;
	if (!brush) return false;

	return g->FillRegion(brush, bound);
}

BOOL GraphicsObject::FillRegion(POINT vertices[], int size)
{
	HRGN poly;

	if (!g) return false;
	// Create a region from the polygon.
	poly = CreatePolygonRgn(vertices, size, ALTERNATE);
	bound = Region::FromHRGN(poly);

	DeleteObject(poly);
	if (bound) g->FillRegion(brush, bound);

	// Hit test the region with the given point.
	return bound != NULL;
}

BOOL GraphicsObject::CreateRegionAroundSource()
{
	POINT points[4] = {PT2WPT(VECT2PT(source)), PT2WPT(VECT2PT(source)), PT2WPT(VECT2PT(source)), PT2WPT(VECT2PT(source))};
	HRGN poly;

	if (!g) return false;
	// Create rectangle around point.
	points[0].x -= this->width / 2;
	points[0].y += this->height / 2;

	points[1].x += this->width / 2;
	points[1].y += this->height / 2;

	points[2].x += this->width / 2;
	points[2].y -= this->height / 2;

	points[3].x -= this->width / 2;
	points[3].y -= this->height / 2;

	poly = CreatePolygonRgn(points, 4, ALTERNATE);
	bound = Region::FromHRGN(poly);
	DeleteObject(poly);

	if (bound) g->FillRegion(brush, bound);
	return bound != NULL;
}

BOOL GraphicsObject::CreateRegionAroundLine(Point to, Point test)
{
	POINT points[4] = {PT2WPT(to), PT2WPT(to), PT2WPT(VECT2PT(source)), PT2WPT(VECT2PT(source))};
	HRGN poly;

	if (!g) return false;
	points[0].x -= 5;
	points[1].x += 5;
	points[2].x += 5;
	points[3].x -= 5;

	// Create a region from the polygon.
	poly = CreatePolygonRgn(points, 4, ALTERNATE);
	this->bound = Region::FromHRGN(poly);

	// Delete HRGN, otherwise a memory leak occurs.
	DeleteObject(poly);

	// Fill the region.
	if (bound) g->FillRegion(brush, bound);

	//DeleteObject(poly);
	return 1;
}

int GraphicsObject::DrawLine(Gdiplus::Point *to)
{
	if (!g) return false;
	if (!pen) return false;
	return g->DrawLine(pen, (Gdiplus::REAL)source.x, (Gdiplus::REAL)source.y, (Gdiplus::REAL)to->X, (Gdiplus::REAL)to->Y);
}

int GraphicsObject::DrawLine(Vec2f &v)
{
	if (!g) return false;
	if (!pen) return false;
	return g->DrawLine(pen, (Gdiplus::REAL)source.x, (Gdiplus::REAL)source.y, (Gdiplus::REAL)v.x, (Gdiplus::REAL)v.y);
}

#include "Utilities\Utilities.h"

int GraphicsObject::DrawCross(Gdiplus::Point &c, int length)
{
	Vec2f leftArmTail(0, 0), leftArmHead(0, 0);
	Vec2f rightArmTail(0, 0), rightArmHead(0, 0);
	Vec2f center(c.X, c.Y);

	if (!g) return false;
	if (!pen) return false;
	leftArmHead += length * Vec2f(1, 0);
	rightArmHead = leftArmHead;

	// Rotate arms into a cross
	leftArmHead.rotate(PI / 4);
	rightArmHead.rotate(-PI / 4);

	Vec2f shift(0, rightArmHead.y / 2);

	// Translate arms into X formation
	leftArmHead += shift;
	leftArmTail += shift;

	rightArmHead -= shift;
	rightArmTail -= shift;

	// Shift the arms back into the plane
	leftArmHead += center;
	leftArmTail += center;
	rightArmHead += center;
	rightArmTail += center;

	this->source = leftArmTail;
	this->DrawLine(leftArmHead);

	this->source = rightArmTail;
	return this->DrawLine(rightArmHead);
}

int GraphicsObject::DrawArrow(Gdiplus::Point &center, float rot, int width, int length, bool fill)
{
	if (!g) return false;
	if (!pen) return false;
	if (!brush) return false;

	int result = 0;

	GraphicsObject circle(g, pen, brush);
	GraphicsObject line(g, pen, brush);

	Point to;

	if (rot != 0)
	{
		line.source.x = center.X;
		line.source.y = center.Y;

		// Draw the arrow to show which direction the unit is facing.
		to.X = center.X + (int)(length * cos(-(rot + PI / 2)));
		to.Y = center.Y + (int)(length * sin(-(rot + PI / 2)));
		line.DrawArrow(&to);
	}

	// Draw the circle to show the unit.
	circle.width = 7;
	circle.height = 7;

	// This calculates the point to start drawing the circle.
	// Otherwise the circle is offset by its width and height.
	circle.source.x = center.X - (circle.width / 2);
	circle.source.y = center.Y - (circle.height / 2);
	circle.DrawEllipse();

	if (fill) circle.FillEllipse();

	return 1;
}

// Note that paths seem to be faster than drawing individual lines.
int GraphicsObject::DrawArrow(Gdiplus::Point *to)
{
	Vec2f dest(to->X, to->Y);
	Vec2f main(to->X, to->Y);
	Vec2f normal; // normal to main vector.
	Vec2f vertex1, vertex2;

	float scalingFactor = 0.11;

	if (!g) return false;
	dest.x = scalingFactor * (dest.x - source.x) + source.x;
	dest.y = scalingFactor * (dest.y - source.y) + source.y;
	//dest = 0.scalingFactor * (dest - source) + source;


	main.x -= source.x;
	main.y -= source.y;
	//main -= source;
	
	// Add the main vector line.
	//this->path.AddLine(VECT2PT(source), VECT2PT(dest));

	// Calculate the normal at the origin.
	normal.x = -main.y;
	normal.y = main.x;
	
	// Resize normal vector.
	normal.x = 3 * normal.normalized().x;
	normal.y = 3 * normal.normalized().y;
	//normal = 3 * normal.normalized();

	normal.x += dest.x;
	normal.y += dest.y;
	//normal = normal + dest;

	vertex1.x = normal.x;
	vertex1.y = normal.y;
	//vertex1 = normal;

	// Add the corner parts of the arrow head.
	//this->path.AddLine(VECT2PT(dest), VECT2PT(normal));

	// Calculate the negative of the normal vector.
	normal.x -= dest.x;
	normal.y -= dest.y;
	//normal -= dest;

	normal.x = -normal.x;
	normal.y = -normal.y;

	normal.x += dest.x;
	normal.y += dest.y;
	//normal += dest;
	vertex2.x = normal.x;
	vertex2.y = normal.y;
	//vertex2 = normal;

	//this->path.AddLine(VECT2PT(dest), VECT2PT(normal));
	//line.DrawLine(normal.x(), normal.y());

	// Connect the two vertices to the final endpoint.
	dest.x = 4 * (dest.x - source.x) + source.x;
	dest.y = 4 * (dest.y - source.y) + source.y;
	//dest = 4 * (dest - source) + source;

	this->path.AddLine(VECT2PT(vertex2), VECT2PT(dest));
	this->path.AddLine(VECT2PT(vertex1), VECT2PT(dest));

	//g->FillPath(brush, &path);
	return g->DrawPath(this->pen, &path);
}

int GraphicsObject::DrawHull(Geometry::Polygon *poly)
{
	int order = poly->order();

	Point from;
	Point to;

	for (int x = 0; x < order; x++)
	{
		from.X = poly->transformed[x].x;
		from.Y = poly->transformed[x].y;

		if (x == order - 1)
		{
			to.X = poly->transformed[0].x;
			to.Y = poly->transformed[0].y;
		}
		else
		{
			to.X = poly->transformed[x+1].x;
			to.Y = poly->transformed[x+1].y;
		}

		this->path.AddLine(from, to);
	}

	return this->DrawPath();
}

int GraphicsObject::DrawTriangle()
{
	Vec2f top(source.x, source.y + this->height / 2);
	Vec2f ayy(source.x, source.y + this->height / 2);

	if (!g) return false;
	return 0;
}

int GraphicsObject::DrawPath()
{
	if (!g) return false;
	if (!pen) return false;
	return g->DrawPath(this->pen, &path);
}

int GraphicsObject::DrawEllipse()
{
	if (!g) return false;
	if (!pen) return false;
	return g->DrawEllipse(this->pen, (int)source.x, (int)source.y, width, height);
}

int GraphicsObject::FillEllipse()
{
	if (!g) return false;
	if (!brush) return false;
	
	return g->FillEllipse(brush, (int)source.x, (int)source.y, width, height);
}

GraphicsObject::GraphicsObject(Graphics *gr, Pen *p, SolidBrush *b)
{
	g = gr;
	pen = p;
	brush = b;
	this->bound = NULL;
}
/*
GraphicsObject::GraphicsObject(HDC hdc, Color color)
{
	this->g = new Graphics(hdc);
	this->pen = new Pen(color);
	this->brush = new SolidBrush(color);
	this->bound = NULL;
}

GraphicsObject::GraphicsObject()
{
	this->g = NULL;
	this->pen = NULL;
	this->brush = NULL; 
	this->bound = NULL;
}
*/
GraphicsObject::~GraphicsObject(void)
{
	/*if (g)	   delete g;
	if (pen)   delete pen;
	if (brush) delete brush;*/
	if (bound) delete bound;
}
