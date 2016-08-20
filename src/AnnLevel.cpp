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
	levelLightingIdMap.clear();
	for(AnnLightList::iterator it = levelLighting.begin(); it != levelLighting.end(); ++it)
		AnnGetGameObjectManager()->removeLightObject(*it);
	levelLighting.clear();

	//Remove the level objects
	levelContentIdMap.clear();
	for(auto it = levelContent.begin(); it != levelContent.end(); ++it)
		AnnGetGameObjectManager()->removeGameObject(*it);
	levelContent.clear();

	//Remove volumetric event triggers
	levelTriggerIdMap.clear();
	for(AnnTriggerObjectList::iterator it = levelTrigger.begin(); it != levelTrigger.end(); ++it)
		AnnGetGameObjectManager()->removeTriggerObject(*it);
	levelTrigger.clear();
}

std::shared_ptr<AnnLightObject> AnnLevel::addLightObject(std::string id)
{
	auto light (AnnGetGameObjectManager()->createLightObject());
	levelLighting.push_back(light);
	levelLightingIdMap[id] = light;
	return light;
}

std::shared_ptr<AnnGameObject> AnnLevel::addGameObject(std::string entityName, std::string id)
{
	auto object(AnnGetGameObjectManager()->createGameObject(entityName.c_str()));
	object->setID(id);
	levelContent.push_back(object);
	levelContentIdMap[id] = object;
	return object;
}
std::shared_ptr<AnnTriggerObject> AnnLevel::addTrggerObject(std::shared_ptr<AnnTriggerObject> obj , std::string id)
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
