#include "stdafx.h"
#include "AnnSplashLevel.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnSplashLevel::AnnSplashLevel(Ogre::String resourceName, AnnAbstractLevel* nextLevel, float timeoutTime) : constructLevel(),
	next(nextLevel),
	timeout(timeoutTime*1000),
	currentTime(0),
	startTime(-1),
	splashImage(resourceName),
	hasBGM(false)
{
}

void AnnSplashLevel::load()
{
	AnnDebug() << "Ignore physics";
	AnnGetEngine()->setWorldBackgroundColor(AnnColor(Ogre::ColourValue::Black));
	AnnGetPlayer()->ignorePhysics = true;
	AnnGetPlayer()->setOrientation(Ogre::Euler(0,0,0));

	//Create manual material
	AnnDebug() << "Creating a material with no culling, now lighting, and the wanted texture";
	Ogre::MaterialPtr Console = Ogre::MaterialManager::getSingleton().create("Splash", "General", true);
	Ogre::Technique* technique = Console.getPointer()->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	pass->setCullingMode(Ogre::CullingMode::CULL_NONE);
	Ogre::TextureUnitState* displaySurfaceTextureUniteState = pass->createTextureUnitState();
	Ogre::TexturePtr texture =  Ogre::TextureManager::getSingleton().load(splashImage, "General");
	displaySurfaceTextureUniteState->setTexture(texture);

	//Create manual object
	AnnDebug() << "Creating the diplay \"plane\" for the splash";
	auto smgr(AnnGetEngine()->getSceneManager());
	CurvedPlane = smgr->createManualObject(generateRandomID());

	CurvedPlane->begin("Splash", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

	const float CurveC (1);
	const float CurveB (CurveC/4);
	//10 vertex:
	//1
	CurvedPlane->position(-2, -2, CurveC);
	CurvedPlane->textureCoord(0, 1);
	//2
	CurvedPlane->position(-2, 2, CurveC);
	CurvedPlane->textureCoord(0 , 0);
	//3
	CurvedPlane->position(-1, -2, CurveB);
	CurvedPlane->textureCoord(.25 , 1);
	//4
	CurvedPlane->position(-1, 2, CurveB);
	CurvedPlane->textureCoord(.25, 0);
	//5
	CurvedPlane->position(0, -2, 0);
	CurvedPlane->textureCoord(.5 , 1);
	//6
	CurvedPlane->position(0, 2, 0);
	CurvedPlane->textureCoord(.5 ,0);
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
	Splash->setScale(10,10,10);


}

void AnnSplashLevel::setBGM(std::string path, bool preload)
{
	if(preload) AnnGetAudioEngine()->preLoadBuffer(path);
	bgmPath = path;
	hasBGM = true;
}

void AnnSplashLevel::runLogic()
{

	//If start time not set yet
	if(startTime == -1)
	{
		//The app is "not visible" before the Health and Safety warning is displayed, or when you're inside the Oculus Home menu
		if(AnnGetEngine()->appVisibleInHMD())
		{
			AnnDebug() << "visible";
			//This set the "startTime" variable, preventing this peice of code to be ran twice
			AnnDebug() << "Starting time at : " << AnnGetEngine()->getTimeFromStartUp();
			startTime = AnnGetEngine()->getTimeFromStartUp();
			//If you put some background music or sound for the splashscreen, we start it
			if(hasBGM)
				AnnGetAudioEngine()->playBGM(bgmPath);
		}
		else return;
	}

	//Run the following only if you set a "next" level to jump to
	if(next)
	{
		//test if the splash has timed out
		currentTime = AnnGetEngine()->getTimeFromStartUp();
		if(currentTime - startTime> timeout)
			//Jump to the next
			AnnGetEngine()->getLevelManager()->jump(next);
	}
}

void AnnSplashLevel::unload()
{
	//Normal level unload
	AnnAbstractLevel::unload();

	AnnDebug() << "Removing object from scene" ;
	auto smgr(AnnGetEngine()->getSceneManager());
	smgr->destroySceneNode(Splash);
	Ogre::MaterialManager::getSingleton().remove("Splash");

	AnnDebug() << "Restore Player's normal state : ";
	AnnGetPlayer()->ignorePhysics = false;
	AnnGetPlayer()->setPosition(AnnVect3(0,0,10));
	AnnGetEngine()->resetPlayerPhysics();
	AnnGetEngine()->setWorldBackgroundColor();
}

void AnnSplashLevel::setNextLevel(AnnAbstractLevel* level)
{
	next = level;
}

void AnnSplashLevel::setTimeout(float time)
{
	if(time > 0)
		timeout = 1000*time;
}

void AnnSplashLevel::setTimeoutMillisec(unsigned time)
{
	timeout = time;
}