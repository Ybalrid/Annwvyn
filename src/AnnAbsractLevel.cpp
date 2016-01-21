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

	//Remove volumetric event triggers
	for(AnnTriggerObjectVect::iterator it = levelTrigger.begin(); it != levelTrigger.end(); ++it)
		AnnEngine::Instance()->destroyTriggerObject(*it);
	levelTrigger.clear();
}

AnnLightObject* AnnAbstractLevel::addLight(std::string id)
{
	AnnEngine* engine(AnnEngine::Instance());
	AnnLightObject* light (engine->createLightObject());
	levelLighting.push_back(light);
	return light;
}

AnnGameObject* AnnAbstractLevel::addGameObject(std::string entityName, std::string id)
{
	AnnEngine* engine(AnnEngine::Instance());
	AnnGameObject* object(engine->createGameObject(entityName.c_str()));
	object->setID(id);
	return object;
}

AnnTriggerObject* AnnAbstractLevel::addTrggerObject(AnnTriggerObject* obj , std::string id)
{
	AnnEngine* engine(AnnEngine::Instance());
	engine->createTriggerObject(obj);
	return obj;
}

std::string AnnAbstractLevel::generateRandomID(size_t len)
{
	std::string id;
	std::string buffer = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";

	for(size_t i(0); i < len; i++)
		id+= buffer.substr(rand()%buffer.size(), 1);

	return id;
}
