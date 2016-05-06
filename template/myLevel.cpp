#include "stdafx.h"
#include "myLevel.hpp"

MyLevel::MyLevel() : AnnAbstractLevel()
{
}

void MyLevel::load()
{	
	//Load Sinbad:
	auto Sinbad (addGameObject("Sinbad.mesh"));
	Sinbad->setUpPhysics(100, phyShapeType::boxShape);

	//Load Ground:
	auto Ground (addGameObject("Ground.mesh"));
	Ground->setPosition(0,-2,0);
	Ground->setUpPhysics();

	//Create a light source
	auto light(addLightObject());
	light->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
	//zenith sunlight
	light->setDirection(AnnVect3(0,-1,0));

	AnnGetSceneryManager()->setAmbiantLight(AnnColor(.5,.5,.5));
}

void MyLevel::runLogic()
{
}