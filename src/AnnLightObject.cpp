#include "stdafx.h"
#include "AnnLightObject.hpp"

using namespace Annwvyn;

AnnLightObject::AnnLightObject(Ogre::Light* light) :
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

void AnnLightObject::setType(AnnLightObject::LightTypes type)
{
	light->setType(Ogre::Light::LightTypes(type));
}

void Annwvyn::AnnLightObject::setDiffuseColor(AnnColor color)
{
	light->setDiffuseColour(color.getOgreColor());
}

void Annwvyn::AnnLightObject::setSpecularColor(AnnColor color)
{
	light->setSpecularColour(color.getOgreColor());
}

AnnColor Annwvyn::AnnLightObject::getDiffuseColor()
{
	return AnnColor(light->getDiffuseColour());
}

AnnColor Annwvyn::AnnLightObject::getSpecularColor()
{
	return AnnColor(light->getSpecularColour());
}
