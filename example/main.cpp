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

class TextMessageEvent : public AnnUserSpaceEvent
{
public:
	TextMessageEvent(string message) : AnnUserSpaceEvent("TextMessage"),
		enclosedMessage(message)
	{
	}

	string getMessage() const { return enclosedMessage; };
private:
	string enclosedMessage;
};

class SomeEventListener : LISTENER
{
public:
	SomeEventListener() : constructListener(),
		//TODO make the hashing less painful to write and without constructing an std::hash at each call
		hashTypeCheckTextEvent(hash<string>()("TextMessage"))
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

class SomeSubSystem : public AnnUserSubSystem
{
public:
	SomeSubSystem() : AnnUserSubSystem("Useless Subsystem")
	{
	}

	void update() override
	{
		auto e = TextMessageEvent("Useless message!");
		dispatchEvent(e);
	}

	bool needUpdate() override
	{
		return true;
	}
};

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

	AnnDebug() << "Starting the render loop";

	AnnGetEngine()->registerUserSubSystem(make_shared<SomeSubSystem>());

	//Not how you're supposed to do it:
	auto someListener = make_shared<SomeEventListener>();
	AnnGetEventManager()->addListener(someListener);

	AnnGetEngine()->startGameplayLoop();

	//Cleanup of the "don't do it" listener
	AnnGetEventManager()->removeListener(someListener);
	someListener.reset();

	AnnQuit();

	return EXIT_SUCCESS;
}