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
	TEST_CLASS(AnnGameObjetManipulationTest)
	{
		TEST_METHOD(AnnObjectNameTest)
		{
			auto engine = startTestEngine("AnnObjectNameTest");
			AnnGetResourceManager()->initResources();

			auto Ogre = AnnGetGameObjectManager()->createGameObject("Sinbad.mesh", "TheOgre");
			auto OgreByName = AnnGetGameObjectManager()->getObjectFromID("TheOgre");

			//I'm too lazy to use their "ToString" thing. So, we take the 2 pointers as 2 64bits unsigned integer. that should do it.
			Assert::AreEqual(uint64_t(Ogre.get()), uint64_t(OgreByName.get()));
			AnnDebug() << "Hey, apparently " << Ogre.get() << " and " << OgreByName.get() << " are equals";
		}

		TEST_METHOD(AnnGameObjectCreateDeleteTest)
		{
			auto engine = startTestEngine("AnnGameObjectCreateDeleteTest");
			AnnGetResourceManager()->initResources();

			AnnGetGameObjectManager()->createGameObject("Sinbad.mesh", "TheOgre");
			AnnGetGameObjectManager()->removeGameObject(AnnGetGameObjectManager()->getObjectFromID("TheOgre"));
			auto object = AnnGetGameObjectManager()->getObjectFromID("TheOgre");

			//Should be null now...
			Assert::AreEqual(uint64_t(NULL), uint64_t(object.get()));
		}

		TEST_METHOD(AnnDeleteNonExistingObject)
		{
			auto engine = startTestEngine("AnnDeleteNonExistingObject");
			auto catched{ false };
			try
			{
				auto obj = AnnGetGameObjectManager()->getObjectFromID("TheOgre");
				AnnGetGameObjectManager()->removeGameObject(obj);
				obj.reset();
			}
			catch (const std::exception& e)
			{
				catched = true;
				AnnDebug() << e.what();
			}

			Assert::IsTrue(catched);
		}
	};
}