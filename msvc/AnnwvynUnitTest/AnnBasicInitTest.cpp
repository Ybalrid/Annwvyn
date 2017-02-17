#include "stdafx.h"
#include "CppUnitTest.h"
#include <Annwvyn.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Annwvyn;

namespace AnnwvynUnitTest
{
	TEST_CLASS(AnnBasicInit)
	{
		TEST_METHOD(AnnInitOnceTest)
		{
			//Assert that the environment is clean
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr", LINE_INFO());

			//Init the engine
			auto engine = std::make_unique<AnnEngine>("AnnInitOnceTest", "OgreNoVRRender");

			//Assert that engine is initialized
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is initialized", LINE_INFO());

			//Release pointer -> this destruct the engine -> this set back the instance to nullptr
			engine.reset(nullptr);
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr again", LINE_INFO());
		}

		TEST_METHOD(AnnInitMultipleTest)
		{
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr", LINE_INFO());
			auto engine0 = std::make_unique<AnnEngine>("AnnInitOnceTest", "OgreNoVRRender");
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is initialized", LINE_INFO());
			engine0.reset(nullptr);
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr again", LINE_INFO());

			//repeat:
			auto engine1 = std::make_unique<AnnEngine>("AnnInitOnceTest", "OgreNoVRRender");
			Assert::AreNotEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is initialized", LINE_INFO());
			engine1.reset(nullptr);
			Assert::AreEqual(uint64_t(NULL), uint64_t(AnnGetEngine()), L"AnnEngine is nullptr again", LINE_INFO());
		}
	};
}