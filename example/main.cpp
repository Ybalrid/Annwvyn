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

//Annwvyn
#include <Annwvyn.h>
#include "TestLevel.hpp"

using namespace std;
using namespace Annwvyn;

class DebugListener : LISTENER
{
public:
	DebugListener() : constructListener()
	{
	}

	void TimeEvent(AnnTimeEvent e)
	{
		AnnDebug() << "TimeEvent id : " << e.getID() << " over " << SIZE_MAX ;
		AnnEngine::Instance()->getEventManager()->fireTimer(1000);
	}
};

AnnMain()
{
	//Only usefull on windows : Open a debug console to get stdout/stderr
	AnnEngine::openConsole();	
	//Init game engine
	AnnEngine* GameEngine(new AnnEngine("Test program"));

	//load ressources
	GameEngine->loadDir("media/environement");
	GameEngine->loadDir("media/debug");
	GameEngine->initResources();

	//SetUp Oculus system	
	GameEngine->oculusInit();
	GameEngine->setNearClippingDistance(0.20f);
	
	//Init some player body parameters
	GameEngine->initPlayerPhysics();
	GameEngine->attachVisualBody("male_Body.mesh",-0.1f ,true);
	
	//Register a level
	GameEngine->getLevelManager()->addLevel(new TestLevel);	//This is the first level known by the LevelManager (and the only one here)
	GameEngine->getLevelManager()->jumpToFirstLevel();		//Jump to that level 
	
	GameEngine->useDefaultEventListener();
	GameEngine->resetOculusOrientation();
	GameEngine->getEventManager()->addListener(new DebugListener);
	GameEngine->getEventManager()->fireTimer(10000);

	AnnDebug() << "Starting the render loop";
	do	
	{
		if(GameEngine->isKeyDown(OIS::KC_Q))
			GameEngine->getLevelManager()->unloadCurrentLevel();
		if(GameEngine->isKeyDown(OIS::KC_E))
			GameEngine->getLevelManager()->jumpToFirstLevel();	
	}
	while(GameEngine->refresh());

	delete GameEngine;
	return EXIT_SUCCESS;
}
