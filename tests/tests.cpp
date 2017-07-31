//To speed up things a bit, we precompile headers
#include "stdafx.h"

//Include the engine itself
#include <Annwvyn.h>
#define RENDERER "OgreNoVRRender"

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

void consolePause();

int main(int argc, char* argv[])
{
	Annwvyn::AnnEngine::setNoConsoleColor();

	auto result = Catch::Session().run(argc, argv);

	//consolePause();

	return (result < 0xff ? result : 0xff);
}

void consolePause()
{
	std::cout << "Press RETURN to continue... ";
	std::cin.get();
}

TEST_CASE("Test 1")
{
	REQUIRE(1 == 1);
}

TEST_CASE("BASIC ENGINE START")
{
	auto GameEngine = std::make_unique<Annwvyn::AnnEngine>("UnitTest0", RENDERER);
	REQUIRE(GameEngine != nullptr);
}

TEST_CASE("BASIC ENGINE START WITH RENDER")
{
	auto GameEngine = std::make_unique<Annwvyn::AnnEngine>("UnitTest1", RENDERER);
	REQUIRE(GameEngine != nullptr);

	for (auto i = 0; i < 60 * 5; i++)
		GameEngine->refresh();
}