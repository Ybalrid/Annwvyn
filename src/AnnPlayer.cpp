#include "AnnPlayer.hpp"

using namespace Annwvyn;

bodyParams::bodyParams()
{
	//these parameters looks good for testing. Costumise them before initializing the physics!
	eyeHeight = 1.59f;
	walkSpeed = 3;
	turnSpeed = 0.005f;
	mass = 80.0f;
	Position = Ogre::Vector3(0,0,10);
	HeadOrientation = Ogre::Quaternion(1,0,0,0);
	Shape = NULL;
	Body = NULL;
	runFactor = 3;
	jumpForce = btVector3(0,500,0);
}

AnnPlayer::AnnPlayer()
{
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

void AnnPlayer::applyMouseRelativeRotation(int relValue)
{
	applyRelativeBodyYaw(Ogre::Radian(-float(relValue)*getTurnSpeed()));
}

Ogre::Vector3 AnnPlayer::getTranslation()
{

	Ogre::Vector3 translation (Ogre::Vector3::ZERO);
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

Ogre::Vector3 AnnPlayer::getAnalogTranslation()
{
	Ogre::Vector3 translate(Ogre::Vector3::ZERO);

	translate.x = analogStraff;
	translate.z = analogWalk;

	return translate;
}

void AnnPlayer::applyAnalogYaw()
{
	applyRelativeBodyYaw(-Ogre::Radian(10 * analogRotate * getTurnSpeed()));
}

void AnnPlayer::jump()
{
	if(!getBody()) return;
	if(contactWithGround)
		getBody()->applyCentralImpulse(playerBody->jumpForce);
}


#include "AnnEngine.hpp"
void AnnPlayer::engineUpdate(float time)
{
	bool standing = true;
	if(getBody())
	{
		applyAnalogYaw();
		Ogre::Vector3 translate(getWalkSpeed() * (getTranslation()+getAnalogTranslation()));

		btVector3 currentVelocity = getBody()->getLinearVelocity();

		//Prevent the rigid body to be put asleep by the physics engine
		getBody()->activate();

		//if no  user input, be just pull toward the ground by gravity (not physicly realist, but usefull)
		if(translate.isZeroLength())
		{
			getBody()->setLinearVelocity(btVector3(
				0,
				currentVelocity.y(),
				0
				));
		}
		else
		{
			Ogre::Vector3 velocity(getOrientation()*translate);
			if(run) velocity *= playerBody->runFactor;
			getBody()->setLinearVelocity(btVector3(
				velocity.x,
				currentVelocity.y(),
				velocity.z
				));
		}

		//if the player can stand (= not dead or something like that)
		if(standing) 
		{
			btTransform Transform = getBody()->getCenterOfMassTransform();
			Transform.setRotation(btQuaternion(0,0,0,1));
			getBody()->setCenterOfMassTransform(Transform);
		}

		//get back position data from physics engine
		setPosition(
			Ogre::Vector3( 
			getBody()->getCenterOfMassPosition().x(),
			getBody()->getCenterOfMassPosition().y() + getEyesHeight()/2,
			getBody()->getCenterOfMassPosition().z()
			));

	}


}
