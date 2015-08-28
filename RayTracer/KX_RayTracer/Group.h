// Group of surfaces.
#ifndef _GROUP_H
#define _GROUP_H

#include "Surface.h"
#include <vector>
#include "Sphere.h"
#include "Utility.h"

using namespace std;

class Group : public Surface
{
public:
	Group();

	virtual bool Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const;

	virtual Vector3f GetNormal(const Point3f& p) const;

	virtual void GatherLightSources(std::vector<const Surface*>& lights) const;

	virtual void SetMaterial(const std::shared_ptr<Material>& _pMaterial);

	virtual Point3f GetLightPointInGrid(int gridNum) const;

	void AddObject(const std::shared_ptr<Surface>& pObject);

	void SetEnclosingSphere(const Point3f& _c, float _r);

private:
	vector<const std::shared_ptr<Surface> > surfaces;

	Sphere enclosingSphere;
};

#endif
