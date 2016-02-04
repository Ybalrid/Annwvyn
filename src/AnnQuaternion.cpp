#include "stdafx.h"
#include "AnnQuaternion.hpp"

using namespace Annwvyn;

AnnQuaternion::AnnQuaternion(){init();}	

AnnQuaternion::AnnQuaternion(const Ogre::Quaternion& q) : Ogre::Quaternion(q){init();}

AnnQuaternion::AnnQuaternion(float cw, float cx, float cy, float cz) : Ogre::Quaternion(cw, cx, cy, cz){init();}

AnnQuaternion::AnnQuaternion(const Ogre::Matrix3& rot) : Ogre::Quaternion(rot){init();}

AnnQuaternion::AnnQuaternion(const Ogre::Radian angle, AnnVect3 raxis) : Ogre::Quaternion(angle, raxis){init();}

AnnQuaternion::AnnQuaternion(const AnnVect3& xAxis, const AnnVect3& yAxis, const AnnVect3& zAxis) : Ogre::Quaternion(xAxis, yAxis, zAxis){init();}

AnnQuaternion::AnnQuaternion(const AnnVect3* vectorArray) : Ogre::Quaternion(vectorArray){init();}

AnnQuaternion::AnnQuaternion(float* floatArray) : Ogre::Quaternion(floatArray){init();}

AnnQuaternion::AnnQuaternion(bool validState) : Ogre::Quaternion(){init(); valid=false;}

AnnVect3 AnnQuaternion::getAtVector()
{
	return ((*this) * AnnVect3::NEGATIVE_UNIT_Z);
}

AnnVect3 AnnQuaternion::getUpVector()
{
	return ((*this) * AnnVect3::UNIT_Y);
}

btQuaternion AnnQuaternion::getBtQuaternion()
{
	return btQuaternion(x, y, z, w);
}

void AnnQuaternion::init()
{
	valid = true;
}


