#include "engineBootstrap.hpp"
#include <catch/catch.hpp>

namespace Annwvyn
{
	TEST_CASE("Test Procedural Genrator")
	{
		auto GameEngine = Annwvyn::bootstrapTestEngine("Proc Test");
		GameEngine->initPlayerRoomscalePhysics();
		AnnGetEventManager()->useDefaultEventListener();
		const auto maxFrames = 200;

		auto object = AnnGetGameObjectManager()->getProceduralGenerator()->getBox("myBox.mesh");
		REQUIRE(object);

		object->setPosition(0, 1, 0);

		for(auto i{ 0 }; i < maxFrames && !GameEngine->checkNeedToQuit(); ++i)
		{
			AnnDebug() << "frame number = " << i;
			GameEngine->refresh();
		}

		AnnDebug() << "Render loop done!";
	}
}