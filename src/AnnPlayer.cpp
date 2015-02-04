#include "AnnPlayer.hpp"

using namespace Annwvyn;

bodyParams::bodyParams()
{
	//these parameters looks good for testing. Costumise them before initializing the physics!
	eyeHeight = 1.59f;
	walkSpeed = 3.0f;
	turnSpeed = 0.003f;
	mass = 80.0f;
	Position = Ogre::Vector3(0,0,10);
	HeadOrientation = Ogre::Quaternion(1,0,0,0);
	Shape = NULL;
	Body = NULL;
}

AnnPlayer::AnnPlayer()
{
	playerBody = new bodyParams;
	locked = false;
}

AnnPlayer::~AnnPlayer()
{
	delete playerBody;
}

bool AnnPlayer::isLocked()
{
	return locked;
}

void AnnPlayer::setPosition(Ogre::Vector3 Position)
{
		playerBody->Position = Position;
}

void AnnPlayer::setOrientation(Ogre::Euler Orientation)
{
		playerBody->Orientation = Orientation;
}

void AnnPlayer::setHeadOrientation(Ogre::Quaternion Orientation)
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
}

void AnnPlayer::setBody(btRigidBody* Body)
{
	if(!isLocked())
		playerBody->Body = Body;
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

Ogre::Vector3 AnnPlayer::getPosition()
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