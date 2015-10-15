#include "stdafx.h"
#include "AnnPlayer.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

bodyParams::bodyParams()
{
	//these parameters looks good for testing. Costumise them before initializing the physics!
	eyeHeight = 1.59f;
	walkSpeed = 3;
	turnSpeed = 0.5f;
	mass = 80.0f;
	Position = AnnVect3(0, 0, 10);
	HeadOrientation = AnnQuaternion(1, 0, 0, 0);
	Shape = NULL;
	Body = NULL;
	runFactor = 3;
	jumpForce = AnnVect3(0, 500, 0);
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
	
	for(size_t i(0); i < JMP_BUFFER; i++) YSpeedWasZero[i]=false;
	frameCount = 0;
	standing = true;
	updateTime = 0;
	physics = false;

	actuator = nullptr;
	setActuator (new AnnDefaultPlayerActuator);

}

void AnnPlayer::setActuator(AnnPlayerActuator* act)
{
	if(actuator) 
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
	playerBody->Position = Position;
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
	if(!isLocked())
		playerBody->eyeHeight = eyeHeight;
}

void AnnPlayer::setWalkSpeed(float walk)
{
	if(!isLocked())
		playerBody->eyeHeight = walk;
}

void AnnPlayer::setTurnSpeed(float ts)
{
	if(!isLocked())
		playerBody->eyeHeight = ts;
}

void AnnPlayer::setMass(float mass)
{
	if(!isLocked())
		playerBody->eyeHeight = mass;
}

void AnnPlayer::setShape(btCollisionShape* Shape)
{
	if(!isLocked())
		playerBody->Shape = Shape;
	physics = true;
}

void AnnPlayer::setBody(btRigidBody* Body)
{
//	if(!isLocked())
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
	return playerBody->Position;
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
	applyRelativeBodyYaw(Ogre::Radian(- float(relValue) * getTurnSpeed() * updateTime));
}

AnnVect3 AnnPlayer::getTranslation()
{
	AnnVect3 translation (AnnVect3::ZERO);
	if(walking[forward])
		translation.z -= 1;
	if(walking[backward])
		translation.z += 1;
	if(walking[left])
		translation.x -= 1;
	if(walking[right])
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
	applyRelativeBodyYaw(Ogre::Radian(- 7 * analogRotate * getTurnSpeed() * updateTime));
}

void AnnPlayer::jump()
{
	if(!getBody()) return;
	if(contactWithGround)
		getBody()->applyCentralImpulse(playerBody->jumpForce.getBtVector());
}

float AnnPlayer::getRunFactor()
{
	return playerBody->runFactor;
}

void AnnPlayer::engineUpdate(float deltaTime)
{
	updateTime = deltaTime;
	if(getBody())
	{
		applyAnalogYaw();
		getBody()->activate();
	}

	//Tell the actuator to "act" on the player
	actuator->actuate(deltaTime);
	
	//get back position data from physics engine
	if(getBody())
	{
		setPosition(
			AnnVect3( 
			getBody()->getCenterOfMassPosition().x(),
			getBody()->getCenterOfMassPosition().y() + getEyesHeight()/2,
			getBody()->getCenterOfMassPosition().z()
			));
	}
}

bool AnnPlayer::hasPhysics()
{
	return physics;
}
