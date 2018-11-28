// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnPlayerBody.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

const AnnVect3 AnnPlayerBody::DEFAULT_STARTING_POS { 0, 1, 10 };
const Ogre::Euler AnnPlayerBody::DEFAULT_STARTING_ORIENT { 0 };

AnnPlayerBody::PhysicalParameters::PhysicalParameters() :
 eyeHeight { 1.59f },
 walkSpeed { 3 },
 runFactor { 3 },
 turnSpeed { 0.15f },
 mass { 80 },
 FeetPosition { 0, 0, 10 },
 RoomBase { FeetPosition },
 HeadOrientation { AnnQuaternion::IDENTITY },
 Shape { nullptr },
 Body { nullptr }

{}

AnnPlayerBody::AnnPlayerBody()
{
	AnnDebug() << "Creating player object";
	locked = false;
	run	= false;

	walking[forward]  = false;
	walking[backward] = false;
	walking[left]	 = false;
	walking[right]	= false;

	analogWalk   = 0;
	analogStraff = 0;
	analogRotate = 0;

	standing   = true;
	updateTime = 0;
	physics	= false;

	setActuator<AnnDefaultPlayerActuator>();

	ignorePhysics = false;

	mouseSensitivity			  = 3;
	contactWithGround			  = false;
	RoomReferenceNode			  = nullptr;
	needNewRoomTranslateReference = false;

	mode = UNKNOWN;
}

void AnnPlayerBody::setActuator(std::unique_ptr<AnnPlayerActuator>&& act)
{
	actuator = std::move(act);
}

bool AnnPlayerBody::isLocked() const
{
	return locked;
}

void AnnPlayerBody::setPosition(AnnVect3 Position)
{
	physicsParams.FeetPosition = Position;
}

void AnnPlayerBody::setOrientation(Ogre::Euler Orientation)
{
	physicsParams.Orientation = Orientation;
}

void AnnPlayerBody::setOrientation(AnnQuaternion Orientation)
{
	Ogre::Euler e;
	e.fromQuaternion(static_cast<Ogre::Quaternion>(Orientation));
	setOrientation(e);
}

void AnnPlayerBody::setHeadOrientation(AnnQuaternion Orientation)
{
	physicsParams.HeadOrientation = Orientation;
}

void AnnPlayerBody::setEyesHeight(float eyeHeight)
{
	if(!isLocked())
		physicsParams.eyeHeight = eyeHeight;
}

void AnnPlayerBody::setWalkSpeed(float walk)
{
	if(!isLocked())
		physicsParams.walkSpeed = walk;
}

void AnnPlayerBody::setTurnSpeed(float ts)
{
	if(!isLocked())
		physicsParams.turnSpeed = ts;
}

void AnnPlayerBody::setMass(float mass)
{
	physicsParams.mass = mass;
}

void AnnPlayerBody::setShape(btCollisionShape* Shape)
{
	if(!isLocked())
		physicsParams.Shape = Shape;
	physics = true;
}

void AnnPlayerBody::setBody(btRigidBody* Body)
{
	physicsParams.Body = Body;
	physics			   = true;
}

void AnnPlayerBody::lockParameters()
{
	locked = true;
}

void AnnPlayerBody::unlockParameters()
{
	locked = false;
}

float AnnPlayerBody::getWalkSpeed() const
{
	return physicsParams.walkSpeed;
}

float AnnPlayerBody::getEyesHeight() const
{
	return physicsParams.eyeHeight;
}

AnnVect3 AnnPlayerBody::getEyeTranslation() const
{
	return getEyesHeight() * AnnVect3::UNIT_Y;
}

float AnnPlayerBody::getTurnSpeed() const
{
	return physicsParams.turnSpeed;
}

float AnnPlayerBody::getMass() const
{
	return physicsParams.mass;
}

btRigidBody* AnnPlayerBody::getBody() const
{
	return physicsParams.Body;
}

btCollisionShape* AnnPlayerBody::getShape() const
{
	return physicsParams.Shape;
}

AnnVect3 AnnPlayerBody::getPosition() const
{
	return physicsParams.FeetPosition;
}

Ogre::Euler AnnPlayerBody::getOrientation() const
{
	return physicsParams.Orientation;
}

void AnnPlayerBody::applyRelativeBodyYaw(Ogre::Radian angle)
{
	if(mode == STANDING)
		physicsParams.Orientation.yaw(angle);
	else if(mode == ROOMSCALE)
	{
		//Projection of the headset world position on the ground plane. we are turning around this point.
		AnnVect3 basePoint {
			AnnGetVRRenderer()->trackedHeadPose.position.x,
			physicsParams.RoomBase.y,
			AnnGetVRRenderer()->trackedHeadPose.position.z
		};

		physicsParams.Orientation.yaw(angle);
		AnnVect3 displacement = physicsParams.RoomBase - basePoint;
		physicsParams.RoomBase -= displacement;

		AnnQuaternion rotation(angle, AnnVect3::UNIT_Y);
		displacement = rotation * displacement;
		physicsParams.RoomBase += displacement;

		roomTranslateQuatReference = AnnQuaternion(rotation * roomTranslateQuatReference);
	}
}

void AnnPlayerBody::applyMouseRelativeRotation(int relValue)
{
	relValue = int(relValue * mouseSensitivity);
	applyRelativeBodyYaw(Ogre::Radian(-float(relValue) * getTurnSpeed() * updateTime));
}

