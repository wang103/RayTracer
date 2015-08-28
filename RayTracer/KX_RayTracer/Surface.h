// Surface base class.
// A surface is any object that can be hit by a ray.
#ifndef _SURFACE_H
#define _SURFACE_H

#include <memory>
#include <vector>
#include "Utility.h"
#include "Material.h"

struct Ray;

class Surface
{
public:
	// Return true if 'ray' hits this surface between t0 and t1, and store the
	// hit point in 't' if 't' is not NULL, and store the surface that was hit
	// in 's' if 's' is not NULL, and store the normal in 'normal' if it is not
	// NULL. Otherwise return false.
	virtual bool Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const = 0;

	virtual Vector3f GetNormal(const Point3f& p) const = 0;

	// Put all the light sources into 'lights'.
	virtual void GatherLightSources(std::vector<const Surface*>& lights) const = 0;

	// Return a random position inside the light grid 'gridNum'.
	// Only used if this surface is a light source.
	virtual Point3f GetLightPointInGrid(int gridNum) const = 0;

	virtual void SetMaterial(const std::shared_ptr<Material>& _pMaterial);
	std::shared_ptr<Material> GetMaterial() const;

	bool fIsLight() const;

private:
	std::shared_ptr<Material> pMaterial;
};

#endif
