#include <stdafx.h>

#include <engineBootstrap.hpp>

namespace Annwvyn
{
	class TestLevel : LEVEL
	{
	public:
		TestLevel() : constructLevel()
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
		TestLevelLoad(bool& loadState, bool& unloadState) : TestLevel(),
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
		auto GameEngine = bootstrapEmptyEngine("TestLevel");
		auto LevelManager = AnnGetLevelManager();

		auto first = std::make_shared<TestLevel>();
		auto second = std::make_shared<TestLevel>();
		auto third = std::make_shared<TestLevel>();

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
			auto GameEngine = bootstrapEmptyEngine("TestLevel");
			auto LevelManager = AnnGetLevelManager();

			LevelManager->addLevel(std::make_shared<TestLevelLoad>(loadStatus, unloadStatus));
			LevelManager->jumpToFirstLevel();

			AnnGetOnScreenConsole()->setVisible(true);

			for (auto i{ 0 }; i < 3; ++i)
				GameEngine->refresh();
		}//Scope "GameEngine"'s lifetime

		REQUIRE(loadStatus);
		REQUIRE(unloadStatus);
	}
}