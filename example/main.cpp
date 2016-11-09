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

AnnMain()
{
	//Only useful on windows : Open a debug console to get stdout/stderr
	AnnEngine::openConsole();

	OgreVRRender::setAntiAliasingLevel(8);
	AnnInit("AnnTest");

	//Init some player body parameters
	AnnGetEngine()->initPlayerPhysics();
	AnnGetEventManager()->useDefaultEventListener();

	if (dynamic_cast<OgreOculusRender*>(AnnGetVRRenderer().get()))
	{
		AnnGetVRRenderer()->recenter();
	}

	//load resources
	AnnGetResourceManager()->addFileLocation("media/environment");
	AnnGetResourceManager()->addFileLocation("media/debug");
	AnnGetResourceManager()->initResources();
	AnnGetResourceManager()->loadGroup(AnnResourceManager::reservedResourceGroupName);
	AnnGetResourceManager()->loadGroup(AnnResourceManager::defaultResourceGroupName);
	AnnGetAudioEngine()->preLoadBuffer("media/bgm/bensound-happyrock.ogg");

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

	return EXIT_SUCCESS;
}