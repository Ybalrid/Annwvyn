#include "stdafx.h"
#include "configs.hpp"

#include <Annwvyn.h>
#include <catch/catch.hpp>

namespace Annwvyn
{
	//All tests will need at least this :
	std::unique_ptr<AnnEngine> setupBase()
	{
		//Start engine
		std::unique_ptr<AnnEngine> GameEngine{ nullptr };
		GameEngine = std::make_unique<AnnEngine>("TestCollision", RENDERER);
		REQUIRE(GameEngine);

		//Construct environement
		auto sun = AnnGetGameObjectManager()->createLightObject(); //physics based shading crash shaders if no light
		sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		sun->setPower(97);
		sun->setDirection(AnnVect3{ 0, -1, -2 }.normalisedCopy());
		REQUIRE(sun);
		//Fixed object in space : the floor
		auto floor = AnnGetGameObjectManager()->createGameObject("floorplane.mesh", "floor");
		floor->setUpPhysics();
		REQUIRE(floor);
		REQUIRE(floor->getBody());
		return move(GameEngine);
	}

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

			//If we ever get a time event that corespond to the timer we want, set to true
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

		auto GameEngine = setupBase();

		//Construct and register the listener
		auto state{ false };
		auto timerListener = std::make_shared<TimerTest>(state);
		AnnGetEventManager()->addListener(timerListener);

		//Start a timer, and give it's ID to the listener
		auto timer = AnnGetEventManager()->fireTimer(5);
		timerListener->setID(timer);

		//Run 10 seconds of simulation with debug console visible
		AnnGetOnScreenConsole()->setVisible(true);
		double sec;
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
				results(res)
			{
			}

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

		auto GameEngine = setupBase();

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