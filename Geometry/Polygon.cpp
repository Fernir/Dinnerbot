#include "Polygon.h"

namespace Geometry
{
	bool Polygon::contains(const Vec2f &v)
	{
		int i, j, c = 0;

		// Creates ray from test point outwards and counts the number of line intersections with the polygon.
		// If the number of intersections is odd, it is inside. Otherwise, the point is outside.
		for (i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) 
		{ 
			if (((vertices.at(i).y > v.y) != (vertices.at(j).y > v.y)) &&
				(v.x < (vertices.at(j).x - vertices.at(i).x) * (v.y - vertices.at(i).y) / (vertices.at(j).y - vertices.at(i).y) + vertices.at(i).x))
				c = !c;
		}

		return c;
	}
	
	// Returns area of polygon.
	float Polygon::area()
	{
		float area = 0.0f;

		if (this->order() <= 2) return 0;
		for (int i = 0; i < this->order() - 1; ++i)
			area += vertices[i].x * vertices[i+1].y - vertices[i+1].x * vertices[i].y;

		area += vertices[order()-1].x * vertices[0].y - vertices[0].x * vertices[order()-1].y;
		area = abs(area) / 2.0f;
		return area;
	}

	void Polygon::import(const std::vector<Triangle> &ts)
	{
		for (int x = 0; x < ts.size(); x++)
		{
			this->addTriangle(ts[x]);
		}
	}

	void Polygon::addTriangle(const Triangle &t)
	{
		vertices.push_back(t.p1);
		vertices.push_back(t.p2);
		vertices.push_back(t.p3);
	}

	bool Polygon::contains(Vec2f v)
	{
		POINT vt;

		vt.x = v.x;
		vt.y = v.y;
		return contains(vt);
	}

	bool Polygon::contains(POINT v)
	{
		int i, j, c = 0;

		for (i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) 
		{ 
			if (((vertices[i].y > v.y) != (vertices[j].y > v.y)) &&
				(v.x < (vertices[j].x - vertices[i].x) * (v.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x))
				c = !c;
		}

		return c;
	} 

	void Polygon::add(Vec2f v)
	{
		Vec2f vt(v.x, v.y);
		vertices.push_back(vt);
	}

	void Polygon::add(const Vec2f &v)
	{
		vertices.push_back(v);
	}

	void Polygon::remove(const Vec2f &v)
	{
		for (int x = 0; x < this->vertices.size(); x++)
		{
			if (this->vertices.at(x) == v)
			{
				this->vertices.erase(this->vertices.begin() + x);
			}
		}
	}

	void Polygon::clear()
	{
		vertices.clear();
		transformed.clear();
	}

	void Polygon::convertVertices()
	{
		POINT temp;

		for (int x = 0; x < vertices.size(); x++)
		{
			temp.x = vertices[x].x;
			temp.y = vertices[x].y;
			transformed.push_back(temp);
		}
	}

	int Polygon::order()
	{
		return vertices.size();
	}

	Polygon::Polygon(void)
	{
		vertices.reserve(10);
	}

	Polygon::Polygon(int order)
	{
		vertices.reserve(order);
	}

	Polygon::~Polygon(void)
	{
	}
}