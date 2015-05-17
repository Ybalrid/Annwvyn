#include "stdafx.h"
#include "AnnVect3.hpp"

using namespace Annwvyn;

AnnVect3::AnnVect3() : Ogre::Vector3()
{
}

AnnVect3::AnnVect3(const float cx, const float cy, const float cz) : Ogre::Vector3(cx, cy, cz)
{
}

AnnVect3::AnnVect3(const int coord[3]) : Ogre::Vector3(coord)
{
}

AnnVect3::AnnVect3(const float coord[3]) : Ogre::Vector3(coord)
{
}

AnnVect3::AnnVect3(float* const r) : Ogre::Vector3(r)
{
}

AnnVect3::AnnVect3(const float s) : Ogre::Vector3(s)
{
}

AnnVect3::AnnVect3(btVector3& v) : Ogre::Vector3(v.x(), v.y(), v.z())
{
}

btVector3 AnnVect3::getBtVector()
{
	return btVector3(x, y, z);
}