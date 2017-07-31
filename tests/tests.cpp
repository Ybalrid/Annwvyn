//To speed up things a bit, we precompile headers
#include "stdafx.h"

//Include the engine itself
#include <Annwvyn.h>

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

void consolePause();

int main(int argc, char* argv[])
{
	auto result = Catch::Session().run(argc, argv);

	consolePause();

	return (result < 0xff ? result : 0xff);
}

void consolePause()
{
	std::cout << "Press RETURN to continue... ";
	std::cin.get();
}