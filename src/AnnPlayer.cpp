#include "stdafx.h"
#include "AnnPlayer.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

bodyParams::bodyParams() :
	RoomBase{ FeetPosition }
{
	//these parameters looks good for testing. Customize them before initializing the physics!
	eyeHeight = 1.59f;
	walkSpeed = 3;
	turnSpeed = 0.15f;
	mass = 80.0f;
	FeetPosition = AnnVect3(0, 0, 10);
	HeadOrientation = AnnQuaternion::IDENTITY;
	Shape = nullptr;
	Body = nullptr;
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

AnnVect3 AnnPlayer::getEyeTranslation()
{
	return getEyesHeight() * AnnVect3::UNIT_Y;
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
	if (mode == STANDING)
		playerBody->Orientation.yaw(angle);
	else if (mode == ROOMSCALE)
	{
		//Projection of the headset world position on the ground plane. we are turning around this point.
		AnnVect3 basePoint
		{
			AnnGetVRRenderer()->trackedHeadPose.position.x,
			playerBody->RoomBase.y,
			AnnGetVRRenderer()->trackedHeadPose.position.z
		};

		//AnnDebug() << "angle : " << angle;
		//AnnDebug() << "basepoint " << basePoint;

		playerBody->Orientation.yaw(angle);
		AnnVect3 displacement = playerBody->RoomBase - basePoint;
		//AnnDebug() << "displacement :" << displacement;
		playerBody->RoomBase -= displacement;

		AnnQuaternion rotation(angle, AnnVect3::UNIT_Y);
		displacement = rotation*displacement;
		playerBody->RoomBase += displacement;

		//AnnDebug() << "displacement :" << displacement;
	}
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
	float  value = -7 * analogRotate * getTurnSpeed() * updateTime;
	applyRelativeBodyYaw(Ogre::Radian(value));
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
	setBody(nullptr);

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

void AnnPlayer::teleport(AnnVect3 position)
{
	teleport(position, this->getOrientation().getYaw());
}

void AnnPlayer::engineUpdate(float deltaTime)
{
	updateTime = deltaTime;
	switch (mode)
	{
		case STANDING:
			if (ignorePhysics) return;

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
			break;

		case ROOMSCALE:
			applyAnalogYaw();

			playerBody->RoomBase += updateTime*getWalkSpeed() *
				(playerBody->Orientation.toQuaternion() *
				(getTranslation() + getAnalogTranslation()));
			break;

		default:break;
	}
}

bool AnnPlayer::hasPhysics()
{
	return physics;
}

void AnnPlayer::setMode(AnnPlayerMode playerMode)
{
	mode = playerMode;
}

void AnnPlayer::setRoomRefNode(Ogre::SceneNode* node)
{
	RoomReferenceNode = node;
	/*auto gizmo = node->createChildSceneNode();
	gizmo->attachObject(AnnGetEngine()->getSceneManager()->createEntity("gizmo.mesh"));
	gizmo->setScale(4, 4, 4);*/
}

void AnnPlayer::reground(float YvalueForGround)
{
	if (mode != ROOMSCALE) return;
	playerBody->FeetPosition.y = YvalueForGround;

	AnnDebug() << "Re-grounding to Y=" << YvalueForGround;
}

void AnnPlayer::reground(AnnVect3 pointOnGround)
{
	reground(pointOnGround.y);
}

void AnnPlayer::regroundOnPhysicsBody(float length, AnnVect3 preoffset)
{
	if (mode != ROOMSCALE) return;
	AnnVect3 rayOrigin{ playerBody->FeetPosition + preoffset };
	AnnVect3 rayEndPoint{ rayOrigin + length * AnnVect3::NEGATIVE_UNIT_Y };

	btCollisionWorld::ClosestRayResultCallback rayGroundingCallback(rayOrigin.getBtVector(),
																	rayEndPoint.getBtVector());

	AnnGetPhysicsEngine()->getWorld()->rayTest(rayOrigin.getBtVector(),
											   rayEndPoint.getBtVector(),
											   rayGroundingCallback);

	if (rayGroundingCallback.hasHit())
		reground(rayGroundingCallback.m_hitPointWorld);
}