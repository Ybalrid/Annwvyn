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
	AnnGetSceneryManager()->setAmbiantLight(AnnColor(0, 0, 0));
	
	//Remove the level lights
	levelLightingIdMap.clear();
	for (auto obj : levelLighting)
		AnnGetGameObjectManager()->removeLightObject(obj);
	levelLighting.clear();

	//Remove the level objects
	levelContentIdMap.clear();
	for(auto obj : levelContent)
		AnnGetGameObjectManager()->removeGameObject(obj);
	levelContent.clear();

	//Remove volumetric event triggers
	levelTriggerIdMap.clear();
	for(auto obj : levelTrigger)
		AnnGetGameObjectManager()->removeTriggerObject(obj);
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
