#include "stdafx.h"
#include "AnnLightObject.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnLightObject::AnnLightObject(Ogre::Light* light) :
	light(light)
{
	AnnDebug() << "Light constructor called";
	if (light)
	{
		node = AnnGetEngine()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		node->attachObject(light);
	}
}

AnnLightObject::~AnnLightObject()
{
	AnnDebug() << "Light destructor called";
	if (light)
	{
		if (node)
		{
			node->detachObject(light);
			if (node->getParentSceneNode())
				node->getParentSceneNode()->removeChild(node);
			AnnGetEngine()->getSceneManager()->destroySceneNode(node);
			node = nullptr;
		}
		AnnGetEngine()->getSceneManager()->destroyLight(light);
	}
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
	//TODO lights have to be attached to nodes
	//light->setPosition(position);

	node->setPosition(position);
}

void AnnLightObject::setDirection(AnnVect3 direction) const
{
	light->setDirection(direction);
}

void AnnLightObject::setType(LightTypes type) const
{
	light->setType(Ogre::Light::LightTypes(type));
}

void AnnLightObject::setDiffuseColor(AnnColor color) const
{
	light->setDiffuseColour(color.getOgreColor());
}

void AnnLightObject::setSpecularColor(AnnColor color) const
{
	light->setSpecularColour(color.getOgreColor());
}

AnnColor AnnLightObject::getDiffuseColor() const
{
	return AnnColor(light->getDiffuseColour());
}

AnnColor AnnLightObject::getSpecularColor() const
{
	return AnnColor(light->getSpecularColour());
}

AnnVect3 AnnLightObject::getPosition()
{
	return AnnVect3();
	//TODO lights have to be attached to nodes
	//return light->getPosition();
}

void AnnLightObject::setAttenuation(float range, float constant, float linear, float quadratic)
{
	light->setAttenuation(range, constant, linear, quadratic);
}

Ogre::Light* AnnLightObject::getOgreLight()
{
	return light;
}

void AnnLightObject::setPower(float lumens)
{
	light->setPowerScale(lumens);
}

float AnnLightObject::getPower()
{
	return light->getPowerScale();
}
