#include "AnnTriggerObject.hpp"

using namespace Annwvyn;

AnnTriggerObject::AnnTriggerObject()
{
	m_contactWithPlayer = false;
	m_threshold = 1.0f;
	
	m_position = Ogre::Vector3(0,0,0);
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
	m_contactWithPlayer = contact;
}

float AnnTriggerObject::getThreshold()
{
	return m_threshold;
}

void AnnTriggerObject::setThreshold(float threshold)
{
    m_threshold = threshold;
}
