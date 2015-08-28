// All utility functions and base structs.

#ifndef _UTILITY_H
#define _UTILITY_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include "SimpleImage.h"

class Surface;
struct Vector3f;

const float RAY_T0 = 0.0001f;
const float RAY_T1 = 1000.0f;

const int LIGHT_SAMPLES      = 16;  // Each light source is a 4x4 grids.
const int ECLIPTIC_SAMPLES   = 8;   // Diffuse Reflection samples at ecliptic.
const int HEMISPHERE_SAMPLES = 4;   // Diffuse Reflection samples in the upper hemisphere.

const float REFLECTION_FACTOR = 0.99f;
const float DIFFUSE_FACTOR = 0.3f;
const float REFRACTION_FACTOR = 0.99f;

extern bool fUseFastShading;

struct Point3f {
	float x, y, z;

	Point3f() {
		x = 0.0f; y = 0.0f; z = 0.0f;
	}

	Point3f(float _x, float _y, float _z) {
		x = _x; y = _y; z = _z;
	}

	bool operator==(const Point3f& p) {
		return x == p.x && y == p.y && z == p.z;
	}

	bool operator!=(const Point3f& p) {
		return x != p.x || y != p.y || z != p.z;
	}

	Point3f operator-() const {
		return Point3f(-x, -y, -z);
	}

	Point3f operator*(float s) const {
		return Point3f(x * s, y * s, z * s);
	}

	Point3f operator+(const Point3f& add) const {
		return Point3f(x + add.x, y + add.y, z + add.z);
	}

	Point3f operator-(const Point3f& sub) const {
		return Point3f(x - sub.x, y - sub.y, z - sub.z);
	}

	Point3f operator+(const Vector3f& add) const;
};

struct Vector3f {
	float x, y, z;

	Vector3f() {
		x = 0; y = 0; z = 0;
	}

	Vector3f(float _x, float _y, float _z) {
		x = _x; y = _y; z = _z;
	}

	Vector3f(const Point3f& start, const Point3f& end) {
		x = end.x - start.x;
		y = end.y - start.y;
		z = end.z - start.z;
	}

	void Normalize() {
		float length = sqrt(x * x + y * y + z * z);
		if (length != 0) {
			x /= length;
			y /= length;
			z /= length;
		}
	}

	Vector3f operator*(float s) const {
		return Vector3f(x * s, y * s, z * s);
	}

	Vector3f operator+(const Vector3f& add) const {
		return Vector3f(x + add.x, y + add.y, z + add.z);
	}

	Vector3f operator-(const Vector3f& sub) const {
		return Vector3f(x - sub.x, y - sub.y, z - sub.z);
	}

	float& operator [](int i) {
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else
			return z;
	}

	float operator [](int i) const {
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else
			return z;
	}
};

// A 3x3 matrix. By default it is an identity matrix.
struct Matrix3x3 {
	float matrix[3][3];

	Matrix3x3() {
		make_identity();
	}

	void make_identity() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (i == j)
					matrix[i][j] = 1.0;
				else
					matrix[i][j] = 0.0;
			}
		}
	}

	Matrix3x3 operator*(const Matrix3x3& m) const {
		Matrix3x3 result;

		result[0][0] = matrix[0][0] * m[0][0] + matrix[0][1] * m[1][0] + matrix[0][2] * m[2][0];
		result[0][1] = matrix[0][0] * m[0][1] + matrix[0][1] * m[1][1] + matrix[0][2] * m[2][1];
		result[0][2] = matrix[0][0] * m[0][2] + matrix[0][1] * m[1][2] + matrix[0][2] * m[2][2];

		result[1][0] = matrix[1][0] * m[0][0] + matrix[1][1] * m[1][0] + matrix[1][2] * m[2][0];
		result[1][1] = matrix[1][0] * m[0][1] + matrix[1][1] * m[1][1] + matrix[1][2] * m[2][1];
		result[1][2] = matrix[1][0] * m[0][2] + matrix[1][1] * m[1][2] + matrix[1][2] * m[2][2];

		result[2][0] = matrix[2][0] * m[0][0] + matrix[2][1] * m[1][0] + matrix[2][2] * m[2][0];
		result[2][1] = matrix[2][0] * m[0][1] + matrix[2][1] * m[1][1] + matrix[2][2] * m[2][1];
		result[2][2] = matrix[2][0] * m[0][2] + matrix[2][1] * m[1][2] + matrix[2][2] * m[2][2];

		return result;
	}

	Vector3f operator*(const Vector3f& v) const {
		Vector3f result;

		result[0] = matrix[0][0] * v[0] + matrix[0][1] * v[1] + matrix[0][2] * v[2];
		result[1] = matrix[1][0] * v[0] + matrix[1][1] * v[1] + matrix[1][2] * v[2];
		result[2] = matrix[2][0] * v[0] + matrix[2][1] * v[1] + matrix[2][2] * v[2];

		return result;
	}

	float getDeterminant() const {
		return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
			   matrix[1][0] * (matrix[0][1] * matrix[2][2] - matrix[2][1] * matrix[0][2]) +
			   matrix[2][0] * (matrix[0][1] * matrix[1][2] - matrix[1][1] * matrix[0][2]);
	}

	float* operator [](int i) {
		return matrix[i];
	}

	const float* operator [](int i) const {
		return matrix[i];
	}
};

// Utility methods.

// Randomly pick a float between -r and +r.
inline float frand_radius(float r) {
	return (-r) + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (2.0f * r));
}

float _rand();
float dot(Vector3f v1, Vector3f v2);
Vector3f cross(Vector3f v1, Vector3f v2);

double get_wall_time();
double get_cpu_time();

std::shared_ptr<Surface> LoadMesh(const char *file_name, float scale, const Vector3f& offset);

#endif
