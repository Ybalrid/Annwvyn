// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnPlayerActuator.hpp"
#include "AnnPlayerBody.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnPlayerActuator::AnnPlayerActuator()
{
	player = nullptr;
}

AnnDefaultPlayerActuator::AnnDefaultPlayerActuator() :
 AnnPlayerActuator()
{
}

void AnnDefaultPlayerActuator::actuate(float delta)
{
	if(delta < 0) return;
	if(!player->getBody()) return;

	//Get WASD or Gamepad joystick translation vector
	AnnVect3 translate(player->getWalkSpeed() * (player->getTranslation() + player->getAnalogTranslation()));
	//AnnDebug() << "Translation : " << translate;

	//Get current linear velocity
	auto currentVelocity = player->getBody()->getLinearVelocity();
	//AnnDebug() << "CurentVelocity" << currentVelocity;

	//if no  user input, be just pull toward the ground by gravity (not physically realist, but useful)
	if(translate.isZeroLength())
	{
		player->getBody()->setLinearVelocity(btVector3(0, currentVelocity.y(), 0));
	}
	else
	{
		//Calculate the direction of the velocity vector in global space
		AnnVect3 velocity(player->getOrientation() * translate);
		//AnnDebug() << "Should apply this planar velocity : " << velocity;
		if(player->run) velocity *= player->getRunFactor(); //If the player is running, go faster.

		//Apply it to player's rigid body
		player->getBody()->setLinearVelocity(btVector3(velocity.x, currentVelocity.y(), velocity.z));
	}

	//if the player can stand (= not dead or something like that)
	if(player->standing)
	{
		//Block it's orientation to the Identity quaternion in the rigid body transform
		auto Transform = player->getBody()->getCenterOfMassTransform();
		Transform.setRotation(AnnQuaternion(0, 0, 0, 1).getBtQuaternion());
		player->getBody()->setCenterOfMassTransform(Transform);
	}
}

AnnPlayerActuator::~AnnPlayerActuator() {}
