#include "stdafx.h"
#include "myLevel.hpp"

MyLevel::MyLevel() : constructLevel()
{
}

void MyLevel::load()
{
	//Load Sinbad:
	auto Sinbad(addGameObject("Sinbad.mesh"));
	Sinbad->setScale({ 0.3f, 0.3f, 0.3f });
	Sinbad->setPosition({ 0,1,0 });
	Sinbad->setUpPhysics(250, boxShape);

	//Load Ground:
	auto Ground(addGameObject("Ground.mesh"));
	Ground->setPosition({ 0, 0, 0 });
	Ground->setUpPhysics();

	//Create a light source
	auto Sun(addLightObject());
	Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
	//zenith sunlight
	Sun->setDirection(AnnVect3{ -1, -1.5f, -1 }.normalisedCopy());

	AnnGetSceneryManager()->setAmbientLight(AnnColor(.25f, .25f, .25));
}

void MyLevel::runLogic()
{
}