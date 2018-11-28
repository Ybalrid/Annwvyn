#include "stdafx.h"
#include "myLevel.hpp"

using namespace Annwvyn;

//Appart from calling the base class coonstructor via "constructLevel()", this constructor doesn't do anything special
MyLevel::MyLevel() :
 constructLevel()
{
}

void MyLevel::load()
{
	//Create Sinbad the Ogre. Set it's scale and position
	auto Sinbad(addGameObject("Sinbad.mesh", "Sinbad"));
	Sinbad->setScale(0.5f * AnnVect3::UNIT_SCALE);
	Sinbad->setPosition({ 0, 1, 0 });

	//Give Sinbad a rigidbody. By entering an non-null mass and choosing a pysical shape
	//different than "static shape", you make an object be a "dynamic object" that will
	//move according to the physical forces applied to it.
	Sinbad->setupPhysics(250, boxShape);

	//Create the plan that serve as the goround.
	auto Ground(addGameObject("floorplane.mesh"));
	Ground->setPosition({ 0, 0, 0 });

	//Just calling "setupPhysics" makes the object completely static. This is great for anything that is part of the environment.
	Ground->setupPhysics();

	//This makes sure the feet of the player will be on the ground
	AnnGetPlayer()->regroundOnPhysicsBody();

	//Create a light source
	auto Sun(addLightObject("MySun"));

	//You can choose three types of light sources, point, directional or spot
	Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
	Sun->setDirection(AnnVect3 { -1, -1.5f, -1 }.normalisedCopy());
	Sun->setPower(97);

	//Add an additional light. By default, lighs are point lights
	auto OtherLight(addLightObject("OtherLight"));
	OtherLight->setPower(50);
	OtherLight->setPosition({ 0, 1, 1 });

	//Set the player position at level loading
	auto player { AnnGetPlayer() };
	player->setPosition({ 0, 1, 10 });
	player->setOrientation(0);
	player->resetPlayerPhysics();
}

void MyLevel::runLogic()
{
	//Here we aren't doing anything usefull
}
