#include "stdafx.h"
#include "engineBootstrap.hpp"

#include <Annwvyn.h>
#include <catch/catch.hpp>

namespace Annwvyn
{
	TEST_CASE("Test timer event")
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
			//outside owned boolean
			bool& state;
		};

		auto GameEngine = bootstrapTestEngine("TestTimeEvent");

		//Construct and register the listener
		auto state{ false };
		auto timerListener = std::make_shared<TimerTest>(state);
		AnnGetEventManager()->addListener(timerListener);
		AnnGetOnScreenConsole()->setVisible(true);
		GameEngine->refresh();
		AnnGetVRRenderer()->_resetOgreTimer();
		double sec;

		//Start a timer, and give it's ID to the listener
		auto timer = AnnGetEventManager()->fireTimer(5);
		timerListener->setID(timer);

		//Run 10 seconds of simulation with debug console visible
		while ((sec = GameEngine->getTimeFromStartupSeconds()) < 10)
		{
			GameEngine->refresh();
			AnnDebug() << "Current time : " << sec;
			if (state) AnnDebug() << "Timer event caught!!!";
		}

		//Assert that result is right
		REQUIRE(state);
	}

	TEST_CASE("Test event collision")
	{
		class CollisionTest : LISTENER
		{
		public:
			CollisionTest(bool& res) : constructListener(),
				results(res),
				position{ 0, 0, 0 },
				normal{ 0, 0 ,0 }
			{}

			void CollisionEvent(AnnCollisionEvent e) override
			{
				auto objectManager = AnnGetGameObjectManager();
				if (e.hasObject(objectManager->getGameObject("_internal_test_floor").get()) && //you should store a pointer to the object for performance, not search it each time
					e.hasObject(objectManager->getGameObject("Sinbad").get()))
				{
					results = true;
					position = e.getPosition();
					normal = e.getNormal();

					if (e.isGroundCollision()) AnnDebug() << "ground collision";
					if (e.isWallCollision()) AnnDebug() << "wall collision";
				}
			}

			AnnVect3 getPosition() const { return position; }
			AnnVect3 getNormal() const { return normal; }
		private:
			bool& results;
			AnnVect3 position, normal;
		};

		auto GameEngine = bootstrapTestEngine("TestCollision");
		AnnGetOnScreenConsole()->setVisible(true);

		//Falling object
		auto sinbad = AnnGetGameObjectManager()->createGameObject("Sinbad.mesh", "Sinbad");
		sinbad->setScale(AnnVect3::UNIT_SCALE / 2.0f);
		sinbad->setPosition(-8, 5, 1);
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
		AnnGetVRRenderer()->_resetOgreTimer();
		while (GameEngine->getTimeFromStartupSeconds() < 5.0f)
		{
			GameEngine->refresh();
			if (state)
			{
				AnnDebug() << "Detected collision";
				AnnDebug() << "Position " << eventListener->getPosition();
				AnnDebug() << "Normal " << eventListener->getNormal();
			}
		}

		REQUIRE(state);
	}

	TEST_CASE("Event Listener Tick sanity test")
	{
		class TickTest : LISTENER
		{
		public:
			TickTest(int& counter) : constructListener(),
				counter(counter) {}

			void tick() override { ++counter; }

		private:
			int& counter;
		};

		auto GameEngine = bootstrapTestEngine("TestTick");
		auto counter = 0;

		auto tickListener = std::make_shared<TickTest>(counter);
		AnnGetEventManager()->addListener(tickListener);

		AnnGetOnScreenConsole()->setVisible(true);

		auto refCounter = 0;
		const auto nbFrames = 200;
		while (refCounter < nbFrames)
		{
			AnnDebug() << ++refCounter;
			GameEngine->refresh();
		}

		REQUIRE(counter == refCounter);
		REQUIRE(counter == nbFrames);
	}
}