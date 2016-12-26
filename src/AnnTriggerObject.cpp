#include "stdafx.h"
#include "AnnTriggerObject.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnTriggerObject::AnnTriggerObject() :
	position(Ogre::Vector3(0, 0, 0)),
	contactWithPlayer(false),
	lastFrameContactWithPlayer(false)
{
}

AnnTriggerObject::~AnnTriggerObject()
{
	AnnDebug() << "AnnTriggerObject destructor called";
}

void AnnTriggerObject::setPosition(AnnVect3 pos)
{
	position = pos;
}

bool AnnTriggerObject::getContactInformation()
{
	return contactWithPlayer;
}

AnnVect3 AnnTriggerObject::getPosition()
{
	return position;
}

void AnnTriggerObject::setContactInformation(bool contact)
{
	lastFrameContactWithPlayer = contactWithPlayer;
	contactWithPlayer = contact;
}

float AnnSphericalTriggerObject::getThreshold()
{
	return threshold;
}

void AnnSphericalTriggerObject::setThreshold(float newThreshold)
{
	threshold = newThreshold;
	squaredThreshold = threshold*threshold;
}

AnnSphericalTriggerObject::AnnSphericalTriggerObject() : AnnTriggerObject(),
threshold(1),
squaredThreshold()
{
}

bool AnnSphericalTriggerObject::computeVolumetricTest(std::shared_ptr<AnnPlayer> player)
{
	return getPosition().squaredDistance(player->getPosition()) <= squaredThreshold;
}

AnnAlignedBoxTriggerObject::AnnAlignedBoxTriggerObject() : AnnTriggerObject(),
xMin(0),
xMax(0),
yMin(0),
yMax(0),
zMin(0),
zMax(0)
{
}

void AnnAlignedBoxTriggerObject::setBoundaries(float x1, float x2, float y1, float y2, float z1, float z2)
{
	xMin = x1;
	xMax = x2;
	yMin = y1;
	yMax = y2;
	zMin = z1;
	zMax = z2;
}

bool AnnAlignedBoxTriggerObject::computeVolumetricTest(std::shared_ptr<AnnPlayer> player)
{
	AnnVect3 pos(player->getPosition());

	if ((pos.x >= xMin && pos.x <= xMax) &&
		(pos.y >= yMin && pos.y <= yMax) &&
		(pos.z >= zMin && pos.z <= zMax))
		return true;
	return false;
}