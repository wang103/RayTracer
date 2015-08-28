// Sphere surface.
#ifndef _SPHERE_H
#define _SPHERE_H

#include "Surface.h"
#include "Utility.h"

class Sphere : public Surface
{
public:
	Sphere();
	Sphere(const Point3f& _center, float _radius);

	virtual bool Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const;

	virtual void GatherLightSources(std::vector<const Surface*>& lights) const;

	virtual Point3f GetLightPointInGrid(int gridNum) const;

	virtual Vector3f GetNormal(const Point3f& p) const;

	Point3f GetCenter() const;
	void SetCenter(const Point3f& _c);

	float GetRadius() const;
	void SetRadius(float _r);

private:
	Point3f center;
	float	radius;
};

#endif
