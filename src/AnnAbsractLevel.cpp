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
	AnnGetAudioEngine()->stopBGM();
	//Remove the sky
	AnnGetEngine()->removeSkyDome();

	//Remove the ambiant lighting
	AnnGetEngine()->setAmbiantLight(AnnColor(0,0,0));
	
	//Remove the level lights
	for(AnnLightList::iterator it = levelLighting.begin(); it != levelLighting.end(); ++it)
		AnnGetEngine()->destroyLightObject(*it);
	levelLighting.clear();

	//Remove the level objects
	for(AnnGameObjectList::iterator it = levelContent.begin(); it != levelContent.end(); ++it)
		AnnGetGameObjectManager()->destroyGameObject(*it);
	levelContent.clear();

	//Remove volumetric event triggers
	for(AnnTriggerObjectList::iterator it = levelTrigger.begin(); it != levelTrigger.end(); ++it)
		AnnGetEngine()->destroyTriggerObject(*it);
	levelTrigger.clear();
}

AnnLightObject* AnnAbstractLevel::addLightObject(std::string id)
{
	AnnLightObject* light (AnnGetEngine()->createLightObject());
	levelLighting.push_back(light);
	return light;
}

AnnGameObject* AnnAbstractLevel::addGameObject(std::string entityName, std::string id)
{
	AnnGameObject* object(AnnGetGameObjectManager()->createGameObject(entityName.c_str()));
	object->setID(id);
	levelContent.push_back(object);
	return object;
}

AnnTriggerObject* AnnAbstractLevel::addTrggerObject(AnnTriggerObject* obj , std::string id)
{
	AnnGetEngine()->createTriggerObject(obj);
	levelTrigger.push_back(obj);
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
