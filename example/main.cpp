/**
* \file main.cpp
* \brief test/demo program
* \author Arthur Brainville
*
* Annwvyn test program http://annwvyn.org/
*
*/
#include "stdafx.h"

//Annwvyn
#include <Annwvyn.h>
#include <Ann3DTextPlane.hpp>
#include "TestLevel.hpp"
#include "DemoLevel.hpp"

using namespace std;
using namespace Annwvyn;

constexpr bool isRoomscale{ false };

AnnMain()
{
	OgreVRRender::setAntiAliasingLevel(8);
	AnnInit("AnnTest");

	//Init some player body parameters
	if (isRoomscale)
		AnnGetEngine()->initPlayerRoomscalePhysics();
	else
		AnnGetEngine()->initPlayerStandingPhysics();

	AnnGetEventManager()->useDefaultEventListener();

	//load resources
	AnnGetResourceManager()->addFileLocation("media/environment");
	AnnGetResourceManager()->addFileLocation("media/debug");

	AnnGetResourceManager()->initResources();
	//AnnGetResourceManager()->loadGroup(AnnResourceManager::reservedResourceGroupName);
	//AnnGetResourceManager()->loadGroup(AnnResourceManager::defaultResourceGroupName);
	//AnnGetAudioEngine()->preLoadBuffer("media/bgm/bensound-happyrock.ogg");

	//create and load level objects
	AnnGetLevelManager()->addLevel(make_shared<DemoHub>());
	AnnGetLevelManager()->addLevel(make_shared<Demo0>());
	AnnGetLevelManager()->addLevel(make_shared<TestLevel>());

	//ask the level manager to perform a jump to the first level
	AnnGetLevelManager()->jumpToFirstLevel();

	AnnGetScriptManager()->evalFile("script/test.chai");

	//stringstream controllerOut;
	AnnDebug() << "Starting the render loop";

	AnnGetEngine()->startGameplayLoop();

	AnnQuit();

	//Try to start the engine again :

	MessageBox(nullptr, L"Starting Annwvyn again", L"Engine restart", MB_ICONINFORMATION);

	/*auto */GameEngine = std::make_unique<AnnEngine>("other app", detectedHMD);
	AnnGetResourceManager()->initResources();
	AnnGetEngine()->startGameplayLoop();

	AnnQuit();

	return EXIT_SUCCESS;
}