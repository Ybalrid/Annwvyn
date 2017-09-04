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
}