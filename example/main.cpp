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
#include "DemoEvent.hpp"
#include "TutorialTimer.hpp"

using namespace std;
using namespace Annwvyn;

constexpr bool isRoomscale{ true };

void putGizmoOnHands()
{
	static volatile bool done[2] = { false, false };
	enum sideNames : size_t
	{
		left = 0x0,
		right = 0x1
	};

	for (auto side : { left, right })
	{
		if (!done[side])
		{
			if (auto controller = AnnGetVRRenderer()->getHandControllerArray()[side])
			{
				Ogre::v1::MeshPtr v1; Ogre::MeshPtr v2;
				if (side == left)
					v2 = AnnGetGameObjectManager()->getAndConvertFromV1Mesh("Hand_left.mesh", v1, v2);
				else
					v2 = AnnGetGameObjectManager()->getAndConvertFromV1Mesh("Hand_right.mesh", v1, v2);
				controller->attachModel(AnnGetEngine()->getSceneManager()->createItem(v2));
				done[side] = true;
			}
		}
	}
}

class QuitOnButtonListener : LISTENER
{
public:
	QuitOnButtonListener() : constructListener()
	{
	}

	///Quit app when button zero of left controller is pressed
	void HandControllerEvent(AnnHandControllerEvent e) override
	{
		if (e.getSide() == AnnHandController::leftHandController)
			if (e.buttonPressed(0))
				AnnGetEngine()->requestQuit();
	}
};

AnnMain()
{
	AnnOgreVRRenderer::setAntiAliasingLevel(8);
	AnnEngine::openConsole();
	AnnEngine::logFileName = "Samples.log";
	AnnEngine::defaultRenderer = "OgreNoVRRender";

	AnnInit("AnnTest");

	//Init some player body parameters
	if (isRoomscale)
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

	auto debugHook = []()
	{
		putGizmoOnHands();
	};

	do
	{
		debugHook();
	} while (AnnGetEngine()->refresh());

	AnnQuit();

	return EXIT_SUCCESS;
}