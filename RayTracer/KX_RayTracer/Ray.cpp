#include "Ray.h"
#include "Surface.h"
#include <algorithm>
#include <functional>

static float hemisphereStepLength = static_cast<float>(M_PI) / HEMISPHERE_SAMPLES;
static float eclipticStepLength = static_cast<float>(2 * M_PI) / ECLIPTIC_SAMPLES;

// RGBColor returned must have 0<=r<=1, 0<=g<=1, 0<=b<=1.
RGBColor Ray::traceForColor(const Surface& surface, int depth, float prob, bool fHitDiffuse) const {

	float t;
	Surface *s = nullptr;
	Vector3f normal;
	depth++;

	if (surface.Hit(*this, RAY_T0, RAY_T1, &t, &s, &normal) == false) {
		// Did not hit anything, return background color: black.
		return RGBColor(0.0f, 0.0f, 0.0f);
	}

	if (s == nullptr || s->GetMaterial() == nullptr) {
		// If reached here, somehow the material of this surface does not exist.
		// This should not happen.
		return RGBColor(0.0f, 0.0f, 0.0f);
	}

	Vector3f rayEnd = direction * t;
	Point3f hitPoint = Point3f(origin.x + rayEnd.x, origin.y + rayEnd.y, origin.z + rayEnd.z);
	normal.Normalize();
	// Correct normal's direction: make sure it's always on the same side as the incoming ray.
	bool fRayNormalOnSameSide = dot(normal, direction) < 0;
	normal = fRayNormalOnSameSide ? normal : normal * -1;

	std::shared_ptr<Material> pMaterial = s->GetMaterial();

	RGBColor materialColor = pMaterial->materialColor;
	RGBColor emissionColor = pMaterial->emissionColor;

	if ((depth > 2 && _rand() > prob) || depth > 5) {
		return emissionColor;
	}

	if (pMaterial->reflType == Type::DIFF)
	{
		// Treat front face of a light as light, treat its back face as
		// a regular non-light surface.
		if (s->fIsLight() && fRayNormalOnSameSide)
			return emissionColor;
		else if (s->fIsLight())
			return RGBColor();

		RGBColor result;

		if (fUseFastShading) {
			std::vector<const Surface*> lights;
			surface.GatherLightSources(lights);
			std::vector<const Surface*>::const_iterator it;
			for (it = lights.begin(); it != lights.end(); ++it)
			{
				const Surface *light = *it;
				RGBColor lightResult;

				for (int gridIndex = 0; gridIndex < LIGHT_SAMPLES; gridIndex++)
				{
					Vector3f L(hitPoint /*Start*/, light->GetLightPointInGrid(gridIndex) /*End*/);
					L.Normalize();
					Ray rayTowardsLight(hitPoint, L);
					float dotP = dot(L, normal);
					RGBColor DiffC = (dotP > 0) ? rayTowardsLight.traceForLight(surface, light) * dotP * s->GetMaterial()->diffAmount * materialColor
						: RGBColor();

					lightResult = lightResult + DiffC;
				}

				lightResult = (lightResult * (1.0f / LIGHT_SAMPLES)).Trunc();
				result = result + lightResult;
			}
		}
		else {
			// Create multiple diffuse rays bouncing off from the hit point.
			// Create orthonormal coordinate frame at the hit point (w, u, v).
			Vector3f w = normal;
			Vector3f u = cross((fabs(w.x) > 0.1) ? Vector3f(0, 1.f, 0) : Vector3f(1.f, 0, 0), w); // u is perpendicular to w
			Vector3f v = cross(w, u); // z is perpendicular to w and u
			u.Normalize();
			v.Normalize();

			std::vector<RGBColor> diffuseResults;

			for (uint8_t longitude_coord = 0; longitude_coord < HEMISPHERE_SAMPLES; longitude_coord++)
			{
				for (uint8_t latitude_coord = 0; latitude_coord < ECLIPTIC_SAMPLES; latitude_coord++)
				{
					float theta = hemisphereStepLength * (longitude_coord + _rand());
					float phi = eclipticStepLength * (latitude_coord + _rand());

					float sin_theta = sin(theta);

					Vector3f diffRelfDir = u * sin_theta * cos(phi) + v * sin_theta * sin(phi) + w * cos(theta);
					Ray diffRelfRay(hitPoint, diffRelfDir);
					RGBColor tracedColor = diffRelfRay.traceForColor(surface, depth, fHitDiffuse ? prob * DIFFUSE_FACTOR : prob, true /*fHitDiffuse*/);
					
					if (fHitDiffuse) {
						// Perf optimization.
						if (tracedColor.r >= 0.1f || tracedColor.g >= 0.1f || tracedColor.b >= 0.1f)
							return tracedColor * materialColor;
					}
					
					diffuseResults.push_back(tracedColor);
				}
			}

			std::sort(diffuseResults.begin(), diffuseResults.end(), std::greater<RGBColor>());

			result = materialColor * (diffuseResults[0] + diffuseResults[1] + diffuseResults[2] + diffuseResults[3]) * 0.25;
		}

		return result.Trunc();
	}
	else if (pMaterial->reflType == Type::SPEC)
	{
		// Create reflection ray.
		Vector3f reflDir = direction - normal * 2.0f * dot(direction, normal);
		Ray reflRay = Ray(hitPoint, reflDir);

		RGBColor result = emissionColor + materialColor * reflRay.traceForColor(surface, depth, prob * REFLECTION_FACTOR, fHitDiffuse);
		return result.Trunc();
	}
	else // pMaterial->reflType == Type::REFR
	{
		Vector3f reflDir = direction - normal * 2.0f * dot(direction, normal);
		Ray reflRay(hitPoint, reflDir);

		bool fOutSideIn = fRayNormalOnSameSide;

		// Ideal Dielectric Refraction
		float ni = (fOutSideIn) ? pMaterial->extrRefrIndex : pMaterial->refrIndex;
		float nt = (fOutSideIn) ? pMaterial->refrIndex : pMaterial->extrRefrIndex;

		float nnt = ni / nt;                             // sin(t) / sin(i)
		float cosi = fabs(dot(direction, normal));       // cos(i)
		float cos2t = 1 - nnt * nnt * (1 - cosi * cosi); // cos(t)^2 = 1 - (sin(t) / sin(i))^2 * (1 - cos(i)^2)

		if (cos2t < 0) // Total internal Reflection
		{
			RGBColor result = emissionColor + materialColor * reflRay.traceForColor(surface, depth, prob * REFLECTION_FACTOR, fHitDiffuse);
			return result.Trunc();
		}

		float cost = sqrt(cos2t);

		// Refraction direction ref:
		// http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
		// formula (22)
		Vector3f refrDir = direction * nnt + normal * (nnt * cosi - cost);
		Ray refrRay(hitPoint, refrDir);

		// Reflection intensity ref:
		// https://en.wikipedia.org/wiki/Fresnel_equations
		float Rs = pow((nnt * cosi - cost) / (nnt * cosi + cost), 2);
		float Rp = pow((nnt * cost - cosi) / (nnt * cost + cosi), 2);
		float refl_Intensity = (Rs + Rp) / 2;

		RGBColor result = emissionColor + materialColor * (
			reflRay.traceForColor(surface, depth, prob * REFLECTION_FACTOR, fHitDiffuse) * refl_Intensity +
			refrRay.traceForColor(surface, depth - 1, prob * REFRACTION_FACTOR, fHitDiffuse) * (1 - refl_Intensity));

		return result.Trunc();
	}
};

// Return RGBColor based on one light source and shadow.
// Assumption: this ray points at a light source.
RGBColor Ray::traceForLight(const Surface& surface, const Surface *light) const
{
	// Find the light.
	float t;
	Surface *s = nullptr;

	if (surface.Hit(*this, RAY_T0, RAY_T1, &t, &s, nullptr) == false) {
		// Did not hit anything, but is not possible with this ray, the assumption is violated.
		std::cerr << "Error: Didn't hit anything as expected." << std::endl;
		return RGBColor(0.0f, 0.0f, 0.0f);
	}

	if (s == nullptr || s->GetMaterial() == nullptr) {
		// If reached here, somehow the material of this surface does not exist.
		// This should not happen.
		std::cerr << "Error: Didn't find light correctly as expect." << std::endl;
		return RGBColor(0.0f, 0.0f, 0.0f);
	}

	if (s != light) // something blocked
		return RGBColor();
	else
		return s->GetMaterial()->emissionColor;
}
