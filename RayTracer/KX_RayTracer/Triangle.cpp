#include "Triangle.h"
#include "Ray.h"

Triangle::Triangle(const Point3f& v1, const Point3f& v2, const Point3f& v3)
{
	vertex1 = v1; vertex2 = v2; vertex3 = v3;
}

bool Triangle::Hit(const Ray& ray, float t0, float t1, float *t, Surface **s, Vector3f *normal) const
{
	Matrix3x3 a;
	a[0][0] = vertex1.x - vertex2.x;
	a[0][1] = vertex1.x - vertex3.x;
	a[0][2] = ray.direction.x;

	a[1][0] = vertex1.y - vertex2.y;
	a[1][1] = vertex1.y - vertex3.y;
	a[1][2] = ray.direction.y;

	a[2][0] = vertex1.z - vertex2.z;
	a[2][1] = vertex1.z - vertex3.z;
	a[2][2] = ray.direction.z;

	Matrix3x3 tUpper;
	tUpper[0][0] = vertex1.x - vertex2.x;
	tUpper[0][1] = vertex1.x - vertex3.x;
	tUpper[0][2] = vertex1.x - ray.origin.x;

	tUpper[1][0] = vertex1.y - vertex2.y;
	tUpper[1][1] = vertex1.y - vertex3.y;
	tUpper[1][2] = vertex1.y - ray.origin.y;

	tUpper[2][0] = vertex1.z - vertex2.z;
	tUpper[2][1] = vertex1.z - vertex3.z;
	tUpper[2][2] = vertex1.z - ray.origin.z;

	float tUpperDet = tUpper.getDeterminant();
	float aDet = a.getDeterminant();

	float result = tUpperDet / aDet;

	if (result < t0 || result > t1)
		return false;

	Matrix3x3 gammaUpper;
	gammaUpper[0][0] = vertex1.x - vertex2.x;
	gammaUpper[0][1] = vertex1.x - ray.origin.x;
	gammaUpper[0][2] = ray.direction.x;

	gammaUpper[1][0] = vertex1.y - vertex2.y;
	gammaUpper[1][1] = vertex1.y - ray.origin.y;
	gammaUpper[1][2] = ray.direction.y;

	gammaUpper[2][0] = vertex1.z - vertex2.z;
	gammaUpper[2][1] = vertex1.z - ray.origin.z;
	gammaUpper[2][2] = ray.direction.z;

	float gammaUpperDet = gammaUpper.getDeterminant();

	float gamma = gammaUpperDet / aDet;

	if (gamma < 0.0 || gamma > 1.0)
		return false;

	Matrix3x3 betaUpper;
	betaUpper[0][0] = vertex1.x - ray.origin.x;
	betaUpper[0][1] = vertex1.x - vertex3.x;
	betaUpper[0][2] = ray.direction.x;

	betaUpper[1][0] = vertex1.y - ray.origin.y;
	betaUpper[1][1] = vertex1.y - vertex3.y;
	betaUpper[1][2] = ray.direction.y;

	betaUpper[2][0] = vertex1.z - ray.origin.z;
	betaUpper[2][1] = vertex1.z - vertex3.z;
	betaUpper[2][2] = ray.direction.z;
	
	float betaUpperDet = betaUpper.getDeterminant();

	float beta = betaUpperDet / aDet;

	if (beta < 0.0 || beta > 1.0 - gamma)
		return false;

	if (t)
		*t = result;
	if (s)
		*s = const_cast<Triangle*>(this);
	if (normal) {
		Vector3f n = GetNormal(Point3f() /*not used*/);
		normal->x = n.x; normal->y = n.y; normal->z = n.z;
	}

	return true;
}

void Triangle::GatherLightSources(std::vector<const Surface*>& lights) const
{
	if (fIsLight())
		lights.push_back(this);
}

Point3f Triangle::GetLightPointInGrid(int gridNum) const
{
	Vector3f u(vertex2 /*start*/, vertex1 /*end*/);
	Vector3f v(vertex2 /*start*/, vertex3 /*end*/);

	float du = _rand();
	float dv = _rand();

	return vertex2 + u * du + v * dv;
}

Vector3f Triangle::GetNormal(const Point3f& /*p*/) const
{
	Vector3f u(vertex3 /*start*/, vertex1 /*end*/);
	Vector3f v(vertex3 /*start*/, vertex2 /*end*/);

	return cross(u, v);
}
