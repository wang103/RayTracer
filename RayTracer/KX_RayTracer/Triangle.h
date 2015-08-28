// Triangle surface.
#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "Surface.h"
#include "Utility.h"

class Triangle : public Surface
{
public:
	Triangle(const Point3f& v1, const Point3f& v2, const Point3f& v3);

	virtual bool Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const;
 
	virtual void GatherLightSources(std::vector<const Surface*>& lights) const;

	virtual Point3f GetLightPointInGrid(int gridNum) const;

	virtual Vector3f GetNormal(const Point3f& p) const;

	Point3f GetVertex1() const { return vertex1; }
	Point3f GetVertex2() const { return vertex2; }
	Point3f GetVertex3() const { return vertex3; }

private:
	Point3f vertex1;
	Point3f vertex2;
	Point3f vertex3;
};

#endif
