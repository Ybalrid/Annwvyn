//Pre-compiled header are used in this project
#include "stdafx.h"

//Include Annwvyn Engine.
#include <Annwvyn.h>

//Every Annwvyn classes are in the Annwvyn namespace
using namespace Annwvyn;

//Include our level/stages here
#include "myLevel.hpp"

//This main function job here is to init the engine,
//create and load one 'Level' object,
//and set to use the built in "event listener" for controls

//All the resources here have been put in the "media" directory
//that is auto loaded by Annwvyn

AnnMain() //The application entry point is "AnnMain()". return type int.
{
	//AnnEngine::openConsole(); //optional : open console in terminal window;

	//Initialize the engine
	AnnInit("MyVRGame");

	//Call physics initialization for the player body:
	AnnGetEngine()->initPlayerRoomscalePhysics(); //There's an optional "room scale" physics mode too, but it's pretty beta right now.

	//Instantiate and register our basic level and "jump" to it:
	AnnGetLevelManager()->addLevel<MyLevel>();
	AnnGetLevelManager()->jumpToFirstLevel();

	//Use the default event listener. It's a commodity for development, you'll probably want to write your own.
	AnnGetEventManager()->useDefaultEventListener();

	//The game runs here
	AnnGetEngine()->startGameplayLoop();

	//Destroy the engine now. Engine is RAII managed, but you can manually release it with this command
	AnnQuit();

	return EXIT_SUCCESS;
}