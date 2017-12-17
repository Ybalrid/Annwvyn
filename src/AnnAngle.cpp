// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnAngle.hpp"

using namespace Annwvyn;

AnnDegree::AnnDegree(float angle) : Degree(angle) {}
AnnDegree::AnnDegree(AnnRadian angle) : Degree(angle) {}
AnnDegree::AnnDegree(Ogre::Radian angle) : Degree(angle) {}
AnnDegree::AnnDegree(Degree angle) : Degree(angle) {}

AnnRadian::AnnRadian(float angle) : Radian(angle) {}
AnnRadian::AnnRadian(AnnDegree angle) : Radian(angle) {}
AnnRadian::AnnRadian(Ogre::Degree angle) : Radian(angle) {}
AnnRadian::AnnRadian(Radian angle) : Radian(angle) {}

AnnDegree AnnAngle::degree(float value)
{
	return AnnDegree(value);
}

AnnRadian AnnAngle::radian(float value)
{
	return AnnDegree(value);
}