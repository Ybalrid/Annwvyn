#include "stdafx.h"
#include "AnnQuaternion.hpp"

using namespace Annwvyn;

AnnQuaternion::AnnQuaternion(){}
AnnQuaternion::AnnQuaternion(const Ogre::Quaternion& q) : Ogre::Quaternion(q){}
AnnQuaternion::AnnQuaternion(float cw, float cx, float cy, float cz) : Ogre::Quaternion(cw,cx,cy,cz){}
AnnQuaternion::AnnQuaternion(const Ogre::Matrix3& rot) : Ogre::Quaternion(rot){}
AnnQuaternion::AnnQuaternion(const Ogre::Radian angle, AnnVect3 raxis) : Ogre::Quaternion(angle, raxis){}
AnnQuaternion::AnnQuaternion(const AnnVect3& xAxis, const AnnVect3& yAxis, const AnnVect3& zAxis) : Ogre::Quaternion(xAxis, yAxis, zAxis){}
AnnQuaternion::AnnQuaternion(const AnnVect3* vectorArray) : Ogre::Quaternion(vectorArray){}
AnnQuaternion::AnnQuaternion(float* floatArray) : Ogre::Quaternion(floatArray){}

btQuaternion AnnQuaternion::getBtQuaternion()
{
	return btQuaternion(x, y, z, w);
}


