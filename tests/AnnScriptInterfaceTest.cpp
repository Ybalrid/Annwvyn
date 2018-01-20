#include <stdafx.h>
#include <engineBootstrap.hpp>

namespace Annwvyn
{
	TEST_CASE("Attach script to object")
	{
		auto GameEngine = bootstrapEmptyEngine("TestScript");

		auto ResourceManager = AnnGetResourceManager();
		ResourceManager->addFileLocation("./unitTestScripts");
		ResourceManager->initResources();

		auto GameObjectManager = AnnGetGameObjectManager();
		auto sun			   = GameObjectManager->createLightObject();
		sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		sun->setDirection({ 0, -5, -1 });
		sun->setPower(97);
		auto floor = GameObjectManager->createGameObject("floorplane.mesh", "Floor");
		auto ogre  = GameObjectManager->createGameObject("Sinbad.mesh", "Ogre");

		//Attach the behavior that cause to move the object by +0.02f on +y every frame
		ogre->attachScript("GoUpBehavior");

		//run 250 frames of the game
		auto counter{ 0 };
		while(GameEngine->refresh())
		{
			if(counter++ > 250) break;
		}

		//Check if script ran correctly
		REQUIRE(ogre->getPosition().y >= 5);
	}

	TEST_CASE("Object manipulation via scripting")
	{
		//Get the engine components
		auto GameEngine			 = bootstrapTestEngine("TestScriptObject");
		const auto scriptManager = AnnGetScriptManager();

		//Function for rendering a few seconds of gameplay
		auto renderForSecs = [&](const double duration = 2) {
			auto time = GameEngine->getTimeFromStartupSeconds() + duration;
			while(GameEngine->refresh())
				if(GameEngine->getTimeFromStartupSeconds() > time) break;
		};

		renderForSecs();
		AnnGetOnScreenConsole()->setVisible();
		scriptManager->evalString(R"( AnnCreateGameObject("Sinbad.mesh", "sinbad") )");
		renderForSecs(0.5);
		AnnGetOnScreenConsole()->setVisible(false);

		REQUIRE(AnnGetGameObjectManager()->getGameObject("sinbad"));

		scriptManager->evalString(R"(
		AnnGetGameObject("sinbad").setPosition(AnnVect3(0, 2, 8))
		AnnGetGameObject("sinbad").setScale(AnnVect3(0.2, 0.2, 0.2))
		)");

		renderForSecs();
	}

	TEST_CASE("Test scripting API")
	{
		using Ogre::Vector3;
		using std::function;

		auto GameEngine = bootstrapEmptyEngine("TestScripts");

		auto ScriptManager = AnnGetScriptManager();
		const auto result  = ScriptManager->evalFile("./unitTestScripts/UnitTestMain.chai");
		REQUIRE(result);

		//Get direct access to the underlying scripting engine (ChaiScript)
		auto chai = ScriptManager->_getEngine();

		int fortyTwo;
		SECTION("Evaluate function call form Chaiscript and get result")
		{
			fortyTwo = chai->eval<int>("returnFortyTwo()");
			REQUIRE(fortyTwo == 42);
		}

		SECTION("Get a functor to a function defined inside ChaiScript")
		{
			fortyTwo			= 0;
			auto returnFortyTwo = chai->eval<function<int()>>("returnFortyTwo");
			fortyTwo			= returnFortyTwo();
			REQUIRE(returnFortyTwo);
			REQUIRE(fortyTwo == 42);
		}

		SECTION("Get a functor to a function that return an object from ChaiScript")
		{
			std::string annwvyn;
			auto returnAnnwvyn = chai->eval<function<std::string()>>("returnAnnwvyn");
			annwvyn			   = returnAnnwvyn();
			REQUIRE(returnAnnwvyn);
			REQUIRE(annwvyn == "Annwvyn");
		}

		SECTION("Get a functor to a function that takes arguments")
		{
			const auto a = 1, b = 2;
			auto simpleSumInt = chai->eval<function<int(int, int)>>("simpleSum");
			REQUIRE(simpleSumInt);
			REQUIRE(simpleSumInt(a, b) == a + b);

			const auto c = 1.2, d = 2.1;
			auto simpleSumDouble = chai->eval<function<double(double, double)>>("simpleSum");
			REQUIRE(simpleSumDouble);
			REQUIRE(simpleSumDouble(c, d) == c + d);
		}

		SECTION("Test vector arithmetic via chaiscript")
		{
			const AnnVect3 a{ 1, 1, 1 };
			const AnnVect3 b{ 9, 9, 9 };
			const auto scalar = 3.14f;

			auto addVect3	= chai->eval<function<Vector3(Vector3, Vector3)>>("addVect3");
			auto subVect3	= chai->eval<function<Vector3(Vector3, Vector3)>>("subVect3");
			auto scalarVect3 = chai->eval<function<Vector3(float, Vector3)>>("scalarVect3");

			REQUIRE((a + b) == addVect3(a, b));
			REQUIRE((b - a) == subVect3(b, a));
			REQUIRE(scalar * a == scalarVect3(scalar, a));
		}
	}
}
