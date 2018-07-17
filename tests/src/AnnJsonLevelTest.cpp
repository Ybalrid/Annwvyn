#include <engineBootstrap.hpp>
#include "AnnJsonLevel.hpp"

namespace Annwvyn
{

	TEST_CASE("Loading of JSON level")
	{
		auto GameEngine = bootstrapEmptyEngine("JSON");

		{
			auto LevelManager = AnnGetLevelManager();
			LevelManager->addLevel<AnnJsonLevel>(false, R"JSON(
{
	"name":"JsonTestLevel",
	"resources": [{
		"group":"TestLevel",
		"path":"./TestLevel.zip",
		"type":"Zip"
	}],

	"player":{
		"startPosition":[0.0, 0.0, 10.0],
		"startOrientation":[0.0, 0.0, 0.0, 1.0]
	},

	"content" : [{
		"name":"Sinbad",
		"mesh":"Sinbad.mesh",
		"position":[0.0, 1.0, 0.0],
		"orientation":[0.0, 0.0, 0.0, 1.0],
		"scale":[0.5, 0.5, 0.5],
		"hasPhysics":true,
		"physics" : {
			"shape":"box",
			"mass":120.0,
			"playerColide":true
		},
		"scripts":null
	},
	{
		"name":"Penguin",
		"mesh":"penguin.mesh",
		"position":[3,1.5,0],
		"orientation":[0,0,0,1],
		"scale":[0.1,0.1,0.1],
		"hasPhysics":false,
		"scripts":["penguinMove"]
	},
	{
		"name":"Floor",
		"mesh":"floorplane.mesh",
		"position":[0.0, 0.0, 0.0],
		"orientation":[0.0, 0.0, 0.0, 1.0],
		"scale":[1.0, 1.0, 1.0],
		"hasPhysics":true,
		"physics" : {
			"shape":"static",
			"mass":0,
			"playerColide":true
		},
		"scripts":null
	}],

	"lighting":[{
		"name":"sun",
		"type":"directional",
		"power":97,
		"direction":[-1.0, -1.5, -1.0]
	},
	{
		"name":"otherLight",
		"type":"point",
		"position":[0,1,1],
		"power":50

	}]

}
)JSON");

			LevelManager->switchToFirstLoadedLevel();
		};

		for(auto i{ 0 }; i < 3 * 60; ++i)
			if(!GameEngine->refresh())
				break;
	}
}
