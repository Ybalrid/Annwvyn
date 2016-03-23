#include "stdafx.h"
#include "myLevel.hpp"

MyLevel::MyLevel() : AnnAbstractLevel()
{
}

void MyLevel::load()
{
	//For having a lighter syntax :
	auto engine(AnnEngine::Instance());
	
	//Load Sinbad:
	auto Sinbad (addGameObject("Sinbad.mesh"));
	Sinbad->setUpPhysics(100, phyShapeType::boxShape);

	//Load Ground:
	auto Ground (addGameObject("Ground.mesh"));
	Ground->setPosition(0,-2,0);
	Ground->setUpPhysics();

	//Create a light source
	auto light(addLight());
	light->setPosition(AnnVect3(0,1,3));

	engine->setAmbiantLight(AnnColor(.5,.5,.5));
}

void MyLevel::runLogic()
{
}