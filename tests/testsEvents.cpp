#include "stdafx.h"
#include "engineBootstrap.hpp"

#include <Annwvyn.h>
#include <catch/catch.hpp>

namespace Annwvyn
{
	TEST_CASE("TEST TIMER EVENT")
	{
		//Nested listener class
		class TimerTest : LISTENER
		{
		public:
			TimerTest(bool& state) : constructListener(),
				id(-1), state(state) {}

			//Save the ID we will await for
			void setID(timerID newID) { id = newID; }

			//If we ever get a time event that correspond to the timer we want, set to true
			void TimeEvent(AnnTimeEvent e) override
			{
				if (e.getID() == id) state = true;
			}

		private:
			//timer we check
			timerID id;
			//ouside owned boolean
			bool& state;
		};

		auto GameEngine = bootstrapTestEngine("TestTimeEvent");

		//Construct and register the listener
		auto state{ false };
		auto timerListener = std::make_shared<TimerTest>(state);
		AnnGetEventManager()->addListener(timerListener);
		AnnGetOnScreenConsole()->setVisible(true);
		GameEngine->refresh();
		AnnGetVRRenderer()->getTimer()->reset();
		double sec;

		//Start a timer, and give it's ID to the listener
		auto timer = AnnGetEventManager()->fireTimer(5);
		timerListener->setID(timer);

		//Run 10 seconds of simulation with debug console visible
		while ((sec = GameEngine->getTimeFromStartupSeconds()) < 10)
		{
			GameEngine->refresh();
			AnnDebug() << "Curernt time : " << sec;
			if (state) AnnDebug() << "Timer event caught!!!";
		}

		//Assert that result is right
		REQUIRE(state);
	}

	TEST_CASE("TEST EVENT COLLISION")
	{
		class CollisionTest : LISTENER
		{
		public:
			CollisionTest(bool& res) : constructListener(),
				results(res) {}

			void CollisionEvent(AnnCollisionEvent e) override
			{
				auto objectManager = AnnGetGameObjectManager();
				if (e.hasObject(objectManager->getObjectFromID("floor").get()) &&
					e.hasObject(objectManager->getObjectFromID("Sinbad").get()))
					results = true;
			}
		private:
			bool& results;
		};

		auto GameEngine = bootstrapTestEngine("TestCollision");

		//Falling object
		auto sinbad = AnnGetGameObjectManager()->createGameObject("Sinbad.mesh", "Sinbad");
		sinbad->setScale(AnnVect3::UNIT_SCALE / 2.0f);
		sinbad->setPosition(0, 5, 0);
		sinbad->setUpPhysics(100, boxShape);
		REQUIRE(sinbad);
		REQUIRE(sinbad->getBody());
		REQUIRE(sinbad->getPosition().y > 0);

		//Set debug stuff
		auto state{ false };
		auto eventListener = std::make_shared<CollisionTest>(state);
		AnnGetEventManager()->addListener(eventListener);
		REQUIRE(eventListener);

		AnnGetPhysicsEngine()->setDebugPhysics(true);
		AnnGetVRRenderer()->getTimer()->reset();
		while (GameEngine->getTimeFromStartupSeconds() < 5.0f)
		{
			GameEngine->refresh();
			if (state)
			{
				AnnGetOnScreenConsole()->setVisible(true);
				AnnDebug() << "Detected collision";
			}
		}

		REQUIRE(state);
	}
}