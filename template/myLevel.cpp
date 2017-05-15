#include "stdafx.h"
#include "myLevel.hpp"

using namespace Annwvyn;

MyLevel::MyLevel() : constructLevel()
{
}

void MyLevel::load()
{
	//Create Sinbad the Ogre
	auto Sinbad(addGameObject("Sinbad.mesh"));
	Sinbad->setScale({ 0.3f, 0.3f, 0.3f });
	Sinbad->setPosition({ 0, 1, 0 });
	//Setting a non-null mass and a non static geometric primitive make this object
	//a dynamic physics object (it can move if forces are applied upon him)
	Sinbad->setUpPhysics(250, boxShape);

	//Load Ground:
	auto Ground(addGameObject("floorplane.mesh"));
	Ground->setPosition({ 0, 0, 0 });
	Ground->setUpPhysics();
    AnnGetPlayer()->regroundOnPhysicsBody();

	//Create a light source
	auto Sun(addLightObject("MySun"));
	Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
	Sun->setPower(97);

	auto OtherLight(addLightObject("Other Light"));
	OtherLight->setPower(50);
	OtherLight->setPosition({ 0, 1, 1 });

	//zenith sunlight
	Sun->setDirection(AnnVect3{ -1, -1.5f, -1 }.normalisedCopy());
}

void MyLevel::runLogic()
{
}
