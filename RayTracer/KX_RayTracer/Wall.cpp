#include "Wall.h"
#include "Ray.h"
#include "Triangle.h"

Wall::Wall(const Point3f& p1, const Point3f& p2, const Point3f& p3, const Point3f& p4)
{
	triangle1.reset(new Triangle(p1, p4, p3));
	triangle2.reset(new Triangle(p3, p2, p1));
}

bool Wall::Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const
{
	if (triangle1->Hit(ray, t0, t1, t, NULL, normal)) {
		if (s)
			*s = const_cast<Wall*>(this);
		return true;
	}

	if (triangle2->Hit(ray, t0, t1, t, NULL, normal)) {
		if (s)
			*s = const_cast<Wall*>(this);
		return true;
	}

	return false;
}

void Wall::GatherLightSources(std::vector<const Surface*>& lights) const
{
	if (fIsLight())
		lights.push_back(this);
}

Point3f Wall::GetLightPointInGrid(int gridNum) const
{
	if (gridNum < LIGHT_SAMPLES / 2)
		return triangle1->GetLightPointInGrid(gridNum);
	return triangle2->GetLightPointInGrid(gridNum - LIGHT_SAMPLES / 2);
}

Vector3f Wall::GetNormal(const Point3f& p) const
{
	// Assume the wall is flat, so the wall's normal is just either
	// triangle's normal.
	return triangle1->GetNormal(p);
}