AnnVect3 AnnPlayerBody::getTranslation()
{
	AnnVect3 translation(AnnVect3::ZERO);
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

AnnVect3 AnnPlayerBody::getAnalogTranslation() const
{
	AnnVect3 translate(AnnVect3::ZERO);

	translate.x = analogStraff;
	translate.z = analogWalk;

	return translate;
}

void AnnPlayerBody::applyAnalogYaw()
{
	//7 is the value that was more or less feeling good for me.
	auto value = -7 * analogRotate * getTurnSpeed() * updateTime;
	applyRelativeBodyYaw(Ogre::Radian(value));
}

float AnnPlayerBody::getRunFactor() const
{
	return physicsParams.runFactor;
}

void AnnPlayerBody::resetPlayerPhysics()
{
	if(!hasPhysics()) return;
	AnnDebug("Reset player's physics");

	//Remove the player's rigid-body from the world
	AnnGetPhysicsEngine()->getWorld()->removeRigidBody(getBody());

	//We don't need that body anymore...
	delete getBody();

	//prevent memory access to unallocated address
	setBody(nullptr);

	//Put everything back in order
	AnnGetEngine()->syncPalyerPov();
	AnnGetPhysicsEngine()->createPlayerPhysicalVirtualBody(AnnGetEngine()->getPlayerPovNode());
	AnnGetPhysicsEngine()->addPlayerPhysicalBodyToDynamicsWorld();
}

void AnnPlayerBody::teleport(AnnVect3 position, AnnRadian orientation)
{
	setPosition(position);
	setOrientation(Ogre::Euler(orientation));
	resetPlayerPhysics();
}

void AnnPlayerBody::teleport(AnnVect3 position)
{
	teleport(position, this->getOrientation().getYaw());
}

void AnnPlayerBody::engineUpdate(float deltaTime)
{
	static AnnVect3 roomTranslation;
	updateTime = deltaTime;
	switch(mode)
	{
		case STANDING:
			if(ignorePhysics) return;

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
				setPosition(AnnVect3(getBody()->getCenterOfMassPosition()) - AnnQuaternion(getBody()->getCenterOfMassTransform().getRotation()) * AnnVect3(0, getEyesHeight() / 2, 0));
			}
			break;

		case ROOMSCALE:
			if(ignorePhysics) return;
			applyAnalogYaw();

			if(needNewRoomTranslateReference)
				roomTranslateQuatReference = AnnQuaternion { AnnGetVRRenderer()->trackedHeadPose.orientation.getYaw(), AnnVect3::UNIT_Y };

			roomTranslation = updateTime * getWalkSpeed() *
				//(physicsParams.Orientation.toQuaternion() *
				(roomTranslateQuatReference * (getTranslation() + getAnalogTranslation()));

			if(roomTranslation == AnnVect3::ZERO)
				needNewRoomTranslateReference = true; //New base will be calculated at next frame
			else
			{
				needNewRoomTranslateReference = false;
				physicsParams.RoomBase += roomTranslation;
			}

			syncToTrackedPose();

			break;

		default: break;
	}
}

bool AnnPlayerBody::hasPhysics() const
{
	return physics;
}

void AnnPlayerBody::setMode(AnnPlayerMode playerMode)
{
	mode = playerMode;
}

void AnnPlayerBody::setRoomRefNode(Ogre::SceneNode* node)
{
	RoomReferenceNode = node;
}

void AnnPlayerBody::reground(float YvalueForGround)
{
	if(mode != ROOMSCALE) return;
	physicsParams.FeetPosition.y = YvalueForGround;

	AnnDebug() << "Re-grounding to Y=" << YvalueForGround;
}

void AnnPlayerBody::reground(AnnVect3 pointOnGround)
{
	reground(pointOnGround.y);
}

void AnnPlayerBody::regroundOnPhysicsBody(float length, AnnVect3 preoffset)
{
	if(mode != ROOMSCALE) return;
	AnnVect3 rayOrigin { physicsParams.FeetPosition + preoffset };
	AnnVect3 rayEndPoint { rayOrigin + length * AnnVect3::NEGATIVE_UNIT_Y };

	btCollisionWorld::ClosestRayResultCallback rayGroundingCallback(rayOrigin.getBtVector(),
																	rayEndPoint.getBtVector());

	AnnGetPhysicsEngine()->getWorld()->rayTest(rayOrigin.getBtVector(),
											   rayEndPoint.getBtVector(),
											   rayGroundingCallback);

	if(rayGroundingCallback.hasHit())
		reground(rayGroundingCallback.m_hitPointWorld);
}

void AnnPlayerBody::_hintRoomscaleUpdateTranslationReference()
{
	roomTranslateQuatReference = true;
}

void AnnPlayerBody::syncToTrackedPose() const
{
	if(mode != ROOMSCALE) return;
	if(physicsParams.Body)
	{
		btTransform transform;
		auto pose = AnnGetVRRenderer()->trackedHeadPose;
		transform.setOrigin(pose.position.getBtVector());
		transform.setRotation(pose.orientation.getBtQuaternion());
		physicsParams.Body->setAngularVelocity(btVector3(0, 0, 0));
		physicsParams.Body->setLinearVelocity(btVector3(0, 0, 0));
		physicsParams.Body->setWorldTransform(transform);
		physicsParams.Body->activate(true);
	}
}
