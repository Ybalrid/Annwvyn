#include "stdafx.h"
#include "AnnVect3.hpp"

using namespace Annwvyn;

AnnVect3::AnnVect3() : Vector3() { init(); }

AnnVect3::AnnVect3(const Vector3& v) : Vector3(v) { init(); }

AnnVect3::AnnVect3(const float cx, const float cy, const float cz) : Vector3(cx, cy, cz) { init(); }

AnnVect3::AnnVect3(const int coord[3]) : Vector3(coord) { init(); }

AnnVect3::AnnVect3(const float coord[3]) : Vector3(coord) { init(); }

AnnVect3::AnnVect3(float* const r) : Vector3(r) { init(); }

AnnVect3::AnnVect3(const float s) : Vector3(s) { init(); }

AnnVect3::AnnVect3(const btVector3& v) : Vector3(v.x(), v.y(), v.z()) { init(); }

AnnVect3::AnnVect3(bool validState) : Vector3() { init(); valid = validState; }

void AnnVect3::init()
{
	valid = true;
}

btVector3 AnnVect3::getBtVector() const
{
	return btVector3(x, y, z);
}

bool AnnVect3::isValid() const
{
	return valid;
}
