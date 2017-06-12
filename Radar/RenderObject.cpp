#include "RenderObject.h"

using namespace Radar;

void RenderObject::addRender(const POINT &p)
{
	this->_polygon.transformed.push_back(p);
}

void RenderObject::addWorld(const Geometry::Triangle &t)
{
	this->_polygon.addTriangle(t);
}

void RenderObject::addWorld(const WOWPOS &p)
{
	this->addWorld(Geometry::Vec2f(p.X, p.Y));
}

void RenderObject::addWorld(const Geometry::Vec2f &p)
{
	this->_polygon.vertices.push_back(p);
}

void RenderObject::setColor(Gdiplus::Color color)
{
	this->_color = color;
}

void RenderObject::setType(eType type)
{
	this->_type = type;
}

float RenderObject::area()
{
	return this->_polygon.area();
}

int RenderObject::size()
{
	return this->_polygon.order();
}

std::vector<POINT> &RenderObject::renderBoundary()
{
	return this->_polygon.transformed;
}
std::vector<Geometry::Vec2f> &RenderObject::worldBoundary()
{
	return this->_polygon.vertices;
}

void RenderObject::clearWorld()
{
	this->_polygon.vertices.clear();
}

void RenderObject::clearRender()
{
	this->_polygon.transformed.clear();
}

void RenderObject::clear()
{
	this->clearRender();
	this->clearWorld();
}

Gdiplus::Color RenderObject::color()
{
	return this->_color;
}

RenderObject::eType RenderObject::type()
{
	return this->_type;
}

Geometry::Polygon &RenderObject::polygon()
{
	return this->_polygon;
}

RenderObject::RenderObject(eType type)
{
	this->_type = type;
}

RenderObject::~RenderObject()
{
}
