#include "stdafx.h"
#include "AnnPlayerActuator.hpp"
#include "AnnPlayer.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnPlayerActuator::AnnPlayerActuator()
{
	player = nullptr;
}

AnnDefaultPlayerActuator::AnnDefaultPlayerActuator() : AnnPlayerActuator()
{
}

void AnnDefaultPlayerActuator::actuate(float delta)
{
	if(!player->getBody()) return;
	//Get WASD or Gamepad joystick tranlation vector
	AnnVect3 translate(player->getWalkSpeed() * (player->getTranslation() + player->getAnalogTranslation()));
	//Get current linear velocity
	btVector3 currentVelocity = player->getBody()->getLinearVelocity();
	 
	//if no  user input, be just pull toward the ground by gravity (not physicly realist, but usefull)
	if(translate.isZeroLength())
	{
		player->getBody()->setLinearVelocity(btVector3(0, currentVelocity.y(), 0));
	}
	else
	{
		//Calculate the direction of the velocity vector in global space
		AnnVect3 velocity(player->getOrientation()*translate);
		if(player->run) velocity *= player->getRunFactor(); //If the player is runing, go faster.

		//Apply it to player's rigid body
		player->getBody()->setLinearVelocity(btVector3(velocity.x, currentVelocity.y(), velocity.z));
	}

	//if the player can stand (= not dead or something like that)
	if(player->standing) 
	{
		//Block it's orientation to the Identity quaternion in the rigid body transform
		btTransform Transform = player->getBody()->getCenterOfMassTransform();
		Transform.setRotation(AnnQuaternion(0, 0, 0, 1).getBtQuaternion());
		player->getBody()->setCenterOfMassTransform(Transform);
	}

}
