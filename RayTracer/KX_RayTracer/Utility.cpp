#include "Utility.h"
#include <limits>
#include <Windows.h>
#include "Group.h"
#include "Triangle.h"

bool fUseFastShading = false;

// Return a random float between 0.0 and 1.0.
float _rand() {
	return static_cast<float>(rand()) / RAND_MAX;
}

float dot(Vector3f v1, Vector3f v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

Vector3f cross(Vector3f v1, Vector3f v2) {
	Vector3f prod;
	prod.x = v1.y * v2.z - v1.z * v2.y;
	prod.y = v1.z * v2.x - v1.x * v2.z;
	prod.z = v1.x * v2.y - v1.y * v2.x;
	return prod;
}

double get_wall_time() {
	LARGE_INTEGER time, freq;
	if (!QueryPerformanceFrequency(&freq)) {
		// Ignore error.
		return 0;
	}
	if (!QueryPerformanceCounter(&time)) {
		// Ignore error.
		return 0;
	}
	return (double)time.QuadPart / freq.QuadPart;
}

double get_cpu_time() {
	FILETIME a, b, c, d;
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
		// Returns total user time.
		// Can be tweaked to include kernel times as well.
		return
			(double)(d.dwLowDateTime |
			((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
	}

	// Ignore error.
	return 0;
}

Point3f Point3f::operator+(const Vector3f& add) const {
	return Point3f(x + add.x, y + add.y, z + add.z);
}

const int MESH_LINE_MAX = 255;   // max number of chars in one line in mesh file

int GetVertexIndexFromString(const std::string& str) {
	size_t index1 = string::npos;
	size_t index2 = string::npos;

	index1 = str.find('/');
	if (index1 != string::npos) {
		index2 = str.find('/', index1 + 1);
	}

	if (index1 == string::npos && index2 == string::npos) {
		return atoi(str.c_str()) - 1;
	}
	else {
		std::string str1 = str.substr(0, index1);
		return atoi(str1.c_str()) - 1;
	}
}

std::shared_ptr<Surface> LoadMesh(const char *file_name, float scale, const Vector3f& offset) {
	std::shared_ptr<Group> pMesh(new Group());

	FILE *fp;
	errno_t err = fopen_s(&fp, file_name, "r");
	char line[MESH_LINE_MAX];
	char delims[] = " ";
	char *type = NULL;
	char *param1 = NULL;
	char *param2 = NULL;
	char *param3 = NULL;
	char *next = NULL;

	float minX = std::numeric_limits<float>::infinity();
	float maxX = -std::numeric_limits<float>::infinity();
	float minY = std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();
	float minZ = std::numeric_limits<float>::infinity();
	float maxZ = -std::numeric_limits<float>::infinity();

	if (err == 0) {
		std::vector<Point3f> positionList;

		while (fgets(line, MESH_LINE_MAX, fp) != NULL) {
			if (strlen(line) == 0 || line[0] == '#') {
				continue;
			}

			type = strtok_s(line, delims, &next);
			param1 = strtok_s(NULL, delims, &next);
			param2 = strtok_s(NULL, delims, &next);
			param3 = strtok_s(NULL, delims, &next);

			if (strcmp(type, "v") == 0) {          // A vertex
				float p1 = static_cast<float>(atof(param1)) * scale;
				float p2 = static_cast<float>(atof(param2)) * scale;
				float p3 = static_cast<float>(atof(param3)) * scale;

				p1 += offset.x;
				p2 += offset.y;
				p3 += offset.z;

				if (p1 < minX) minX = p1;
				if (p1 > maxX) maxX = p1;
				if (p2 < minY) minY = p2;
				if (p2 > maxY) maxY = p2;
				if (p3 < minZ) minZ = p3;
				if (p3 > maxZ) maxZ = p3;

				Point3f point(p1, p2, p3);
				positionList.push_back(point);
			}
			else if (strcmp(type, "f") == 0) {     // A face
				std::string face_str_1(param1);
				std::string face_str_2(param2);
				std::string face_str_3(param3);

				int vertexIndex1 = GetVertexIndexFromString(face_str_1);
				int vertexIndex2 = GetVertexIndexFromString(face_str_2);
				int vertexIndex3 = GetVertexIndexFromString(face_str_3);

				std::shared_ptr<Triangle> tri(new Triangle(positionList[vertexIndex1], positionList[vertexIndex2], positionList[vertexIndex3]));
				pMesh->AddObject(tri);
			}
			else {
				// Just ignore.
			}
		}

		// End of the file has been reached.
		fclose(fp);
	}
	else {
		perror("Error opening the mesh file");
		return nullptr;
	}

	Point3f center((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);
	float radius = (maxX - minX) > (maxY - minY) && (maxX - minX) > (maxZ - minZ) ? (maxX - minX) / 2.0f :
		(maxY - minY) > (maxZ - minZ) ? (maxY - minY) / 2.0f : (maxZ - minZ) / 2.0f;
	radius += 0.001f;

	pMesh->SetEnclosingSphere(center, radius);

	return pMesh;
}
