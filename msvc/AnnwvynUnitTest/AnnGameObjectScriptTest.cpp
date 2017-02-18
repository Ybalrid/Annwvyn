#include "stdafx.h"
#include "CppUnitTest.h"
#include "UtilityClasses.hpp"
#include "UtilityFunctions.hpp"
#include <Annwvyn.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace TestUtilityFunctions;
using namespace Annwvyn;

namespace AnnwvynUnitTest
{
	TEST_CLASS(AnnGameObjectScriptUnitTest)
	{
	public:
		TEST_METHOD(SimpleScriptTest)
		{
			openConsole();
			auto engine = startTestEngine("SimpleScriptTest");
			AnnGetOnScreenConsole()->toggle();

			AnnGetLevelManager()->addLevel(std::make_shared<SimpleLevel>());
			AnnGetLevelManager()->jumpToFirstLevel();

			do
			{
				AnnGetScriptManager()->evalString(
					R"(AnnDebugLog("Hello from Script World");)"
				);
			} while (engine->refresh());
		}

		TEST_METHOD(SimpleBehaviorSciptTest)
		{
			openConsole();
			auto engine = startTestEngine("SimpleBehaviorSciptTest");
			AnnGetResourceManager()->initResources();

			auto gizmo = AnnGetGameObjectManager()->createGameObject("gizmo.mesh", "Gizmo");
			gizmo->attachScript("TestScript");
			constexpr auto xOffset = 0.f;
			constexpr auto zOffset = 8.0f;
			gizmo->setPosition({ xOffset, 1.0f, zOffset });

			AnnGetOnScreenConsole()->toggle();

			constexpr auto framerate = 60;
			constexpr auto stageLen = 2 * framerate;
			constexpr auto duration = 3 * stageLen;
			int frame = 0;

			do
			{
				frame++;

				auto time = AnnGetEngine()->getTimeFromStartupSeconds();
				if (frame < 2 * stageLen)
				{
					gizmo->setOrientation(AnnQuaternion{ AnnRadian(Ogre::Math::TWO_PI * sin(time)), AnnVect3::UNIT_Y });
					continue;
				}

				if (frame < 4 * stageLen)
				{
					gizmo->setPosition({ xOffset, 1.0f + float(sin(time)), zOffset });
					continue;
				}

				if (frame > duration)
				{
					AnnGetEngine()->requestQuit();
				}
			} while (engine->refresh());
		}
	};
}