/**
* \file main.cpp
* \brief test/demo program
* \author Arthur Brainville
*
* Annwvyn sample program http://annwvyn.org/
*
*/

//Annwvyn
#include <Annwvyn.h>
#include <Ann3DTextPlane.hpp>
#include "TestLevel.hpp"
#include "DemoLevel.hpp"
#include "DemoEvent.hpp"
#include "TutorialTimer.hpp"

using namespace std;
using namespace Annwvyn;

constexpr bool isRoomscale { true };

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
	{
	}

	///Quit app when button zero of left controller is pressed
	void HandControllerEvent(AnnHandControllerEvent e) override
	{
		if(e.getSide() == AnnHandController::leftHandController)
			if(e.buttonPressed(0))
				AnnGetEngine()->requestQuit();
	}
};

void staticSetup()
{
	AnnEngine::openConsole();
	AnnOgreVRRenderer::setAntiAliasingLevel(8);
	AnnEngine::setLogFileName("Samples.log");
	AnnEngine::setDefaultRenderer("Oculus"); //Change this to Oculus or OpenVR to change the renderer to use
#ifdef _WIN32
	AnnEngine::registerVRRenderer("Oculus");
#endif
	AnnEngine::registerVRRenderer("OpenVR");
}

AnnMain()
{
	staticSetup();
	AnnEngine GameEngine("AnnTest");

	{
		auto eventManager { AnnGetEventManager() };
		auto levelManager { AnnGetLevelManager() };
		auto resourceManager { AnnGetResourceManager() };

		//Init some player body parameters
		if(isRoomscale)
			GameEngine.initPlayerRoomscalePhysics();
		else
			GameEngine.initPlayerStandingPhysics();

		eventManager->useDefaultEventListener();

		resourceManager->addFileLocation("media/environment");
		resourceManager->addFileLocation("script");
		resourceManager->initResources();

		//create and load level objects
		auto hub = levelManager->addLevel<DemoHub>();
		levelManager->addLevel<Demo0>();
		levelManager->addLevel<TestLevel>();
		levelManager->addLevel<DemoEvent>();
		levelManager->addLevel<TutorialTimer>();
		levelManager->addLevel<AnnSplashLevel>("splash.png", hub, 1.f);

		//ask the level manager to perform a jump to the last level
		levelManager->switchToLastLoadedLevel();
	} //this scope exist to release all shared pointer we got when doing auto X = Y

	AnnDebug() << "Starting the render loop";

	//AnnGetEngine()->loadUserSubSystemFromPlugin("PluginTemplate");

	auto debugHook = [] {
		loadHands();
	};

	do
	{
		debugHook();
	} while(AnnGetEngine()->refresh());

	return EXIT_SUCCESS;
}
