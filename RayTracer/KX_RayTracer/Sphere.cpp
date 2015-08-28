#include "Sphere.h"
#include "Ray.h"
#include <algorithm>

Sphere::Sphere()
{
	center = Point3f();
	radius = 1.0f;
}

Sphere::Sphere(const Point3f& _center, float _radius)
{
	center = _center;
	radius = _radius;
}

bool Sphere::Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const
{
	Vector3f ce = Vector3f(center /*start*/, ray.origin /*end*/);
	float discriminant = pow(dot(ray.direction, ce), 2) - dot(ray.direction, ray.direction) * (dot(ce, ce) - radius * radius);
	if (discriminant < 0)
		return false;

	float result[2];
	result[0] = (-dot(ray.direction, ce) - sqrt(discriminant)) / dot(ray.direction, ray.direction);
	result[1] = (-dot(ray.direction, ce) + sqrt(discriminant)) / dot(ray.direction, ray.direction);

	float res;
	if (result[0] < 1e-5 && result[1] < 1e-5)
		return false;
	else if (result[0] < 1e-5)
		res = result[1];
	else if (result[1] < 1e-5)
		res = result[0];
	else
		res = std::min(result[0], result[1]);

	if (res >= t0 && res <= t1) {
		if (t)
			*t = res;
		if (s)
			*s = const_cast<Sphere*>(this);
		if (normal) {
			Vector3f rayEnd = ray.direction * res;
			Point3f hitPoint = Point3f(ray.origin.x + rayEnd.x, ray.origin.y + rayEnd.y, ray.origin.z + rayEnd.z);
			Vector3f n = GetNormal(hitPoint);
			normal->x = n.x; normal->y = n.y; normal->z = n.z;
		}
		return true;
	}

	return false;
}

void Sphere::GatherLightSources(std::vector<const Surface*>& lights) const
{
	if (fIsLight())
		lights.push_back(this);
}

Point3f Sphere::GetLightPointInGrid(int gridNum) const
{
	return center;
}

Vector3f Sphere::GetNormal(const Point3f& p) const
{
	Vector3f normal(center /*start*/, p /*end*/);
	normal.Normalize();
	return normal;
}

Point3f Sphere::GetCenter() const
{
	return center;
}

void Sphere::SetCenter(const Point3f& _c)
{
	center = _c;
}

float Sphere::GetRadius() const
{
	return radius;
}

void Sphere::SetRadius(float _r)
{
	radius = _r;
}
