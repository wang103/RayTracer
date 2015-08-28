#ifndef _RAY_H
#define _RAY_H

#include "Utility.h"

struct Ray {
	Point3f origin;
	Vector3f direction;

	Ray(const Point3f& _o, const Vector3f& _d) {
		origin = _o;
		direction = _d;
		direction.Normalize();
	}

	// 'prob' is the likelyhood to keep reflecting once 'depth' is certain value.
	// 'fHitDiffuse' indicates whether or not a diffuse surface has been hit.
	RGBColor traceForColor(const Surface& surface, int depth, float prob, bool fHitDiffuse) const;

	RGBColor traceForLight(const Surface& surface, const Surface *light) const;
};

#endif
