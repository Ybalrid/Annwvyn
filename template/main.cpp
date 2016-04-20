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
	//AnnEngine::openConsole();//optional : open console
	new AnnEngine("A game using Annwvyn");
	
	//Load your custom graphics resources here:
	//e.g.:
	//AnnEngine::Instance()->loadDir("relativePathToDirectory");
	//AnnEngine::Instance()->loadZip("relativePathToZipedFile");
	
	AnnEngine::Instance()->initResources();
	AnnEngine::Instance()->oculusInit();

	//Call physics initialization for the player body:
	AnnEngine::Instance()->initPlayerPhysics();

	//Intentiate and register our basic level and "jump" to it:
	AnnEngine::Instance()->getLevelManager()->addLevel(new MyLevel);
	AnnEngine::Instance()->getLevelManager()->jumpToFirstLevel();
	
	//This will recenter the facing direction of the player

	//in the real world with the VR
	AnnEngine::Instance()->resetOculusOrientation();
	//The game is rendering here now:
	AnnEngine::Instance()->useDefaultEventListener();
	AnnEngine::Instance()->startGameplayLoop();

	//destroy the engine
	delete AnnEngine::Instance();
	return EXIT_SUCCESS;
}

