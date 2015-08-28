// A wall.
#ifndef _WALL_H
#define _WALL_H

#include <memory>
#include "Surface.h"
#include "Triangle.h"

class Wall : public Surface
{
public:
	Wall(const Point3f& p1, const Point3f& p2, const Point3f& p3, const Point3f& p4);

	virtual bool Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const;

	virtual void GatherLightSources(std::vector<const Surface*>& lights) const;

	virtual Point3f GetLightPointInGrid(int gridNum) const;

	virtual Vector3f GetNormal(const Point3f& p) const;

private:
	std::unique_ptr<Triangle> triangle1;
	std::unique_ptr<Triangle> triangle2;
};

#endif
