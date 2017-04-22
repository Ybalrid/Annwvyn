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

constexpr bool isRoomscale{ true };

///Event
class TextMessageEvent : public AnnUserSpaceEvent
{
public:
	TextMessageEvent(string message) : AnnUserSpaceEvent("TextMessage"),
		enclosedMessage(message)
	{
	}

	~TextMessageEvent()
	{
		//This is just to check that everything is in order
		//AnnDebug() << "event destructed!";
	}

	string getMessage() const { return enclosedMessage; };

private:
	string enclosedMessage;
};

///Event Listener
class SomeEventListener : LISTENER
{
public:
	SomeEventListener() : constructListener(),
		hashTypeCheckTextEvent(AnnGetStringUtility()->hash("TextMessage"))
	{
	}

	void EventFromUserSubsystem(AnnUserSpaceEvent& e, AnnUserSpaceEventLauncher* origin) override
	{
		if (e.getType() == hashTypeCheckTextEvent)
			AnnDebug() << "SomeEventListener got the TextMessageEvent \""
			<< dynamic_cast<TextMessageEvent&>(e).getMessage()
			<< "\" from " << origin;
	}

private:
	const AnnUserSpaceEvent::AnnUserSpaceEventTypeHash hashTypeCheckTextEvent;
};

///A user defined subsystem sending the event declared above regularly
class SomeSubSystem : public AnnUserSubSystem
{
public:
	SomeSubSystem(double updateRate = 3) : AnnUserSubSystem("Useless Subsystem"),
		now(0),
		last(0),
		delay(updateRate)
	{
	}

	void update() override
	{
		now = AnnGetEngine()->getTimeFromStartupSeconds();

		if (now - last > delay)
		{
			static unsigned int count = 0;
			last = now;
			dispatchEvent(make_shared<TextMessageEvent>("Useless message! " + to_string(++count)));
		}
	}

	bool needUpdate() override
	{
		return true;
	}

private:
	double now, last, delay;
};

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
					v2 = AnnGetGameObjectManager()->getMesh("Hand_left.mesh", v1, v2);
				else
					v2 = AnnGetGameObjectManager()->getMesh("Hand_right.mesh", v1, v2);
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
		auto controller = e.getController();
		if (controller->getSide() == AnnHandController::leftHandController)
			if (controller->hasBeenPressed(0))
				AnnGetEngine()->requestQuit();
	}
};

std::function<void()> debugHook;

AnnMain()
{
	OgreVRRender::setAntiAliasingLevel(8);
	AnnEngine::openConsole();

	AnnInit("AnnTest");

	//Init some player body parameters
	if (isRoomscale)
		AnnGetEngine()->initPlayerRoomscalePhysics();
	else
		AnnGetEngine()->initPlayerStandingPhysics();

	AnnGetEventManager()->useDefaultEventListener();
	AnnGetResourceManager()->addFileLocation("media/environment");
	AnnGetResourceManager()->initResources();

	//AnnGetLevelManager()->addLevel(make_shared<Demo0>());

	//////create and load level objects
	AnnGetLevelManager()->addLevel(make_shared<DemoHub>());
	//AnnGetLevelManager()->addLevel(make_shared<AnnSplashLevel>("splash.png", AnnGetLevelManager()->getFirstLevelLoaded(), 3));

	////ask the level manager to perform a jump to the last level
	AnnGetLevelManager()->jump(AnnGetLevelManager()->getLastLevelLoaded());
	AnnDebug() << "Starting the render loop";

	debugHook = []()
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