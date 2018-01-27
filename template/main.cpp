//Pre-compiled header are used in this project
#include "stdafx.h"

//Include Annwvyn Engine.
#include <Annwvyn.h>

//Every Annwvyn classes are in the Annwvyn namespace
using namespace Annwvyn;

//Include our level here
#include "myLevel.hpp"

/*
 * This main funciton will do the following : 
 *		- Initialize the engine
 *		- Initialize the physics model that will be applied for the user's body (seated or roomscale VR)
 *		- Add a Level object (that describe a virtual environement and everything doable in it)
 *		- Jump the level manager to that level (this place the user in said level, you can have how many of them you want in an applictaion)
 *		- Use the default event listener. This handler some basic user inputs, you can create and use your own, and you can use multiple ones in parallel
 *		- Start the main loop of the game
 * 
 * Game resources present in the "media" subdirectory are automatically loaded by the engine, you can specify your own resource locations
 * Theses locations can be simple directories, or zip files.
 */
AnnMain() //The application entry point is "AnnMain()". return type int.
{
	//AnnEngine::openConsole(); //optional : open console in terminal window;

	//Initialize the engine
	AnnInit("MyVRGame");

	//Call physics initialization for the player body:
	AnnGetEngine()->initPlayerRoomscalePhysics(); //There's an optional "room scale" physics mode too, but it's pretty beta right now.

	//Instantiate and register our basic level and "jump" to it:
	AnnGetLevelManager()->addLevel<MyLevel>();
	AnnGetLevelManager()->switchToFirstLoadedLevel();

	//Use the default event listener. It's a commodity for development, you'll probably want to write your own.
	AnnGetEventManager()->useDefaultEventListener();

	//The game runs here
	AnnGetEngine()->startGameplayLoop();

	//Destroy the engine now. Engine is RAII managed, but you can manually release it with this command
	AnnQuit();

	return EXIT_SUCCESS;
}
