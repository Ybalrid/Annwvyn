#include "stdafx.h"
#include "AnnSplashLevel.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

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
	AnnGetPlayer()->ignorePhysics = true;
	AnnGetPlayer()->setPosition({ 0,0,10 });
	AnnGetPlayer()->setOrientation(Ogre::Euler(0, 0, 0));
	AnnGetPhysicsEngine()->changeGravity(AnnVect3::ZERO);

	float ev = 1;
	AnnGetSceneryManager()->setExposure(ev, ev, ev);
	AnnGetSceneryManager()->setSkyColor(AnnColor(0, 0, 0), 0);

	//Create manual material
	auto unlit = AnnGetVRRenderer()->getRoot()->getHlmsManager()->getHlms(Ogre::HLMS_UNLIT);
	auto datablock = unlit->getDatablock("Splash");

	if (!datablock)
	{
		auto macroblock = Ogre::HlmsMacroblock();
		auto blendblock = Ogre::HlmsBlendblock();
		macroblock.mCullMode = Ogre::CULL_NONE;
		datablock = unlit->createDatablock("Splash", "Splash", macroblock, blendblock, Ogre::HlmsParamVec(), true, Ogre::BLANKSTRING, AnnGetResourceManager()->defaultResourceGroupName);
	}

	//Get the texture
	auto texture = Ogre::TextureManager::getSingleton().getByName(splashImage);
	if (!texture) texture = Ogre::TextureManager::getSingleton().load(splashImage, AnnGetResourceManager()->defaultResourceGroupName);
	if (!texture) throw AnnInitializationError(ANN_ERR_NOTINIT, "Texture not found for splash");

	//Set datablock parameters
	static_cast<Ogre::HlmsUnlitDatablock*>(datablock)->setColour(Ogre::ColourValue::White * 97000);
	static_cast<Ogre::HlmsUnlitDatablock*>(datablock)->setTexture(Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE, 0, texture);

	//Create manual object
	AnnDebug() << "Creating the display \"plane\" for the splash";
	auto smgr(AnnGetEngine()->getSceneManager());
	CurvedPlane = smgr->createManualObject(Ogre::SCENE_STATIC);

	CurvedPlane->begin("Splash", Ogre::OT_TRIANGLE_STRIP);

	const auto CurveC(1.f);
	const auto CurveB(CurveC / 4);
	int index = 0;
	//10 vertex:
	//1
	CurvedPlane->position(-2, -2, CurveC);
	CurvedPlane->textureCoord(0, 1);
	CurvedPlane->index(index++);
	//2
	CurvedPlane->position(-2, 2, CurveC);
	CurvedPlane->textureCoord(0, 0);
	CurvedPlane->index(index++);
	//3
	CurvedPlane->position(-1, -2, CurveB);
	CurvedPlane->textureCoord(.25, 1);
	CurvedPlane->index(index++);
	//4
	CurvedPlane->position(-1, 2, CurveB);
	CurvedPlane->textureCoord(.25, 0);
	CurvedPlane->index(index++);
	//5
	CurvedPlane->position(0, -2, 0);
	CurvedPlane->textureCoord(.5, 1);
	CurvedPlane->index(index++);
	//6
	CurvedPlane->position(0, 2, 0);
	CurvedPlane->textureCoord(.5, 0);
	CurvedPlane->index(index++);
	//7
	CurvedPlane->position(1, -2, CurveB);
	CurvedPlane->textureCoord(.75, 1);
	CurvedPlane->index(index++);
	//8
	CurvedPlane->position(1, 2, CurveB);
	CurvedPlane->textureCoord(.75, 0);
	CurvedPlane->index(index++);
	//9
	CurvedPlane->position(2, -2, CurveC);
	CurvedPlane->textureCoord(1, 1);
	CurvedPlane->index(index++);
	//10
	CurvedPlane->position(2, 2, CurveC);
	CurvedPlane->textureCoord(1, 0);
	CurvedPlane->index(index++);

	CurvedPlane->end();

	AnnDebug() << "Add plane to scene";
	Splash = smgr->getRootSceneNode()->createChildSceneNode(Ogre::SCENE_STATIC);
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
	auto nextLevel = next.lock();
	if (nextLevel && AnnGetEngine()->getTimeFromStartUp() - startTime > timeout)
	{
		startTime = -1;
		AnnGetEngine()->getLevelManager()->jump(nextLevel);
	}
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