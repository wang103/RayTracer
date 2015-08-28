#include "Group.h"
#include "Ray.h"

Group::Group()
{
	enclosingSphere.SetRadius(0.0f);
}

bool Group::Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const
{
	if (enclosingSphere.GetRadius() != 0.0f) {
		// First test if the enclosing sphere is hit. If not, early terminate.
		if (!enclosingSphere.Hit(ray, t0, t1, NULL, NULL, NULL))
			return false;
	}

	bool fHit = false;
	float minT = -1;
	float tTemp;

	std::vector<const std::shared_ptr<Surface> >::const_iterator it;
	for (it = surfaces.begin(); it != surfaces.end(); ++it) {
		if ((*it)->Hit(ray, t0, t1, &tTemp, s, normal)) {
			fHit = true;

			if (minT == -1 || tTemp < minT) {
				minT = tTemp;
				t1 = minT;
			}
		}
	}

	if (t)
		*t = minT;

	return fHit;
}

Vector3f Group::GetNormal(const Point3f& p) const
{
	// Should not be used.
	return Vector3f();
}

void Group::GatherLightSources(std::vector<const Surface*>& lights) const
{
	std::vector<const std::shared_ptr<Surface> >::const_iterator it;
	for (it = surfaces.begin(); it != surfaces.end(); ++it) {
		(*it)->GatherLightSources(lights);
	}
}

void Group::SetMaterial(const std::shared_ptr<Material>& _pMaterial)
{
	Surface::SetMaterial(_pMaterial);

	std::vector<const std::shared_ptr<Surface> >::iterator it;
	for (it = surfaces.begin(); it != surfaces.end(); ++it) {
		(*it)->SetMaterial(_pMaterial);
	}
}

Point3f Group::GetLightPointInGrid(int gridNum) const
{
	return Point3f();
}

void Group::AddObject(const std::shared_ptr<Surface>& pObject)
{
	surfaces.push_back(pObject);
}

void Group::SetEnclosingSphere(const Point3f& _c, float _r)
{
	enclosingSphere.SetCenter(_c);
	enclosingSphere.SetRadius(_r);
}
