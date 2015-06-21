#include "stdafx.h"
#include "AnnAbstractLevel.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnAbstractLevel::AnnAbstractLevel()
{
	AnnDebug() << "A level has been created";
}

AnnAbstractLevel::~AnnAbstractLevel()
{
	unload();
	AnnDebug() << "Destroying a level";
}

void AnnAbstractLevel::unload()
{
	//Remove background music
	AnnEngine::Instance()->getAudioEngine()->stopBGM();
	//Remove the sky
	AnnEngine::Instance()->removeSkyDome();

	//Remove the ambiant lighting
	AnnEngine::Instance()->setAmbiantLight(Ogre::ColourValue::Black);
	
	//Remove the level lights
	for(AnnLightVect::iterator it = levelLighting.begin(); it != levelLighting.end(); ++it)
		AnnEngine::Instance()->getSceneManager()->destroyLight(*it);
	levelLighting.clear();

	//Remove the level objects
	for(AnnGameObjectVect::iterator it = levelContent.begin(); it != levelContent.end(); ++it)
		AnnEngine::Instance()->destroyGameObject(*it);
	levelContent.clear();
}