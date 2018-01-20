#include <stdafx.h>
#include <engineBootstrap.hpp>

namespace Annwvyn
{
	class TestLevel : LEVEL
	{
	public:
		TestLevel() :
		 constructLevel()
		{}

		void load() override
		{}

		void unload() override
		{
			AnnLevel::unload();
		}

		void runLogic() override
		{}
	};

	class TestLevelLoad : public TestLevel
	{
	public:
		TestLevelLoad(bool& loadState, bool& unloadState) :
		 TestLevel(),
		 loadState(loadState),
		 unloadState(unloadState)
		{}

		void load() override
		{
			TestLevel::load();
			loadState = true;
			AnnDebug() << "I've been loaded!";
		}

		void unload() override
		{
			TestLevel::unload();
			unloadState = true;
			AnnDebug() << "I've been unloaded!";
		}

	private:
		bool& loadState;
		bool& unloadState;
	};

	TEST_CASE("Level manager sanity test")
	{
		auto GameEngine = bootstrapEmptyEngine("TestLevel");
		REQUIRE(AnnGetLevelManager());
	}

	TEST_CASE("Level manager insertion test")
	{
		auto GameEngine   = bootstrapEmptyEngine("TestLevel");
		auto LevelManager = AnnGetLevelManager();

		auto first  = std::make_shared<TestLevel>();
		auto second = std::make_shared<TestLevel>();
		auto third  = std::make_shared<TestLevel>();

		LevelManager->addLevel(first);
		LevelManager->addLevel(second);
		LevelManager->addLevel(third);

		REQUIRE(first == LevelManager->getFirstLevelLoaded());
		REQUIRE(third == LevelManager->getLastLevelLoaded());
		REQUIRE(first == LevelManager->getLevelByIndex(0));
		REQUIRE(second == LevelManager->getLevelByIndex(1));
		REQUIRE(third == LevelManager->getLevelByIndex(2));

		REQUIRE_FALSE(LevelManager->getCurrentLevel());

		first.reset();
		second.reset();
		third.reset();
	}

	TEST_CASE("Level manager load")
	{
		auto loadStatus{ false }, unloadStatus{ false };

		{
			auto GameEngine   = bootstrapEmptyEngine("TestLevel");
			auto LevelManager = AnnGetLevelManager();

			LevelManager->addLevel(std::make_shared<TestLevelLoad>(loadStatus, unloadStatus));
			LevelManager->jumpToFirstLevel();

			AnnGetOnScreenConsole()->setVisible(true);

			for(auto i{ 0 }; i < 3; ++i)
				GameEngine->refresh();
		} //Scope "GameEngine"'s lifetime

		REQUIRE(loadStatus);
		REQUIRE(unloadStatus);
	}

	TEST_CASE("Level load geometry")
	{
		class TestLevelLoadObjects : LEVEL
		{
			bool& ogreOk;
			bool& floorOk;

		public:
			TestLevelLoadObjects(bool& forOgre, bool& forFloor) :
			 constructLevel(),
			 ogreOk{ forOgre }, floorOk{ forFloor } {}

			void load() override
			{
				auto floor = addGameObject("floorplane.mesh", "Floor");
				auto ogre  = addGameObject("Sinbad.mesh", "Ogre");

				auto sun = addLightObject();
				sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
				sun->setDirection({ -0.0625, -1, 1 });

				auto gameObjectManager = AnnGetGameObjectManager();
				floorOk				   = gameObjectManager->getGameObject("Floor") != nullptr;
				ogreOk				   = gameObjectManager->getGameObject("Ogre") != nullptr;
			}

			void runLogic() override {}
		};

		auto ogreOk{ false }, floorOk{ false };

		//Scope the lifetime of GameEngine this way
		{
			auto GameEngine   = bootstrapEmptyEngine("TestLevel");
			auto levelManager = AnnGetLevelManager();

			levelManager->addLevel(std::make_shared<TestLevelLoadObjects>(ogreOk, floorOk));
			levelManager->jumpToFirstLevel();

			for(auto i = 0; i < 3; ++i)
			{
				GameEngine->refresh();
			}
		}

		REQUIRE(ogreOk);
		REQUIRE(floorOk);
	}

	TEST_CASE("Level Manager add geometry via manager")
	{
		auto GameEngine		   = bootstrapTestEngine("TestLevel");
		auto levelManager	  = AnnGetLevelManager();
		auto gameObjectManager = AnnGetGameObjectManager();
		auto ogre{ false }, floor{ false };
		AnnLevelPtr level;
		levelManager->addLevel(level = std::make_shared<TestLevel>());
		levelManager->jumpToFirstLevel();

		for(auto i{ 0 }; i < 3; i++)
			GameEngine->refresh();

		AnnGameObjectPtr sinbad;
		levelManager->addToCurrentLevel(sinbad = gameObjectManager->createGameObject("sinbad.mesh", "MySinbad"));

		REQUIRE(gameObjectManager->getGameObject("MySinbad") == sinbad);
		auto levelContent = level->getContent();
		auto result		  = std::find(std::begin(levelContent), std::end(levelContent), sinbad);
		REQUIRE(result != std::end(levelContent));
		REQUIRE(*result == sinbad);
	}
}
