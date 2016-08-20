#include "stdafx.h"
#include "AnnGameObjectManager.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnGameObjectManager::AnnGameObjectManager() : AnnSubSystem("GameObjectManager")
{

}

AnnGameObjectManager::~AnnGameObjectManager()
{
	/*
	//In case of orphan objects, do their cleanup here. 
	AnnDebug("Destroying every objects remaining orphan object in engine");
	if (Objects.size() > 0)
		for (auto object : Objects)
			destroyGameObject(object);
	else
		AnnDebug("Object list allready clean");

	if (Lights.size() > 0)
		for (auto object : Lights)
			destroyLightObject(object);
	else
		AnnDebug("Light list allready clean");

	if (Triggers.size() > 0) 
		for (auto object : Triggers) 
			destroyTriggerObject(object);
	else 
		AnnDebug("Trigger list allready clean");
		*/
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
bool AnnGameObjectManager::destroyGameObject(std::shared_ptr<AnnGameObject> object)
{
	//Ogre::SceneManager* SceneManager(AnnGetEngine()->getSceneManager());
	if (!object) return false;
	bool returnCode(false);
	Objects.remove(object);
	Objects.remove(nullptr);
	return returnCode;
}

void AnnGameObjectManager::removeGameObject(std::shared_ptr<AnnGameObject> object)
{
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

void Annwvyn::AnnGameObjectManager::destroyLightObject(AnnLightObject * light)
{
	if (light)
		AnnGetEngine()->getSceneManager()->destroyLight(light->light);

	//Forget that this light existed
	Lights.remove(light);
	delete light;
}

AnnLightObject * Annwvyn::AnnGameObjectManager::createLightObject()
{
	AnnDebug("Creating a light");
	AnnLightObject* Light = new AnnLightObject(AnnGetEngine()->getSceneManager()->createLight());
	Light->setType(AnnLightObject::LightTypes::ANN_LIGHT_POINT);
	Lights.push_back(Light);
	return Light;
}

AnnTriggerObject * Annwvyn::AnnGameObjectManager::createTriggerObject(AnnTriggerObject * trigger)
{
	AnnDebug("Creating a trigger object");
	Triggers.push_back(trigger);
	trigger->postInit();
	return trigger;
}

void Annwvyn::AnnGameObjectManager::destroyTriggerObject(AnnTriggerObject * trigger)
{
	Triggers.remove(trigger);
	AnnDebug() << "Destroy trigger : " << (void*)trigger;
	delete trigger;
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