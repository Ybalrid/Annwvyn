#include "stdafx.h"
#include "engineBootstrap.hpp"
#include <catch/catch.hpp>

namespace Annwvyn
{
	class TestEvent : public AnnUserSpaceEvent
	{
	public:
		TestEvent(const std::string& message) : AnnUserSpaceEvent("TestEvent"),
			storedMessage(message)
		{
		}

		std::string getMessage() const { return storedMessage; }
	private:
		std::string storedMessage;
	};

	class TestSubSystem : public AnnUserSubSystem
	{
	public:
		TestSubSystem() : AnnUserSubSystem("Test Subsystem"),
			dispatchedYet(false)
		{
		}

		virtual ~TestSubSystem()
		{
		}

		bool needUpdate() override
		{
			return true;
		}

		void update() override
		{
			if (!dispatchedYet)
			{
				if (AnnGetEngine()->getTimeFromStartupSeconds() > 3)
				{
					dispatchEvent(std::make_shared<TestEvent>("Message!"));
					dispatchedYet = true;
				}
			}
		}

	private:
		bool dispatchedYet;
	};

	class TestListener : LISTENER
	{
	public:
		TestListener(bool& state) : constructListener(),
			hash(AnnGetStringUtility()->hash("TestEvent")),
			state(state)
		{}

		void EventFromUserSubsystem(AnnUserSpaceEvent& e, AnnUserSpaceEventLauncher* /*origin*/) override
		{
			if (e.getType() == hash)
			{
				AnnDebug() << "got message : " << static_cast<TestEvent&>(e).getMessage();
				state = true;
			}
		}
	private:
		AnnUserSpaceEvent::AnnUserSpaceEventTypeHash hash;
		bool& state;
	};

	TEST_CASE("Test register subsystem")
	{
		auto GameEngine = bootstrapTestEngine("UserSubSystemTest");
		auto testSystem = std::make_shared<TestSubSystem>();
		REQUIRE(testSystem);
		GameEngine->registerUserSubSystem(testSystem);
		REQUIRE(GameEngine->isUserSubSystem(testSystem));
	}

	TEST_CASE("Test user events")
	{
		auto GameEngine = bootstrapTestEngine("UserSubSystemTest");
		AnnGetOnScreenConsole()->setVisible(true);
		GameEngine->refresh();

		auto testSystem = std::make_shared<TestSubSystem>();
		REQUIRE(testSystem);
		GameEngine->registerUserSubSystem(testSystem);
		REQUIRE(GameEngine->isUserSubSystem(testSystem));

		auto state = false;
		auto testListener = std::make_shared<TestListener>(state);
		REQUIRE(testListener);
		AnnGetEventManager()->addListener(testListener);

		//TODO ISSUE make reseting this timer easier
		AnnGetVRRenderer()->getTimer()->reset();

		while (GameEngine->getTimeFromStartupSeconds() < 6)
			GameEngine->refresh();

		REQUIRE(state);
	}
}