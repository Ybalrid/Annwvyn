#include "stdafx.h"
#include "AnnGameObjectManager.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"

using namespace Annwvyn;
unsigned long long AnnGameObjectManager::id;

AnnGameObjectManager::AnnGameObjectManager() : AnnSubSystem("GameObjectManager")
{
}

void AnnGameObjectManager::update()
{
	//Run animations and update OpenAL sources position
	for (auto gameObject : Objects)
	{
		gameObject->addAnimationTime(AnnGetEngine()->getFrameTime());
		gameObject->updateOpenAlPos();
		gameObject->atRefresh();
		gameObject->callUpdateOnScripts();
	}
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::createGameObject(const char entityName[], std::string identifier, std::shared_ptr<AnnGameObject> obj)
{
	AnnDebug("Creating a game object from the entity " + std::string(entityName));
	auto smgr{ AnnGetEngine()->getSceneManager() };
	Ogre::Entity* ent = smgr->createEntity(entityName);
	Ogre::SceneNode* node = smgr->getRootSceneNode()->createChildSceneNode();

	node->attachObject(ent);
	obj->setNode(node);
	obj->setEntity(ent);
	obj->audioSource = AnnGetAudioEngine()->createSource();

	//id will be unique to every non-identified object.
	//The identifier name can be empty, meaning that we have to figure out an unique name.
	//In that case we will append to the entity name + a number that will always be incremented.
	if (identifier.empty())
		identifier = entityName + ++id;

	AnnDebug() << "The object " << identifier << " has been created. Annwvyn memory address " << obj;
	AnnDebug() << "This object take " << sizeof(*(obj.get())) << " bytes";

	obj->name = identifier;
	identifiedObjects[identifier] = obj;
	Objects.push_back(std::shared_ptr<AnnGameObject>(obj)); //keep addreAnnDebug() in list

	obj->postInit();
	return obj;
}

void AnnGameObjectManager::removeGameObject(std::shared_ptr<AnnGameObject> object)
{
	AnnDebug() << "Removed object " << object.get();
	Objects.remove(object);
	identifiedObjects.erase(object->getName());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getFromNode(Ogre::SceneNode* node)
{
	AnnDebug() << "Trying to identify object at address " << (void*)node;

	auto result = std::find_if(Objects.begin(), Objects.end(), [&](std::shared_ptr<AnnGameObject> object) {return object->getNode() == node; });
	if (result != Objects.end())
		return *result;

	AnnDebug() << "The Scene Node" << (void*)node << " doesn't belong to any AnnGameObject";
	return nullptr;
}

void AnnGameObjectManager::removeLightObject(std::shared_ptr<AnnLightObject> light)
{
	Lights.remove(light);
}

std::shared_ptr<AnnLightObject> Annwvyn::AnnGameObjectManager::createLightObject()
{
	AnnDebug("Creating a light");
	auto Light = std::make_shared<AnnLightObject>(AnnGetEngine()->getSceneManager()->createLight());
	Light->setType(AnnLightObject::LightTypes::ANN_LIGHT_POINT);
	Lights.push_back(Light);
	return Light;
}

std::shared_ptr<AnnTriggerObject> Annwvyn::AnnGameObjectManager::createTriggerObject(std::shared_ptr<AnnTriggerObject> trigger)
{
	AnnDebug("Creating a trigger object");
	Triggers.push_back(trigger);
	trigger->postInit();
	return trigger;
}

void AnnGameObjectManager::removeTriggerObject(std::shared_ptr<AnnTriggerObject> trigger)
{
	Triggers.remove(trigger);
}

std::shared_ptr<AnnGameObject> Annwvyn::AnnGameObjectManager::playerLookingAt()
{
	//Origin vector of the ray
	AnnVect3 Orig(AnnGetEngine()->getHmdPose().position);

	//Calculate direction Vector of the ray to be the midpoint camera optical axis
	AnnVect3 LookAt(AnnQuaternion(AnnGetEngine()->getHmdPose().orientation).getAtVector());

	//create ray
	Ogre::Ray ray(Orig, LookAt);

	//create query
	Ogre::RaySceneQuery* raySceneQuery(AnnGetEngine()->getSceneManager()->createRayQuery(ray));
	raySceneQuery->setSortByDistance(true);

	//execute and get the results
	Ogre::RaySceneQueryResult& result(raySceneQuery->execute());

	//read the result list
	for (auto it(result.begin()); it != result.end(); it++)
		if (it->movable && it->movable->getMovableType() == "Entity")
			return AnnGetGameObjectManager()->getFromNode(it->movable->getParentSceneNode());//Get the AnnGameObject that is attached to this SceneNode

	return nullptr; //means that we don't know what the player is looking at.
}

std::shared_ptr<AnnGameObject> Annwvyn::AnnGameObjectManager::getObjectFromID(std::string idString)
{
	auto object = identifiedObjects.find(idString);
	if (object != identifiedObjects.end())
		return object->second;
	return nullptr;
}