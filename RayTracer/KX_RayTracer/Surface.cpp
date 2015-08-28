#include "Surface.h"

void Surface::SetMaterial(const std::shared_ptr<Material>& _pMaterial)
{
	pMaterial = _pMaterial;
}

std::shared_ptr<Material> Surface::GetMaterial() const
{
	return pMaterial;
}

bool Surface::fIsLight() const
{
	return !(GetMaterial()->emissionColor == RGBColor());
}
