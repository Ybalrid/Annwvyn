#include "stdafx.h"
#include "AnnLightObject.hpp"

using namespace Annwvyn;

AnnLightObject::AnnLightObject( Ogre::Light* light):
	light(light)
{
}

void AnnLightObject::setPosition(AnnVect3 position)
{
	light->setPosition(position);
}

void AnnLightObject::setDirection(AnnVect3 direction)
{
	light->setDirection(direction);
}

void AnnLightObject::setType(Ogre::Light::LightTypes type)
{
	light->setType(type);
}
