#include "stdafx.h"
#include "AnnTriggerObject.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

btCollisionShape* AnnTriggerObjectShapeGenerator::box(const float& w, const float& h, const float &l)
{
	AnnVect3 half = AnnVect3(w, h, l) / 2.f;
	return new btBoxShape(half.getBtVector());
}

btCollisionShape* AnnTriggerObjectShapeGenerator::sphere(const float& r)
{
	return new btSphereShape(r);
}

AnnTriggerObject::AnnTriggerObject() :
	contactWithPlayer(false),
	lastFrameContactWithPlayer(false),
	body(nullptr),
	shape(nullptr)
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

void AnnTriggerObject::setShape(btCollisionShape* shp)
{
	if (shape)
	{
		delete shape;
		shape = nullptr;
	}
	if (body)
	{
		AnnGetPhysicsEngine()->removeRigidBody(body);
		delete body;
		body = nullptr;
	}

	shape = shp;
	body = new btRigidBody(0, nullptr, shape);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	body->setUserPointer(static_cast<void*>(this));
}

void AnnTriggerObject::setContactInformation(bool contact)
{
	lastFrameContactWithPlayer = contactWithPlayer;
	contactWithPlayer = contact;
}