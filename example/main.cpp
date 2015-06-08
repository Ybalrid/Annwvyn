/**
* * \file main.cpp
* \brief test/demo program
* \author Arthur Brainville 
* \version 0.5
* \date 06/11/14
*
* Annwvyn test program http://annwvyn.org/
*
*/

#include "stdafx.h"

//C++ includes
#include <iostream>
//Annwvyn
#include <Annwvyn.h>

using namespace std;
using namespace Annwvyn;


class Sinbad : public AnnGameObject
{
public:
	void postInit()
	{
		setPos(0,0,-5);
		setScale(0.2f,0.2f,0.2f);
		setAnimation("Dance");
		playAnimation(true);
		loopAnimation(true);
		setUpPhysics(40, phyShapeType::boxShape);
	}
};

AnnMain()
{
	AnnEngine::openConsole();	//Only usefull on windows : Open a debug console 
	//Init game engine
	AnnEngine* GameEngine(new AnnEngine("Test program"));
	//load ressources
	GameEngine->loadDir("media/dome");
	GameEngine->loadZip("media/Sinbad.zip");
	GameEngine->loadDir("media/plane");
	GameEngine->loadDir("media/body");

	AnnEngine::Instance()->loadDir("media/environement");

	GameEngine->initResources();
	//setUp Oculus system
	GameEngine->oculusInit();
	GameEngine->setNearClippingDistance(0.20f);
	AnnEngine::Instance()->createGameObject("Island.mesh")->setUpBullet();
	AnnEngine::Instance()->createGameObject("Water.mesh");
	AnnGameObject* Sign(AnnEngine::Instance()->createGameObject("Sign.mesh"));

	Sign->setPos(1,-0,-2);
	Sign->setUpPhysics(0, phyShapeType::staticShape);
	Sign->setOrientation(Ogre::Quaternion(Ogre::Degree(-45), Ogre::Vector3::UNIT_Y));
	
	AnnLightObject* Sun = GameEngine->addLight();
	Sun->setType(Ogre::Light::LT_DIRECTIONAL);
	Sun->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y + 1.5* Ogre::Vector3::NEGATIVE_UNIT_Z);
	GameEngine->setAmbiantLight(Ogre::ColourValue(.6f,.6f,.6f));
	//GameEngine->setSkyDomeMaterial(true,"Sky/dome1");

	GameEngine->initPlayerPhysics();
	GameEngine->setDebugPhysicState(true);



	GameEngine->attachVisualBody("male_Body.mesh",-0.1f ,true);
	AnnGameObject* S = GameEngine->createGameObject("Sinbad.mesh", new Sinbad);

    GameEngine->useDefaultEventListener();
	GameEngine->resetOculusOrientation();

	AnnEngine::Instance()->getAudioEngine()->playBGM("media/bgm/bensound-happyrock.ogg");
	AnnEngine::log("Starting the render loop");

	int awnser(42);
	int* address(&awnser);
	AnnDebug() << "This is a test " << awnser << " " << address;

	bool current(false), last(false);
	do	
	{
		
		current = AnnEngine::Instance()->isKeyDown(OIS::KC_Q);
		if(!current && last)
			S->playSound("media/monster.wav",false, 1.5);
		last = current;
	}
	while(GameEngine->refresh());

	delete GameEngine;
	return 0;
}
