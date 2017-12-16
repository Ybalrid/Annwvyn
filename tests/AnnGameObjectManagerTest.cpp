#include "stdafx.h"
#include "engineBootstrap.hpp"
#include <catch/catch.hpp>

namespace Annwvyn
{
	TEST_CASE("Game Object name storage")
	{
		//Init
		auto GameEngine = bootstrapTestEngine("GameObjectManagerTest");

		//Construct an object with a manually choosen id
		const std::string name{ "sinbad" };
		auto object = AnnGetGameObjectManager()->createGameObject("Sinbad.mesh", name);

		//Check everything
		REQUIRE(object->getName() == name);
		REQUIRE(AnnGetGameObjectManager()->getGameObject(name) == object);
		REQUIRE(AnnGetGameObjectManager()->getGameObject(name)->getName() == name);
	}

	TEST_CASE("Game object add remove")
	{
		auto GameEngine = bootstrapTestEngine("GameObjectManagerTest");

		auto manager = AnnGetGameObjectManager();
		{
			auto object = manager->createGameObject("Sinbad.mesh", "Sinbad");

			for (auto i = 0; i < 60; ++i) GameEngine->refresh();

			manager->removeGameObject(object);
		}

		for (auto i = 0; i < 60; ++i) GameEngine->refresh();
	}

	TEST_CASE("Light Object name storage")
	{
		//Init
		auto GameEngine = bootstrapTestEngine("GameObjectManagerTest");

		const std::string name{ "sun" };

		auto sun = AnnGetGameObjectManager()->createLightObject(name);

		REQUIRE(sun->getName() == name);
		REQUIRE(AnnGetGameObjectManager()->getLightObject(name) == sun);
		REQUIRE(AnnGetGameObjectManager()->getLightObject(name)->getName() == name);
	}

	TEST_CASE("Trigger Object name storage")
	{
		//Init
		auto GameEngine = bootstrapTestEngine("GameObjectManagerTest");

		const std::string name{ "collisionBox" };

		auto trigger = AnnGetGameObjectManager()->createTriggerObject(name);
		trigger->setShape(AnnTriggerObjectShapeGenerator::box(1, 1, 1));

		REQUIRE(trigger->getName() == name);
		REQUIRE(AnnGetGameObjectManager()->getTriggerObject(name) == trigger);
		REQUIRE(AnnGetGameObjectManager()->getTriggerObject(name)->getName() == name);
	}
}