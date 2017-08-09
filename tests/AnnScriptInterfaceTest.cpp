#include <stdafx.h>
#include <engineBootstrap.hpp>

namespace Annwvyn
{
	TEST_CASE("Attach script to object")
	{
		{
			auto GameEngine = bootstrapEmptyEngine("TestScript");

			auto ResourceManager = AnnGetResourceManager();
			ResourceManager->addFileLocation("./unitTestScripts");
			ResourceManager->initResources();

			auto GameObjectManager = AnnGetGameObjectManager();
			auto sun = GameObjectManager->createLightObject();
			sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
			sun->setDirection({ 0, -5, -1 });
			sun->setPower(97);
			auto floor = GameObjectManager->createGameObject("floorplane.mesh", "Floor");
			auto ogre = GameObjectManager->createGameObject("Sinbad.mesh", "Ogre");

			//Attach the behavior that cause to move the object by +0.02f on +y every frame
			ogre->attachScript("GoUpBehavior");

			//run 250 frames of the game
			auto counter{ 0 };
			while (GameEngine->refresh())
			{
				if (counter++ > 250) break;
			}

			//Check if script ran correctly
			REQUIRE(ogre->getPosition().y >= 5);
		}
	}
}