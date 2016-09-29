#include "stdafx.h"
#include "AnnAngle.hpp"

using namespace Annwvyn;

AnnDegree::AnnDegree(float angle) : Ogre::Degree(angle){}
AnnDegree::AnnDegree(AnnRadian angle) : Ogre::Degree(angle){}
AnnDegree::AnnDegree(Ogre::Radian angle) : Ogre::Degree(angle){}
AnnDegree::AnnDegree(Ogre::Degree angle) : Ogre::Degree(angle){}

AnnRadian::AnnRadian(float angle) : Ogre::Radian(angle){}
AnnRadian::AnnRadian(AnnDegree angle) : Ogre::Radian(angle){}
AnnRadian::AnnRadian(Ogre::Degree angle) : Ogre::Radian(angle){}
AnnRadian::AnnRadian(Ogre::Radian angle) : Ogre::Radian(angle){}

AnnDegree AnnAngle::degree(float value)
{
	return AnnDegree(value);
}

AnnRadian AnnAngle::radian(float value)
{
	return AnnDegree(value);
}