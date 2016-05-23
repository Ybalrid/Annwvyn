#include "stdafx.h"
#include "AnnLevel.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnLevel::AnnLevel()
{
	AnnDebug() << "A level has been created";
}

AnnLevel::~AnnLevel()
{
	unload();
	AnnDebug() << "Destroying a level";
}

void AnnLevel::unload()
{
	//Remove background music
	AnnGetAudioEngine()->stopBGM();
	//Remove the sky
	AnnGetSceneryManager()->removeSkyDome();

	//Remove the ambiant lighting
	AnnGetSceneryManager()->setAmbiantLight(AnnColor(0,0,0));
	
	//Remove the level lights
	for(AnnLightList::iterator it = levelLighting.begin(); it != levelLighting.end(); ++it)
		AnnGetGameObjectManager()->destroyLightObject(*it);
	levelLighting.clear();
	levelLightingIdMap.clear();

	//Remove the level objects
	for(AnnGameObjectList::iterator it = levelContent.begin(); it != levelContent.end(); ++it)
		AnnGetGameObjectManager()->destroyGameObject(*it);
	levelContent.clear();
	levelContentIdMap.clear();

	//Remove volumetric event triggers
	for(AnnTriggerObjectList::iterator it = levelTrigger.begin(); it != levelTrigger.end(); ++it)
		AnnGetGameObjectManager()->destroyTriggerObject(*it);
	levelTrigger.clear();
	levelTriggerIdMap.clear();
}

AnnLightObject* AnnLevel::addLightObject(std::string id)
{
	AnnLightObject* light (AnnGetGameObjectManager()->createLightObject());
	levelLighting.push_back(light);
	levelLightingIdMap[id] = light;
	return light;
}

AnnGameObject* AnnLevel::addGameObject(std::string entityName, std::string id)
{
	AnnGameObject* object(AnnGetGameObjectManager()->createGameObject(entityName.c_str()));
	object->setID(id);
	levelContent.push_back(object);
	levelContentIdMap[id] = object;
	return object;
}

AnnTriggerObject* AnnLevel::addTrggerObject(AnnTriggerObject* obj , std::string id)
{
	AnnGetGameObjectManager()->createTriggerObject(obj);
	levelTrigger.push_back(obj);
	levelTriggerIdMap[id] = obj;
	return obj;
}

std::string AnnLevel::generateRandomID(size_t len)
{
	std::string id;
	std::string buffer = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";

	for(size_t i(0); i < len; i++)
		id+= buffer.substr(rand()%buffer.size(), 1);

	return id;
}
