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
	if (Objects.size() > 0) 
		for (auto object : Objects) 
			destroyGameObject(object);
	else 
		AnnDebug("Object list allready clean");
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

AnnGameObject * AnnGameObjectManager::createGameObject(const char entityName[], AnnGameObject * obj)
{
	AnnDebug("Creatig a game object from the entity " + std::string(entityName));

	if (std::string(entityName).empty())
	{
		AnnDebug("Hey! what are you trying to do here? Please specify a non empty string for entityName !");
		delete obj;
		return NULL;
	}

	Ogre::Entity* ent = AnnGetEngine()->getSceneManager()->createEntity(entityName);
	Ogre::SceneNode* node = AnnGetEngine()->getSceneManager()->getRootSceneNode()->createChildSceneNode();

	node->attachObject(ent);
	obj->setNode(node);
	obj->setEntity(ent);
	obj->audioSource = AnnGetAudioEngine()->createSource();
	obj->setBulletDynamicsWorld(AnnGetPhysicsEngine()->getWorld());
	obj->postInit(); //Run post init directives

	Objects.push_back(obj); //keep addreAnnDebug() in list

	AnnDebug() << "The object " << entityName << " has been created. Annwvyn memory address " << obj;

	return obj;
}
bool AnnGameObjectManager::destroyGameObject(AnnGameObject * object)
{
	Ogre::SceneManager* SceneManager(AnnGetEngine()->getSceneManager());
	AnnDebug() << "Destroy call : " << object;
	if (!object) return false;
	bool returnCode(false);
	//TODO: remove the need to mark Objects as NULL in this array before being able to clear them.
	for (auto it = Objects.begin(); it != Objects.end(); it++)
	{
		if (!*it) continue;
		(*it)->stopGettingCollisionWith(object);
		if (*it == object)
		{
			returnCode = true;
			*it = NULL;

			Ogre::SceneNode* node = object->getNode();
			node->getParent()->removeChild(node);
			size_t nbObject(node->numAttachedObjects());
			std::vector<Ogre::MovableObject*> attachedObject;

			for (unsigned short i(0); i < nbObject; i++)
				attachedObject.push_back(node->getAttachedObject(i));

			node->detachAllObjects();

			auto attachedIterator(attachedObject.begin());
			while (attachedIterator != attachedObject.end())
				SceneManager->destroyMovableObject(*attachedIterator++);
			if (AnnGetPhysicsEngine()) AnnGetPhysicsEngine()->removeRigidBody(object->getBody());
			SceneManager->destroySceneNode(node);
			delete object;
		}
	}

	//Clear everything equals to "NULL" on the vector
	Objects.remove(NULL);

	return returnCode;
}

