#include "engineBootstrap.hpp"
#include <catch/catch.hpp>

namespace Annwvyn
{
TEST_CASE("Test glTF Loading")
{
	auto GameEngine = Annwvyn::bootstrapTestEngine("GlTF Test");

	auto object = Annwvyn::AnnGetGameObjectManager()->createGameObject("Avocado.glb");
	object->setPosition(0, 1.6, 9);
	object->setScale(3, 3, 3);

	REQUIRE(object != nullptr);
	GameEngine->initPlayerRoomscalePhysics();
	Annwvyn::AnnGetEventManager()->useDefaultEventListener();

	const auto counter = 60 * 5;
	auto frame		   = 0;
	while(frame < counter && !GameEngine->checkNeedToQuit())
	{
		object->setOrientation(Annwvyn::AnnQuaternion(Annwvyn::AnnDegree(frame * 5), Annwvyn::AnnVect3::UNIT_Y));
		frame++;
		GameEngine->refresh();
	}
}
}