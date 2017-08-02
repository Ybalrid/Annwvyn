#pragma once
#include <string>
#include "configs.hpp"
#include <Annwvyn.h>

#include <catch/catch.hpp>

namespace Annwvyn
{
	//All tests will need at least this :
	inline std::unique_ptr<AnnEngine> bootstrapTestEngine(const std::string& name)
	{
		//Start engine
		auto GameEngine = std::make_unique<AnnEngine>(name.c_str(), RENDERER);
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
}