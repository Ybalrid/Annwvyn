#include "stdafx.h"
#include "AnnGameObjectManager.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"

using namespace Annwvyn;

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
	}

}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::createGameObject(const char entityName[], std::shared_ptr<AnnGameObject> obj)
{

	AnnDebug("Creatig a game object from the entity " + std::string(entityName));
	
	//TODO: either choose to throw an execption or quit the program. If it's true, there's a programing error. And "tomber en marche" is bad. 
	if (std::string(entityName).empty())
	{
		AnnDebug("Hey! what are you trying to do here? Please specify a non empty string for entityName !");
		obj = nullptr;
		return nullptr;
	}

	Ogre::Entity* ent = AnnGetEngine()->getSceneManager()->createEntity(entityName);
	Ogre::SceneNode* node = AnnGetEngine()->getSceneManager()->getRootSceneNode()->createChildSceneNode();

	node->attachObject(ent);
	obj->setNode(node);
	obj->setEntity(ent);
	obj->audioSource = AnnGetAudioEngine()->createSource();
	if (AnnGetPhysicsEngine())
		obj->setBulletDynamicsWorld(AnnGetPhysicsEngine()->getWorld());
	obj->postInit(); //Run post init directives

	Objects.push_back(shared_ptr<AnnGameObject>(obj)); //keep addreAnnDebug() in list

	AnnDebug() << "The object " << entityName << " has been created. Annwvyn memory address " << obj;

	return obj;
}


void AnnGameObjectManager::removeGameObject(std::shared_ptr<AnnGameObject> object)
{
	AnnDebug() << "Removed object " << object.get();
	Objects.remove(object);
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getFromNode(Ogre::SceneNode* node)
{
	if (!node)
	{
		AnnDebug("Plese do not try to identify a NULL");
		return NULL;
	}
	AnnDebug() << "Trying to identify object at address " << (void*)node;

	//This methods only test memory address
	for (auto object : Objects)
		if ((void*)object->getNode() == (void*)node)
			return object;
	AnnDebug() << "The object " << (void*)node << " doesn't belong to any AnnGameObject";

	return NULL;
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

	//Caltulate direction Vector of the ray to be the midpont camera optical axis
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