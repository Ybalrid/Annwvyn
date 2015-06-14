//Precompiled header are used in this project
#include "stdafx.h"

//Include Annwvyn Engine.
#include <Annwvyn.h>
//Every Annwvyn classes are in the Annwvyn namespace
using namespace Annwvyn; 

//Include our level/stages here
#include "myLevel.hpp"

AnnMain() //The application entry point is "AnnMain()". return type int.
{
	//Initialize the engine
	new AnnEngine("A game using Annwvyn");
	
	//Load your ressources here
	AnnEngine::Instance()->initResources();
	AnnEngine::Instance()->oculusInit();
	//If the player has to folow the integrated physics scheme
	AnnEngine::Instance()->initPlayerPhysics();
	//Do the other initialization here
	AnnEngine::Instance()->useDefaultEventListener(); //Basic events

	//Intentiate and register our basic level
	AnnEngine::Instance()->getLevelManager()->addLevel(new MyLevel);
	//This will make the game load and start the 1st we have registered
	AnnEngine::Instance()->getLevelManager()->jumpToFirstLevel();

	//This will recenter the facing direction of the player
	//in the real world with the VR
	AnnEngine::Instance()->resetOculusOrientation();
	
	//The game is rendering here now:
	AnnEngine::Instance()->startGameplayLoop();

	//destroy the engine
	delete AnnEngine::Instance();
	return EXIT_SUCCESS;
}

