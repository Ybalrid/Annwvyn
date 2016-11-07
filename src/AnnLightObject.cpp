#include "stdafx.h"
#include "AnnLightObject.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnLightObject::AnnLightObject(Ogre::Light* light) :
	light(light)
{
	AnnDebug() << "Light constructor called";
}

AnnLightObject::~AnnLightObject()
{
	AnnDebug() << "Light destructor called";
	if (light)
		AnnGetEngine()->getSceneManager()->destroyLight(light);
}

AnnLightObject::LightTypes AnnLightObject::getLightTypeFromString(std::string ltype)
{
	if (ltype == "Point") return ANN_LIGHT_POINT;
	if (ltype == "Spotlight") return ANN_LIGHT_SPOTLIGHT;
	if (ltype == "Directional") return ANN_LIGHT_DIRECTIONAL;
	return ANN_LIGHT_POINT;
}

void AnnLightObject::setPosition(AnnVect3 position)
{
	light->setPosition(position);
}

void AnnLightObject::setDirection(AnnVect3 direction)
{
	light->setDirection(direction);
}

void AnnLightObject::setType(LightTypes type)
{
	light->setType(Ogre::Light::LightTypes(type));
}

void AnnLightObject::setDiffuseColor(AnnColor color)
{
	light->setDiffuseColour(color.getOgreColor());
}

void AnnLightObject::setSpecularColor(AnnColor color)
{
	light->setSpecularColour(color.getOgreColor());
}

AnnColor AnnLightObject::getDiffuseColor()
{
	return AnnColor(light->getDiffuseColour());
}

AnnColor AnnLightObject::getSpecularColor()
{
	return AnnColor(light->getSpecularColour());
}

AnnVect3 AnnLightObject::getPosition()
{
	return light->getPosition();
}