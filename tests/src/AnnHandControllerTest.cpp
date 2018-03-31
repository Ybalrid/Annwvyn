#include "stdafx.h"
#include "engineBootstrap.hpp"

namespace Annwvyn
{
	TEST_CASE("Set HandModel")
	{
		auto GameEngine = bootstrapTestEngine("HandModelTest");
		auto smgr		= AnnGetEngine()->getSceneManager();
	}
}
