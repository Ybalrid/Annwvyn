// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

AnnTriggerObject::AnnTriggerObject(const std::string& name) :
	name(name),
	contactWithPlayer(false),
	lastFrameContactWithPlayer(false),
	body(nullptr),
	shape(nullptr)
{
}

std::string AnnTriggerObject::getName() const
{
	return name;
}

AnnTriggerObject::~AnnTriggerObject()
{
	AnnDebug() << "AnnTriggerObject destructor called";
	AnnGetPhysicsEngine()->removeRigidBody(body.get());
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
		shape = nullptr;
	}
	if (body)
	{
		AnnGetPhysicsEngine()->removeRigidBody(body.get());
		body = nullptr;
	}

	shape.reset(shp);
	body = std::make_unique<btRigidBody>(0.f, nullptr, shape.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	body->setUserPointer(static_cast<void*>(this));
	AnnGetPhysicsEngine()->getWorld()->addRigidBody(body.get());
}

void AnnTriggerObject::setContactInformation(bool contact)
{
	lastFrameContactWithPlayer = contactWithPlayer;
	contactWithPlayer = contact;
}
