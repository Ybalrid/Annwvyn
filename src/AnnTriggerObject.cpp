#include "stdafx.h"
#include "AnnTriggerObject.hpp"

using namespace Annwvyn;

AnnTriggerObject::AnnTriggerObject():
	m_contactWithPlayer(false),
	m_position(Ogre::Vector3(0,0,0))
{
}

void AnnTriggerObject::setPosition(Ogre::Vector3 pos)
{
	setPosition(pos.x, pos.y, pos.z);
}

void AnnTriggerObject::setPosition(float x, float y, float z)
{
	m_position = Ogre::Vector3(x,y,z);
}

bool AnnTriggerObject::getContactInformation()
{
	return m_contactWithPlayer;
}

Ogre::Vector3 AnnTriggerObject::getPosition()
{
	return m_position;
}

void AnnTriggerObject::setContactInformation(bool contact)
{
	lastFrameContactWithPlayer = m_contactWithPlayer;
	m_contactWithPlayer = contact;
}

float AnnSphericalTriggerObject::getThreshold()
{
	return m_threshold;
}

void AnnSphericalTriggerObject::setThreshold(float threshold)
{
	m_threshold = threshold;
}

AnnSphericalTriggerObject::AnnSphericalTriggerObject() : AnnTriggerObject(),
	m_threshold(1)
{
}

bool AnnSphericalTriggerObject::computeVolumetricTest(AnnPlayer* player)
{
	return Tools::Geometry::distance(player->getPosition(),
		getPosition()) <= getThreshold();
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

bool AnnAlignedBoxTriggerObject::computeVolumetricTest(AnnPlayer* player)
{
	AnnVect3 position(player->getPosition());

	if((position.x >= xMin && position.x <= xMax) && 
		(position.y >= yMin && position.y <= yMax) && 
		(position.z >= zMin && position.z <= zMax))
		return true;
	return false;
}
