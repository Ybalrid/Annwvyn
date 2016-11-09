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

class VerboseCollision : LISTENER
{
public:
	void CollisionEvent(AnnCollisionEvent e) override
	{
		AnnDebug() << "Collision between " << e.getA() << " and " << e.getB();
		AnnDebug() << "with names " << e.getA()->getName() << " and " << e.getB()->getName();
	}

	void PlayerCollisionEvent(AnnPlayerCollisionEvent e) override
	{
		AnnDebug() << "Player Collision with " << e.getObject();
		AnnDebug() << "That bear the name : " << e.getObject()->getName();
	}
};

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

	auto verboseCollide = make_shared<VerboseCollision>();

	AnnGetEventManager()->addListener(verboseCollide);

	AnnGetEngine()->startGameplayLoop();

	AnnGetEventManager()->removeListener(verboseCollide);
	verboseCollide.reset();

	AnnQuit();

	return EXIT_SUCCESS;
}