#pragma once

#include "systemMacro.h"
#include <Ogre.h>

namespace Annwvyn
{
	//Forward declaration of Radian class to permit to construct a degree object from a radian one
	class AnnRadian;

	///An angle in Degree
	class DLL AnnDegree : public Ogre::Degree
	{
	public:
		AnnDegree(float angle);
		AnnDegree(AnnRadian angle);
		AnnDegree(Ogre::Radian angle);
		AnnDegree(Degree angle);
	};

	///An angle in Radian
	class DLL AnnRadian : public Ogre::Radian
	{
	public:
		AnnRadian(float angle);
		AnnRadian(AnnDegree angle);
		AnnRadian(Ogre::Degree angle);
		AnnRadian(Radian angle);
	};

	///Angle object factory
	class DLL AnnAngle
	{
	public:
		static AnnDegree degree(float value);
		static AnnRadian radian(float value);
	};
}