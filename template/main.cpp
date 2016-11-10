//Pre-compiled header are used in this project
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
	AnnEngine::openConsole();//optional : open console
	AnnInit("NameOfApp");

	//Call physics initialization for the player body:
	AnnGetEngine()->initPlayerPhysics();

	//Instantiate and register our basic level and "jump" to it:
	AnnGetLevelManager()->addLevel(std::make_shared<MyLevel>());
	AnnGetLevelManager()->jumpToFirstLevel();

	//The game is rendering here now:
	AnnGetEventManager()->useDefaultEventListener();

	//The game runs here
	AnnGetEngine()->startGameplayLoop();

	//Destroy the engine now. Engine is RAII managed, but you can manually release it with this command
	AnnQuit();

	return EXIT_SUCCESS;
}