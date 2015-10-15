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
	if(!player) return;

	AnnVect3 translate(player->getWalkSpeed() * (player->getTranslation() + player->getAnalogTranslation()));
	btVector3 currentVelocity = player->getBody()->getLinearVelocity();

	//if no  user input, be just pull toward the ground by gravity (not physicly realist, but usefull)
	if(translate.isZeroLength())
	{
		player->getBody()->setLinearVelocity(btVector3(0, currentVelocity.y(), 0));
	}
	else
	{
		AnnVect3 velocity(player->getOrientation()*translate);
		if(player->run) velocity *= player->getRunFactor();
		player->getBody()->setLinearVelocity(btVector3(velocity.x, currentVelocity.y(), velocity.z));
	}

	//if the player can stand (= not dead or something like that)
	if(player->standing) 
	{
		btTransform Transform = player->getBody()->getCenterOfMassTransform();
		Transform.setRotation(AnnQuaternion(0, 0, 0, 1).getBtQuaternion());
		player->getBody()->setCenterOfMassTransform(Transform);
	}

}
