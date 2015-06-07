#ifndef ANNQUAT
#define ANNQUAT

#include "systemMacro.h"
#include "AnnVect3.hpp"
#include <OgreQuaternion.h>
#include <OgreMatrix3.h>
#include <LinearMath/btQuaternion.h>
#include <al.h>

namespace Annwvyn
{
	class DLL AnnQuaternion : public Ogre::Quaternion
	{
	public:
		AnnQuaternion();
		AnnQuaternion(const Ogre::Quaternion& q);
		AnnQuaternion(float cw, float cx, float cy, float cz);
		AnnQuaternion(const Ogre::Matrix3& rot);
		AnnQuaternion(const Ogre::Radian angle, AnnVect3 raxis);
		AnnQuaternion(const AnnVect3& xAxis, const AnnVect3& yAxis, const AnnVect3& zAxis);
		AnnQuaternion(const AnnVect3* vectorArray);
		AnnQuaternion(float* floatArray);

		AnnVect3 getUpVector();
		AnnVect3 getAtVector();
		
		btQuaternion getBtQuaternion();
	};
}

#endif
