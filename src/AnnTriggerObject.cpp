#include "stdafx.h"
#include "AnnTriggerObject.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnTriggerObject::AnnTriggerObject() :
	contactWithPlayer(false),
	lastFrameContactWithPlayer(false), body(nullptr), shape(nullptr)
{
}

AnnTriggerObject::~AnnTriggerObject()
{
	AnnDebug() << "AnnTriggerObject destructor called";
}

void AnnTriggerObject::setPosition(AnnVect3 pos)
{
	if (!body) return;
	auto transform = body->getWorldTransform();
	transform.setOrigin(pos.getBtVector());
	body->setWorldTransform(transform);
}

void AnnTriggerObject::setOrientation(AnnQuaternion orient)
{
	if (!body) return;
	auto transform = body->getWorldTransform();
	transform.setRotation(orient.getBtQuaternion());
	body->setWorldTransform(transform);
}

bool AnnTriggerObject::getContactInformation() const
{
	return contactWithPlayer;
}

AnnVect3 AnnTriggerObject::getPosition()
{
	if (body)
		return{ body->getWorldTransform().getOrigin() };
	return{};
}

AnnQuaternion AnnTriggerObject::getOrientation()
{
	if (body)
		return{ body->getWorldTransform().getRotation() };
	return{};
}

void AnnTriggerObject::setContactInformation(bool contact)
{
	lastFrameContactWithPlayer = contactWithPlayer;
	contactWithPlayer = contact;
}