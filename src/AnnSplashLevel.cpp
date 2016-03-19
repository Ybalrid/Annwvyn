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
	AnnEngine::Instance()->setWorldBackgroundColor(Ogre::ColourValue::Black);
	AnnEngine::Instance()->getPlayer()->ignorePhysics = true;
	AnnEngine::Instance()->getPlayer()->setOrientation(Ogre::Euler(0,0,0));

	//Create manual material
	AnnDebug() << "Creating a material with no culling, now lighting, and the wanted texture";
	Ogre::MaterialPtr Console = Ogre::MaterialManager::getSingleton().create("Splash", "General", true);
	Ogre::Technique* technique = Console.getPointer()->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	pass->setCullingMode(Ogre::CullingMode::CULL_NONE);
	Ogre::TextureUnitState* displaySurfaceTextureUniteState = pass->createTextureUnitState();
	Ogre::TexturePtr texture =  TextureManager::getSingleton().load(splashImage, "General");
	displaySurfaceTextureUniteState->setTexture(texture);

	//Create manual object
	AnnDebug() << "Creating the diplay \"plane\" for the splash";
	auto smgr(AnnEngine::Instance()->getSceneManager());
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

	AnnDebug() << "Starting time at : " << AnnEngine::Instance()->getTimeFromStartUp();
	startTime = AnnEngine::Instance()->getTimeFromStartUp();
	if(hasBGM)
		AnnEngine::Instance()->getAudioEngine()->playBGM(bgmPath);
}

void AnnSplashLevel::setBGM(std::string path, bool preload)
{
	if(preload) AnnEngine::Instance()->getAudioEngine()->preLoadBuffer(path);
	bgmPath = path;
	hasBGM = true;
}

void AnnSplashLevel::runLogic()
{
	if(!next) return;
	currentTime = AnnEngine::Instance()->getTimeFromStartUp();
	if(currentTime - startTime> timeout)
		AnnEngine::Instance()->getLevelManager()->jump(next);
}

void AnnSplashLevel::unload()
{
	//Normal level unload
	AnnAbstractLevel::unload();

	AnnDebug() << "Removing object from scene" ;
	auto smgr(AnnEngine::Instance()->getSceneManager());
	smgr->destroySceneNode(Splash);
	Ogre::MaterialManager::getSingleton().remove("Splash");

	AnnDebug() << "Restore Player's normal state : ";
	auto player (AnnEngine::Instance()->getPlayer());
	player->ignorePhysics = false;
	player->setPosition(AnnVect3(0,0,10));
	AnnEngine::Instance()->resetPlayerPhysics();
	AnnEngine::Instance()->setWorldBackgroundColor();
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