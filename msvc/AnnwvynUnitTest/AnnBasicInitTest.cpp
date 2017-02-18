#include "stdafx.h"
#include "CppUnitTest.h"
#include <Annwvyn.h>
#include "UtilityClasses.hpp"
#include "UtilityFunctions.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Annwvyn;
using namespace TestUtilityFunctions;

namespace AnnwvynUnitTest
{
	TEST_CLASS(AnnBasicInit)
	{
		TEST_METHOD(AnnInitOnceTest)
		{
			//Assert that the environment is clean
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr");

			//Init the engine
			auto engine = std::make_unique<AnnEngine>("AnnInitOnceTest", "OgreNoVRRender");

			//Assert that engine is initialized
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is initialized");

			//Release pointer -> this destruct the engine -> this set back the instance to nullptr
			engine.reset(nullptr);
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr again");
		}

		TEST_METHOD(AnnInitMultipleTest)
		{
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr");
			auto engine0 = std::make_unique<AnnEngine>("AnnInitTest0", "OgreNoVRRender");
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is initialized");
			engine0.reset(nullptr);
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr again");

			//repeat:
			auto engine1 = std::make_unique<AnnEngine>("AnnInitTest1", "OgreNoVRRender");
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is initialized");
			engine1.reset(nullptr);
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr again");
		}

		//Test will work if engine doesn't crash
		TEST_METHOD(AnnInitSimpleLevel)
		{
			auto GameEngine = std::make_unique<AnnEngine>("AnnInitLevel", "OgreNoVRRender");

			AnnGetLevelManager()->addLevel(std::make_unique<SimpleLevel>());
			AnnGetLevelManager()->jumpToFirstLevel();
			AnnGetOnScreenConsole()->toggle();

			AnnGetEngine()->startGameplayLoop();
		}

		//Test will work if engine doesn't crash
		TEST_METHOD(AnnInitLessSimpleLevel)
		{
			auto GameEngine = std::make_unique<AnnEngine>("AnnInitLevel", "OgreNoVRRender");

			//Will need to load resources now :
			AnnGetResourceManager()->initResources();
			AnnGetLevelManager()->addLevel(std::make_unique<LessSimpleLevel>());
			AnnGetLevelManager()->jumpToFirstLevel();
			AnnGetOnScreenConsole()->toggle();

			AnnGetEngine()->startGameplayLoop();
		}

		TEST_METHOD(AnnInitCoreSubSystems)
		{
			auto engine = startTestEngine("AnnInitCoreSubSystems");

			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetAudioEngine().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetPhysicsEngine().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetFileSystemManager().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetLevelManager().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetEventManager().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetPlayer().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetResourceManager().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetSceneryManager().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetVRRenderer().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetScriptManager().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetGameObjectManager().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetOnScreenConsole().get()));
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetStringUtility().get()));
		}
	};
}