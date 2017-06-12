#include "triangle.h"

#include <assert.h>
#include <math.h>
#include <vector>

using namespace Geometry;

float dist(Vec2f v, Vec2f u)
{
	return sqrtf((v.x - u.x) * (v.x - u.x) + (v.y - u.y) * (v.y - u.y));
}

Triangle::Triangle(const Vec2f &_p1, const Vec2f &_p2, const Vec2f &_p3)
	:	p1(_p1), p2(_p2), p3(_p3)
{
	parent = NULL;
}

std::vector<Vec2f> Triangle::getCommonVertices(Triangle *other)
{
	std::vector<Vec2f> result;

	if (this->p1 == other->p1) result.push_back(p1);
	else if (this->p2 == other->p1) result.push_back(p2);
	else if (this->p3 == other->p1) result.push_back(p3);

	if (this->p1 == other->p2) result.push_back(p1);
	else if (this->p2 == other->p2) result.push_back(p2);
	else if (this->p3 == other->p2) result.push_back(p3);

	if (this->p1 == other->p3) result.push_back(p1);
	else if (this->p2 == other->p3) result.push_back(p2);
	else if (this->p3 == other->p3) result.push_back(p3);

	return result;
}

Vec2f Triangle::edgeMidpointBetween(Triangle *other)
{
	Vec2f mid(0, 0);
	
	if (!this->isAdjacent(other)) return other->centroid();

	// In this case, triangle other must have only 2 vertices in common with ours
	// so it is not necessary to worry about the bounds of count in v.

	std::vector<Vec2f> &commonVertices = this->getCommonVertices(other);

	if (commonVertices.size() == 2)
	{
		mid.x = (commonVertices[0].x + commonVertices[1].x) / 2;
		mid.y = (commonVertices[0].y + commonVertices[1].y) / 2;
	}

	return mid;
}


Vec2f Triangle::centroid()
{
	Vec2f c;

	c.x = (this->p1.x + this->p2.x + this->p3.x) / 3;
	c.y = (this->p1.y + this->p2.y + this->p3.y) / 3;
	return c;
}


float Triangle::centroidDistance(Triangle *other)
{
	Vec2f c1;
	Vec2f c2;

	if (other == NULL) return 9999999999;
	
	c1 = this->centroid();
	c2 = other->centroid();
	return dist(c1, c2);
}

void Triangle::setParent(Triangle *p)
{
	if (p == NULL) return;

	parent = p;
}

void Triangle::set(Vec2f v1, Vec2f v2, Vec2f v3)
{
	p1 = v1;
	p2 = v2;
	p3 = v3;
}

bool Triangle::containsVertex(const Vec2f &v)
{
	return p1 == v || p2 == v || p3 == v; 
}

// If the given triangle has 2 vertices in common, it has an edge intersecting our own, hence is adjacent.
bool Triangle::isAdjacent(Triangle *t)
{
	int count = 0;

	if (t->containsVertex(p1)) count++;
	if (t->containsVertex(p2)) count++;
	if (t->containsVertex(p3)) count++;

	return count == 2;
}

float Triangle::sign(Vec2f *p1, Vec2f *p2, Vec2f *p3)
{
    return (p1->x - p3->x) * (p2->y - p3->y) - (p2->x - p3->x) * (p1->y - p3->y);
}

// 
bool Triangle::contains(Vec2f *p)
{
	float as_x = p->x - p1.x;
	float as_y = p->y - p1.y;

	bool s_ab = (p2.x - p1.x)*as_y - (p2.y - p1.y)*as_x > 0;

	if ((p3.x - p1.x)*as_y - (p3.y - p1.y)*as_x > 0 == s_ab) return false;

	if ((p3.x - p2.x)*(p->y - p2.y) - (p3.y - p2.y)*(p->x - p2.x) > 0 != s_ab) return false;

	return true;
}

bool Triangle::circumCircleContains(const Vec2f &v)
{
	float ab = (p1.x * p1.x) + (p1.y * p1.y);
	float cd = (p2.x * p2.x) + (p2.y * p2.y);
	float ef = (p3.x * p3.x) + (p3.y * p3.y);

	float circum_x = (ab * (p3.y - p2.y) + cd * (p1.y - p3.y) + ef * (p2.y - p1.y)) / (p1.x * (p3.y - p2.y) + p2.x * (p1.y - p3.y) + p3.x * (p2.y - p1.y)) / 2.f;
	float circum_y = (ab * (p3.x - p2.x) + cd * (p1.x - p3.x) + ef * (p2.x - p1.x)) / (p1.y * (p3.x - p2.x) + p2.y * (p1.x - p3.x) + p3.y * (p2.x - p1.x)) / 2.f;
	float circum_radius = sqrtf(((p1.x - circum_x) * (p1.x - circum_x)) + ((p1.y - circum_y) * (p1.y - circum_y)));

	float dist = sqrtf(((v.x - circum_x) * (v.x - circum_x)) + ((v.y - circum_y) * (v.y - circum_y)));
	return dist <= circum_radius;
}
