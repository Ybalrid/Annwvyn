#include "stdafx.h"
#include "AnnLevel.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnLevel::AnnLevel()
{
	AnnDebug() << "Level object " << this << " has been created";
}

AnnLevel::~AnnLevel()
{
	AnnDebug() << "Running generic level unload procedure now.";
	AnnLevel::unload();
}

void AnnLevel::unload()
{
	AnnDebug() << "Unloading the level " << this << " From the scene";

	//Remove background music
	AnnGetAudioEngine()->stopBGM();

	//Reset scene
	AnnGetSceneryManager()->resetSceneParameters();

	//Restore the default gravity
	AnnGetPhysicsEngine()->resetGravity();

	//Remove the level lights
	levelLightingIdMap.clear();
	for (auto obj : levelLighting)
		AnnGetGameObjectManager()->removeLightObject(obj);
	levelLighting.clear();

	//Remove the level objects
	for (auto obj : levelContent)
		AnnGetGameObjectManager()->removeGameObject(obj);
	levelContent.clear();

	//Remove volumetric event triggers
	levelTriggerIdMap.clear();
	for (auto obj : levelTrigger)
		AnnGetGameObjectManager()->removeTriggerObject(obj);
	levelTrigger.clear();

	levelMovable.clear();

	AnnGetPlayer()->hintRoomscaleUpdateTranslationReference();
}

std::shared_ptr<AnnLightObject> AnnLevel::addLightObject(std::string id)
{
	auto light(AnnGetGameObjectManager()->createLightObject());
	levelLighting.push_back(light);
	levelLightingIdMap[id] = light;
	return light;
}

std::shared_ptr<AnnGameObject> AnnLevel::addGameObject(std::string entityName, std::string objectName)
{
	auto object(AnnGetGameObjectManager()->createGameObject(entityName.c_str(), objectName));
	levelContent.push_back(object);
	return object;
}

void AnnLevel::addManualMovableObject(std::shared_ptr<AnnAbstractMovable> movable)
{
	levelMovable.push_back(movable);
}

std::shared_ptr<AnnTriggerObject> AnnLevel::addTrggerObject(std::string id)
{
	auto obj = AnnGetGameObjectManager()->createTriggerObject();
	levelTrigger.push_back(obj);
	levelTriggerIdMap[id] = obj;
	return obj;
}

AnnGameObjectList& AnnLevel::getContent()
{
	return levelContent;
}

AnnLightList& AnnLevel::getLights()
{
	return levelLighting;
}