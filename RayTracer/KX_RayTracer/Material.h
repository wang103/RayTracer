// Material properties.
#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "SimpleImage.h"

enum class Type : char {
	DIFF,		// diffuse
	SPEC,		// specular
	REFR,		// refraction
};

struct Material {
	RGBColor materialColor;
	RGBColor emissionColor;
	Type reflType;
	float diffAmount;
	float refrIndex;
	float extrRefrIndex;

	Material() {
		reflType = Type::DIFF;
		diffAmount = 1.0f;
		refrIndex = 1.5f;
		extrRefrIndex = 1.0f;
	}

	Material(const RGBColor& _mColor) {
		materialColor = _mColor;
		reflType = Type::DIFF;
		diffAmount = 1.0f;
		refrIndex = 1.5f;
		extrRefrIndex = 1.0f;
	}

	Material(const RGBColor& _mColor, const RGBColor& _eColor) {
		materialColor = _mColor;
		emissionColor = _eColor;
		reflType = Type::DIFF;
		diffAmount = 1.0f;
		refrIndex = 1.5f;
		extrRefrIndex = 1.0f;
	}

	void SetEmissionColor(const RGBColor& _eColor) {
		emissionColor = _eColor;
	}

	void SetReflectionType(Type t) {
		reflType = t;
	}

	void SetDiffAmount(float diff) {
		diffAmount = diff;
	}

	void SetRefrIndex(float refr, float extrRefr) {
		refrIndex = refr;
		extrRefrIndex = extrRefr;
	}

	bool fIsReflective() const {
		return materialColor.r != 0 || materialColor.g != 0 || materialColor.b != 0;
	}
};

#endif
