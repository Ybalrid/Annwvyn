#include "stdafx.h"
#include "configs.hpp"

#include <Annwvyn.h>
#include <catch/catch.hpp>

namespace Annwvyn
{
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