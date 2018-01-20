// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnQuaternion.hpp"

using namespace Annwvyn;

AnnQuaternion::AnnQuaternion() { init(); }

AnnQuaternion::AnnQuaternion(const Quaternion& q) :
 Quaternion(q) { init(); }

AnnQuaternion::AnnQuaternion(float cw, float cx, float cy, float cz) :
 Quaternion(cw, cx, cy, cz) { init(); }

AnnQuaternion::AnnQuaternion(const Ogre::Matrix3& rot) :
 Quaternion(rot) { init(); }

AnnQuaternion::AnnQuaternion(const AnnRadian angle, AnnVect3 raxis) :
 Quaternion(angle, raxis) { init(); }

AnnQuaternion::AnnQuaternion(const AnnVect3& xAxis, const AnnVect3& yAxis, const AnnVect3& zAxis) :
 Quaternion(xAxis, yAxis, zAxis) { init(); }

AnnQuaternion::AnnQuaternion(const AnnVect3* vectorArray) :
 Quaternion(vectorArray) { init(); }

AnnQuaternion::AnnQuaternion(float* floatArray) :
 Quaternion(floatArray) { init(); }

AnnQuaternion::AnnQuaternion(const btQuaternion& q) :
 Quaternion(q.w(), q.x(), q.y(), q.z()) { init(); }

AnnQuaternion::AnnQuaternion(bool validState) :
 Quaternion()
{
	init();
	valid = validState;
}

AnnVect3 AnnQuaternion::getAtVector() const
{
	return *this * AnnVect3::NEGATIVE_UNIT_Z;
}

AnnVect3 AnnQuaternion::getUpVector() const
{
	return *this * AnnVect3::UNIT_Y;
}

btQuaternion AnnQuaternion::getBtQuaternion() const
{
	return btQuaternion(x, y, z, w);
}

void AnnQuaternion::init()
{
	valid = true;
}

bool AnnQuaternion::isValid() const
{
	return valid;
}
