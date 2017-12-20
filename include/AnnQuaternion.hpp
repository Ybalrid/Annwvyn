/**
* \file AnnQuaternion.hpp
* \brief A Quaternion
* \author A. Brainville (Ybalrid)
*/

#pragma once

#include "systemMacro.h"
#include <AnnVect3.hpp>
#include <AnnAngle.hpp>
#include <OgreQuaternion.h>
#include <OgreMatrix3.h>
#include <LinearMath/btQuaternion.h>
#include <al.h>

namespace Annwvyn
{
	///Represent a Quaternion
	class AnnDllExport AnnQuaternion : public Ogre::Quaternion
	{
	public:
		//Call of Ogre::Quaternion constructor
		AnnQuaternion();
		///Construct form an Ogre Quaternion
		AnnQuaternion(const Quaternion& q);
		///Construct from 4 float WXYZ
		AnnQuaternion(float cw, float cx, float cy, float cz);
		///Construct from a rotation matrix 3x3
		AnnQuaternion(const Ogre::Matrix3& rot);
		///Construct from an angle around an arbitrary axis
		AnnQuaternion(const AnnRadian angle, AnnVect3 raxis);
		///Construct from a 3 axis base
		AnnQuaternion(const AnnVect3& xAxis, const AnnVect3& yAxis, const AnnVect3& zAxis);
		///Construct from a vector array
		AnnQuaternion(const AnnVect3* vectorArray);
		///Construct from a float array
		AnnQuaternion(float* floatArray);
		///Constrruct from a bullet Quaternion
		AnnQuaternion(const btQuaternion& q);

		//Custom Annwvyn part

		///Get a vector pointing upwards from this quaternion
		AnnVect3 getUpVector() const;
		///Get a vector pointing in the direction if this quaternion
		AnnVect3 getAtVector() const;

		///get this quaternion as a bullet quaternion
		btQuaternion getBtQuaternion() const;

		///If true you can trust the data on this Quaternion
		bool isValid() const;

		///Used to say that the quaternion hasn't been retrieved
		AnnQuaternion(bool validState);
	private:
		///Set valid to true
		inline void init();
		///Validity boolean
		bool valid;
	};
}
