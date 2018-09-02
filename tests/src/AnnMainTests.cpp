//To speed up things a bit, we precompile headers

#include "configs.hpp"

//Include the engine itself
#include <Annwvyn.h>

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

//Define our own main function
int main(int argc, char* argv[])
{
	//Need to preset some parameters
	Annwvyn::AnnEngine::setNoConsoleColor();
	Annwvyn::AnnDebug::setVerbosity(Annwvyn::AnnDebug::LogLevel::Verbose);

	const auto result = Catch::Session().run(argc, argv);

	return (result < 0xff ? result : 0xff);
}

namespace Annwvyn
{
	TEST_CASE("Basic engine start")
	{
		auto GameEngine = std::make_unique<AnnEngine>("BasicInit", RENDERER);
		REQUIRE(GameEngine != nullptr);
	}

	TEST_CASE("Basic engine start with render")
	{
		auto GameEngine = std::make_unique<AnnEngine>("BasicInitRender", RENDERER);
		REQUIRE(GameEngine != nullptr);

		AnnGetOnScreenConsole()->setVisible(true);

		auto renderer = AnnGetVRRenderer();
		REQUIRE_FALSE(renderer->getName().empty());

		const auto duration = 3;
		double sec;
		AnnGetVRRenderer()->_resetOgreTimer();
		while((sec = GameEngine->getTimeFromStartupSeconds()) < duration)
		{
			GameEngine->refresh();
			AnnDebug() << "Running for " << sec << "sec. out of " << duration;
		}
	}
}
