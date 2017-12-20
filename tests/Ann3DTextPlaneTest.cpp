#include "stdafx.h"
#include "engineBootstrap.hpp"

namespace Annwvyn
{
	TEST_CASE("Create 3D text plane")
	{
		auto GameEngine = bootstrapTestEngine("Test3DTextPlane");

		auto textPlane = std::make_shared<Ann3DTextPlane>(2.0f, 1.5f, "This is a text plane", 200, 50.f, "SomeFont");
		textPlane->setBackgroundColor(AnnColor{1,1,1,1});
		textPlane->setPosition(AnnVect3{0,1.5,8});
		textPlane->update();

		AnnGetVRRenderer()->_resetOgreTimer();
		while(GameEngine->getTimeFromStartupSeconds() < 2)
		{
			GameEngine->refresh();
		}

	}
}
