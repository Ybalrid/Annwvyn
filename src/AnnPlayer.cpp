#include "stdafx.h"
#include "AnnPlayer.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

bodyParams::bodyParams()
{
	//these parameters looks good for testing. Customize them before initializing the physics!
	eyeHeight = 1.59f;
	walkSpeed = 3;
	turnSpeed = 0.15f;
	mass = 80.0f;
	FeetPosition = AnnVect3(0, 0, 10);
	HeadOrientation = AnnQuaternion(1, 0, 0, 0);
	Shape = NULL;
	Body = NULL;
	runFactor = 3;
}

AnnPlayer::AnnPlayer()
{
	AnnDebug() << "Creating player object";
	playerBody = new bodyParams;
	locked = false;
	run = false;

	walking[forward] = false;
	walking[backward] = false;
	walking[left] = false;
	walking[right] = false;

	analogWalk = 0;
	analogStraff = 0;
	analogRotate = 0;

	standing = true;
	updateTime = 0;
	physics = false;

	actuator = nullptr;
	setActuator(new AnnDefaultPlayerActuator);

	ignorePhysics = false;
}

void AnnPlayer::setActuator(AnnPlayerActuator* act)
{
	if (actuator)
	{
		delete actuator;
		actuator = nullptr;
	}

	act->player = this;
	actuator = act;
}

AnnPlayer::~AnnPlayer()
{
	delete playerBody;
}

bool AnnPlayer::isLocked()
{
	return locked;
}

void AnnPlayer::setPosition(AnnVect3 Position)
{
	playerBody->FeetPosition = Position;
}

void AnnPlayer::setOrientation(Ogre::Euler Orientation)
{
	playerBody->Orientation = Orientation;
}

void AnnPlayer::setHeadOrientation(AnnQuaternion Orientation)
{
	playerBody->HeadOrientation = Orientation;
}

void AnnPlayer::setEyesHeight(float eyeHeight)
{
	if (!isLocked())
		playerBody->eyeHeight = eyeHeight;
}

void AnnPlayer::setWalkSpeed(float walk)
{
	if (!isLocked())
		playerBody->walkSpeed = walk;
}

void AnnPlayer::setTurnSpeed(float ts)
{
	if (!isLocked())
		playerBody->turnSpeed = ts;
}

void AnnPlayer::setMass(float mass)
{
	playerBody->mass = mass;
}

void AnnPlayer::setShape(btCollisionShape* Shape)
{
	if (!isLocked())
		playerBody->Shape = Shape;
	physics = true;
}

void AnnPlayer::setBody(btRigidBody* Body)
{
	playerBody->Body = Body;
	physics = true;
}

void AnnPlayer::lockParameters()
{
	locked = true;
}

void AnnPlayer::unlockParameters()
{
	locked = true;
}

bodyParams* AnnPlayer::getLowLevelBodyParams()
{
	return playerBody;
}

float AnnPlayer::getWalkSpeed()
{
	return playerBody->walkSpeed;
}

float AnnPlayer::getEyesHeight()
{
	return playerBody->eyeHeight;
}

float AnnPlayer::getTurnSpeed()
{
	return playerBody->turnSpeed;
}

float AnnPlayer::getMass()
{
	return playerBody->mass;
}

btRigidBody* AnnPlayer::getBody()
{
	return playerBody->Body;
}

btCollisionShape* AnnPlayer::getShape()
{
	return playerBody->Shape;
}

AnnVect3 AnnPlayer::getPosition()
{
	return playerBody->FeetPosition;
}

Ogre::Euler AnnPlayer::getOrientation()
{
	return playerBody->Orientation;
}

void AnnPlayer::applyRelativeBodyYaw(Ogre::Radian angle)
{
	playerBody->Orientation.yaw(angle);
}

void AnnPlayer::applyMouseRelativeRotation(int relValue)
{
	applyRelativeBodyYaw(Ogre::Radian(-float(relValue) * getTurnSpeed() * updateTime));
}

AnnVect3 AnnPlayer::getTranslation()
{
	AnnVect3 translation(AnnVect3::ZERO);
	if (walking[forward])
		translation.z -= 1;
	if (walking[backward])
		translation.z += 1;
	if (walking[left])
		translation.x -= 1;
	if (walking[right])
		translation.x += 1;

	return translation.normalisedCopy();
}

AnnVect3 AnnPlayer::getAnalogTranslation()
{
	AnnVect3 translate(AnnVect3::ZERO);

	translate.x = analogStraff;
	translate.z = analogWalk;

	return translate;
}

void AnnPlayer::applyAnalogYaw()
{
	//7 is the value that was more or less feeling good for me.
	applyRelativeBodyYaw(Ogre::Radian(-7 * analogRotate * getTurnSpeed() * updateTime));
}

float AnnPlayer::getRunFactor()
{
	return playerBody->runFactor;
}

void AnnPlayer::resetPlayerPhysics()
{
	if (!hasPhysics()) return;
	AnnDebug("Reset player's physics");

	//Remove the player's rigid-body from the world
	AnnGetPhysicsEngine()->getWorld()->removeRigidBody(getBody());

	//We don't need that body anymore...
	delete getBody();

	//prevent memory access to unallocated address
	setBody(NULL);

	//Put everything back in order
	AnnGetEngine()->syncPov();
	AnnGetPhysicsEngine()->createPlayerPhysicalVirtualBody(AnnGetEngine()->getPlayerPovNode());
	AnnGetPhysicsEngine()->addPlayerPhysicalBodyToDynamicsWorld();
}

void AnnPlayer::teleport(AnnVect3 position, AnnRadian orientation)
{
	setPosition(position);
	setOrientation(Ogre::Euler(orientation));
	resetPlayerPhysics();
}

void Annwvyn::AnnPlayer::teleport(AnnVect3 position)
{
	teleport(position, this->getOrientation().getYaw());
}

void AnnPlayer::engineUpdate(float deltaTime)
{
	if (ignorePhysics) return;
	updateTime = deltaTime;

	if (getBody())
	{
		applyAnalogYaw();
		getBody()->activate();
	}

	//Tell the actuator to "act" on the player
	actuator->actuate(deltaTime);

	//get back position data from physics engine
	if (getBody())
	{
		setPosition(AnnVect3(getBody()->getCenterOfMassPosition()) -
					AnnQuaternion(getBody()->getCenterOfMassTransform().getRotation()) *
					AnnVect3(0, getEyesHeight() / 2, 0));
	}
}

bool AnnPlayer::hasPhysics()
{
	return physics;
}