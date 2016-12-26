#include "stdafx.h"
#include "AnnSplashLevel.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnSplashLevel::AnnSplashLevel(Ogre::String resourceName, std::shared_ptr<AnnLevel> nextLevel, float timeoutTime) : constructLevel(),
timeout(timeoutTime * 1000),
currentTime(0),
startTime(-1),
next(nextLevel),
CurvedPlane{ nullptr },
Splash{ nullptr },
splashImage(resourceName),
hasBGM(false) {}

void AnnSplashLevel::load()
{
	AnnDebug() << "Ignore physics";
	AnnGetSceneryManager()->setWorldBackgroundColor(AnnColor(Ogre::ColourValue::Black));
	AnnGetPlayer()->ignorePhysics = true;
	AnnGetPlayer()->setOrientation(Ogre::Euler(0, 0, 0));
	AnnGetPhysicsEngine()->changeGravity(AnnVect3::ZERO);

	//Create manual material
	AnnDebug() << "Creating a material with no culling, now lighting, and the wanted texture";
	auto Console = Ogre::MaterialManager::getSingleton().create("Splash", "General", true);
	auto technique = Console.getPointer()->getTechnique(0);
	auto pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	pass->setCullingMode(Ogre::CullingMode::CULL_NONE);
	auto displaySurfaceTextureUniteState = pass->createTextureUnitState();
	auto texture = Ogre::TextureManager::getSingleton().load(splashImage, "General");
	displaySurfaceTextureUniteState->setTexture(texture);

	//Create manual object
	AnnDebug() << "Creating the display \"plane\" for the splash";
	auto smgr(AnnGetEngine()->getSceneManager());
	CurvedPlane = smgr->createManualObject(generateRandomID());

	CurvedPlane->begin("Splash", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

	const auto CurveC(1.f);
	const auto CurveB(CurveC / 4);
	//10 vertex:
	//1
	CurvedPlane->position(-2, -2, CurveC);
	CurvedPlane->textureCoord(0, 1);
	//2
	CurvedPlane->position(-2, 2, CurveC);
	CurvedPlane->textureCoord(0, 0);
	//3
	CurvedPlane->position(-1, -2, CurveB);
	CurvedPlane->textureCoord(.25, 1);
	//4
	CurvedPlane->position(-1, 2, CurveB);
	CurvedPlane->textureCoord(.25, 0);
	//5
	CurvedPlane->position(0, -2, 0);
	CurvedPlane->textureCoord(.5, 1);
	//6
	CurvedPlane->position(0, 2, 0);
	CurvedPlane->textureCoord(.5, 0);
	//7
	CurvedPlane->position(1, -2, CurveB);
	CurvedPlane->textureCoord(.75, 1);
	//8
	CurvedPlane->position(1, 2, CurveB);
	CurvedPlane->textureCoord(.75, 0);
	//9
	CurvedPlane->position(2, -2, CurveC);
	CurvedPlane->textureCoord(1, 1);
	//10
	CurvedPlane->position(2, 2, CurveC);
	CurvedPlane->textureCoord(1, 0);

	CurvedPlane->end();

	AnnDebug() << "Add plane to scene";
	Splash = smgr->getRootSceneNode()->createChildSceneNode();
	Splash->attachObject(CurvedPlane);
	Splash->setScale(10, 10, 10);
}

void AnnSplashLevel::setBGM(std::string path, bool preload)
{
	if (preload) AnnGetAudioEngine()->preLoadBuffer(path);
	bgmPath = path;
	hasBGM = true;
}

void AnnSplashLevel::runLogic()
{
	//If start time not set yet
	if (startTime == -1)
	{
		//The app is "not visible" before the Health and Safety warning is displayed, or when you're inside the Oculus Home menu
		if (AnnGetEngine()->appVisibleInHMD())
		{
			AnnDebug() << "visible";
			//This set the "startTime" variable, preventing this piece of code to be ran twice
			AnnDebug() << "Starting time at : " << AnnGetEngine()->getTimeFromStartUp();
			startTime = AnnGetEngine()->getTimeFromStartUp();
			//If you put some background music or sound for the splash-screen, we start it
			if (hasBGM)
				AnnGetAudioEngine()->playBGM(bgmPath);
		}
		else return;
	}

	//Run the following only if you set a "next" level to jump to
	if (auto nextLevel = next.lock() &&
		AnnGetEngine()->getTimeFromStartUp() - startTime > timeout)
		AnnGetEngine()->getLevelManager()->jump(nextLevel);
}

void AnnSplashLevel::unload()
{
	//Normal level unload
	AnnLevel::unload();

	AnnDebug() << "Removing object from scene";
	auto smgr(AnnGetEngine()->getSceneManager());
	smgr->destroySceneNode(Splash);
	Ogre::MaterialManager::getSingleton().remove("Splash");

	AnnDebug() << "Restore Player's normal state : ";
	AnnGetPlayer()->ignorePhysics = false;
	AnnGetPlayer()->setPosition(AnnVect3(0, 0, 10));
	AnnGetPlayer()->resetPlayerPhysics();
	AnnGetSceneryManager()->setWorldBackgroundColor();
}

void AnnSplashLevel::setNextLevel(std::shared_ptr<AnnLevel> level)
{
	next = level;
}

void AnnSplashLevel::setTimeout(float time)
{
	if (time > 0)
		timeout = 1000 * time;
}

void AnnSplashLevel::setTimeoutMillisec(unsigned time)
{
	timeout = time;
}