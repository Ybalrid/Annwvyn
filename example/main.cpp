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
		//TODO make the hashing less painful to write and without constructing an std::hash at each call
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
	static bool done[2] = { false, false };
	enum sideNames : size_t
	{
		left = 0x0,
		right = 0x1
	};

	for (auto side : { left, right })
		if (!done[side])
			if (auto controller = AnnGetVRRenderer()->getHandControllerArray()[side])
			{
				//TODO mh... Need an item here. 
				//controller->attachModel(AnnGetEngine()->getSceneManager()->createEntity("gizmo.mesh"));
				done[side] = true;
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
/*	auto quitOnButtonListener = make_shared<QuitOnButtonListener>();
	AnnGetEventManager()->addListener(quitOnButtonListener);

	//load resources
	*/
	AnnGetResourceManager()->addFileLocation("media/environment");
	AnnGetResourceManager()->addFileLocation("media/debug");
	//AnnGetResourceManager()->addFileLocation("media/HDR");
	AnnGetResourceManager()->initResources();
	
	
	//create and load level objects
	AnnGetLevelManager()->addLevel(make_shared<DemoHub>());
	AnnGetLevelManager()->addLevel(make_shared<Demo0>());
	AnnGetLevelManager()->addLevel(make_shared<TestLevel>());

	//ask the level manager to perform a jump to the first level
	AnnGetLevelManager()->jumpToFirstLevel();
	
	AnnDebug() << "Starting the render loop";

	//Not how you're supposed to do it:
	AnnGetEngine()->registerUserSubSystem(make_shared<SomeSubSystem>());
	auto someListener = make_shared<SomeEventListener>();
	AnnGetEventManager()->addListener(someListener);

	//AnnGetEngine()->startGameplayLoop();

	debugHook = []()
	{
		putGizmoOnHands();
	};

	do
	{
		//debugHook();
	} while (AnnGetEngine()->refresh());

	//Cleanup of the "don't do it" listener
	AnnGetEventManager()->removeListener(someListener);
	someListener.reset();

	AnnQuit();

	return EXIT_SUCCESS;
}