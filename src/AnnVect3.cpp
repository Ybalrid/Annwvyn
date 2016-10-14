#include "stdafx.h"
#include "AnnVect3.hpp"

using namespace Annwvyn;

AnnVect3::AnnVect3() : Ogre::Vector3() { init(); }

AnnVect3::AnnVect3(const Ogre::Vector3& v) : Ogre::Vector3(v) { init(); }

AnnVect3::AnnVect3(const float cx, const float cy, const float cz) : Ogre::Vector3(cx, cy, cz) { init(); }

AnnVect3::AnnVect3(const int coord[3]) : Ogre::Vector3(coord) { init(); }

AnnVect3::AnnVect3(const float coord[3]) : Ogre::Vector3(coord) { init(); }

AnnVect3::AnnVect3(float* const r) : Ogre::Vector3(r) { init(); }

AnnVect3::AnnVect3(const float s) : Ogre::Vector3(s) { init(); }

AnnVect3::AnnVect3(btVector3& v) : Ogre::Vector3(v.x(), v.y(), v.z()) { init(); }

AnnVect3::AnnVect3(bool validState) : Ogre::Vector3() { init(); valid = validState; }

void AnnVect3::init()
{
	valid = true;
}

btVector3 AnnVect3::getBtVector()
{
	return btVector3(x, y, z);
}

bool AnnVect3::isValid()
{
	return valid;
}