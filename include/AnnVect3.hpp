/**
* \file AnnVect3.hpp
* \brief A 3D Vector
* \author A. Brainville (Ybalrid)
*/

#pragma once

#include "systemMacro.h"
#include <OgreVector3.h>
#include <LinearMath/btVector3.h>

namespace Annwvyn
{
	///A 3D Vector
	class AnnDllExport AnnVect3 : public Ogre::Vector3
	{
	public:
		//Call of Ogre::Vector3 constructors
		AnnVect3();
		///Construct from a Ogre::Vector3
		AnnVect3(const Vector3& v);
		///Construct form 3 floats
		AnnVect3(const float cx, const float cy, const float cz);
		///Construct from an array of three inits
		AnnVect3(const int coord[3]);
		///Construct from an array of 3 float
		AnnVect3(const float coord[3]);
		///Construct form a float array of unspecified length
		AnnVect3(float* const r);
		///Construct from a float value
		AnnVect3(const float s);

		///Only used to signify that this object contains corrupt data
		AnnVect3(bool validState);

		///Construct from bullet vector
		AnnVect3(const btVector3& v);

		///Return as bullet vector
		btVector3 getBtVector() const;

		///You can trust this vector
		bool isValid() const;

        ///Implict convertion to btVector3
        operator btVector3() const;

	private:
		///Set valid to true
		void init();

		///Validity boolean
		bool valid;
	};
}
