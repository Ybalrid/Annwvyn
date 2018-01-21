/**
* \file main.cpp
* \brief test/demo program
* \author Arthur Brainville
*
* Annwvyn sample program http://annwvyn.org/
*
*/

#include "stdafx.h"

//Annwvyn
#include <Annwvyn.h>
#include <Ann3DTextPlane.hpp>
#include "TestLevel.hpp"
#include "DemoLevel.hpp"
#include "DemoEvent.hpp"
#include "TutorialTimer.hpp"

using namespace std;
using namespace Annwvyn;

constexpr bool isRoomscale{ true };

void loadHands()
{
	if(auto controller = AnnGetVRRenderer()->getHandControllerArray()[0])
	{
		if(controller->getHandModel() == nullptr)
			controller->setHandModel("Hand_left.mesh");
	}

	if(auto controller = AnnGetVRRenderer()->getHandControllerArray()[1])
	{
		if(controller->getHandModel() == nullptr)
			controller->setHandModel("Hand_right.mesh");
	}
}

class QuitOnButtonListener : LISTENER
{
public:
	QuitOnButtonListener() :
	 constructListener()
	{}

	///Quit app when button zero of left controller is pressed
	void HandControllerEvent(AnnHandControllerEvent e) override
	{
		if(e.getSide() == AnnHandController::leftHandController)
			if(e.buttonPressed(0))
				AnnGetEngine()->requestQuit();
	}
};

AnnMain()
{
	AnnEngine::openConsole();

	AnnOgreVRRenderer::setAntiAliasingLevel(8);
	AnnEngine::logFileName	 = "Samples.log";
	AnnEngine::defaultRenderer = "NoVRRender";

	AnnInit("AnnTest");

	//Init some player body parameters
	if(isRoomscale)
		AnnGetEngine()->initPlayerRoomscalePhysics();
	else
		AnnGetEngine()->initPlayerStandingPhysics();

	AnnGetEventManager()->useDefaultEventListener();
	AnnGetResourceManager()->addFileLocation("media/environment");
	AnnGetResourceManager()->addFileLocation("script");
	AnnGetResourceManager()->initResources();

	//create and load level objects
	AnnGetLevelManager()->addLevel<DemoHub>();
	AnnGetLevelManager()->addLevel<Demo0>();
	AnnGetLevelManager()->addLevel<TestLevel>();
	AnnGetLevelManager()->addLevel<DemoEvent>();
	AnnGetLevelManager()->addLevel<TutorialTimer>();

	AnnGetLevelManager()->addLevel<AnnSplashLevel>("splash.png", AnnGetLevelManager()->getFirstLevelLoaded(), 1.f);

	//ask the level manager to perform a jump to the last level
	AnnGetLevelManager()->jump(AnnGetLevelManager()->getLastLevelLoaded());

	AnnDebug() << "Starting the render loop";

	//AnnGetEngine()->loadUserSubSystemFromPlugin("PluginTemplate");

	auto debugHook = []() {
		loadHands();
	};

	do
	{
		debugHook();
	} while(AnnGetEngine()->refresh());

	AnnQuit();

	return EXIT_SUCCESS;
}
