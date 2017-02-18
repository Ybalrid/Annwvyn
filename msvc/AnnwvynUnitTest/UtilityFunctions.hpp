#pragma once

#include <Annwvyn.h>

namespace TestUtilityFunctions
{
	static void openConsole()
	{
		Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(Annwvyn::AnnEngine::openConsole());
	}
	static std::unique_ptr<Annwvyn::AnnEngine> startTestEngine(const std::string& testName)
	{
		auto engine = std::make_unique<Annwvyn::AnnEngine>(testName.c_str(), "OgreNoVRRender");
		return engine;
	}
}