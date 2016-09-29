#pragma once

#include "systemMacro.h"
#include <Ogre.h>

namespace Annwvyn
{
	//Forward declaration of Radian class to permit to contruct a degree object from a radian one
	class AnnRadian;
	class DLL AnnDegree : public Ogre::Degree
	{
	public:
		AnnDegree(float angle);
		AnnDegree(AnnRadian angle);
	};

	class DLL AnnRadian : public Ogre::Radian
	{
	public:
		AnnRadian(float angle);
		AnnRadian(AnnDegree angle);
	};

	class DLL AnnAngle
	{
	public:
		static AnnDegree degree(float value);
		static AnnRadian radian(float value);
	};

}
