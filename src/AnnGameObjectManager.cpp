#include "stdafx.h"
#include "AnnGameObjectManager.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;
unsigned long long AnnGameObjectManager::autoID;

AnnGameObjectManager::AnnGameObjectManager() : AnnSubSystem("GameObjectManager"), halfPos(true), halfTexCoord(true), qTan(true)
{
	//There will only be one manager, set the id to 0
	autoID = 0;
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

Ogre::MeshPtr AnnGameObjectManager::getMesh(const char* meshName, Ogre::v1::MeshPtr& v1Mesh, Ogre::MeshPtr& v2Mesh) const
{
	v1Mesh = Ogre::v1::MeshManager::getSingleton().load(meshName,
		Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
		Ogre::v1::HardwareBuffer::HBU_STATIC,
		Ogre::v1::HardwareBuffer::HBU_STATIC);

	static const std::string sufix = "_V2mesh";

	//Generate the name of the v2 mesh
	auto v2meshName = meshName + sufix;
	AnnDebug() << "Mesh v2 name : " << v2meshName;

	//v2Mesh
	v2Mesh = Ogre::MeshManager::getSingleton().getByName(v2meshName);
	if (!v2Mesh) //create and import
	{
		AnnDebug() << v2meshName << " doesn't exist yet in the v2 MeshManager, creating it and loading the v1 " << meshName << " geometry";
		v2Mesh = Ogre::MeshManager::getSingleton().createManual(v2meshName, AnnResourceManager::defaultResourceGroupName);
		v2Mesh->importV1(v1Mesh.get(), halfPos, halfTexCoord, qTan);
	}

	return v2Mesh;
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::createGameObject(const char meshName[], std::string identifier, std::shared_ptr<AnnGameObject> obj)
{
	AnnDebug("Creating a game object from the mesh file :  " + std::string(meshName));
	auto smgr{ AnnGetEngine()->getSceneManager() };

	Ogre::v1::MeshPtr v1Mesh;
	Ogre::MeshPtr v2Mesh;
	getMesh(meshName, v1Mesh, v2Mesh);
	v1Mesh.setNull();

	//Create an item
	auto item = smgr->createItem(v2Mesh);

	//Create a node
	auto node = smgr->getRootSceneNode()->createChildSceneNode();

	//Attach
	node->attachObject(item);

	//Set GameObject members
	obj->setNode(node);
	obj->setItem(item);
	obj->audioSource = AnnGetAudioEngine()->createSource();

	//id will be unique to every non-identified object.
	//The identifier name can be empty, meaning that we have to figure out an unique name.
	//In that case we will append to the entity name + a number that will always be incremented.
	if (identifier.empty())
		identifier = meshName + std::to_string(++autoID);

	AnnDebug() << "The object " << identifier << " has been created. Annwvyn memory address " << obj;
	AnnDebug() << "This object take " << sizeof *obj.get() << " bytes";

	obj->name = identifier;
	identifiedObjects[identifier] = obj;
	Objects.push_back(obj);

	obj->postInit();
	return obj;
}

void AnnGameObjectManager::removeGameObject(std::shared_ptr<AnnGameObject> object)
{
	AnnDebug() << "Removed object " << object->getName();

	if (!object) throw AnnNullGameObjectError();

	Objects.remove(object);
	identifiedObjects.erase(object->getName());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getFromNode(Ogre::SceneNode* node)
{
	AnnDebug() << "Trying to identify object at address " << static_cast<void*>(node);

	const auto result = std::find_if(begin(Objects), end(Objects),
		[&](std::shared_ptr<AnnGameObject> object) {return object->getNode() == node; });
	if (result != end(Objects)) return *result;

	AnnDebug() << "The Scene Node" << static_cast<void*>(node) << " doesn't belong to any AnnGameObject";
	return nullptr;
}

void AnnGameObjectManager::removeLightObject(std::shared_ptr<AnnLightObject> light)
{
	if (!light) throw AnnNullGameObjectError();
	Lights.remove(light);
	identifiedLights.erase(light->getName());
}

std::shared_ptr<AnnLightObject> AnnGameObjectManager::createLightObject(std::string identifier)
{
	AnnDebug("Creating a light");
	if (identifier.empty()) identifier = "light" + std::to_string(++autoID);
	auto Light = std::make_shared<AnnLightObject>(AnnGetEngine()->getSceneManager()->createLight(), identifier);
	Light->setType(AnnLightObject::LightTypes::ANN_LIGHT_POINT);
	Lights.push_back(Light);
	identifiedLights[identifier] = Light;
	return Light;
}

std::shared_ptr<AnnTriggerObject> AnnGameObjectManager::createTriggerObject(std::string identifier)
{
	AnnDebug("Creating a trigger object");
	auto trigger = std::make_shared <AnnTriggerObject>();
	if (identifier.empty()) identifier = "trigger" + std::to_string(++autoID);
	Triggers.push_back(trigger);
	return trigger;
}

void AnnGameObjectManager::removeTriggerObject(std::shared_ptr<AnnTriggerObject> trigger)
{
	Triggers.remove(trigger);
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::playerLookingAt(unsigned short limit)
{
	//Origin vector of the ray is the HMD pose position
	auto hmdPosition{ AnnVect3(AnnGetEngine()->getHmdPose().position) };

	//Calculate direction Vector of the ray to be the midpoint camera optical axis
	auto rayDirection{ AnnQuaternion(AnnGetEngine()->getHmdPose().orientation).getAtVector() };

	//create ray
	Ogre::Ray ray(hmdPosition, rayDirection);

	//create query
	auto raySceneQuery(AnnGetEngine()->getSceneManager()->createRayQuery(ray));
	//Sort by distance. Nearest is first. Limit to `limit` results.
	raySceneQuery->setSortByDistance(true, limit);

	//execute and get the results
	auto& results(raySceneQuery->execute());

	//read the result list
	auto result = std::find_if(results.begin(), results.end(), [](const Ogre::RaySceneQueryResultEntry& entry)
	{
		if (entry.movable && entry.movable->getMovableType() == "Item") return true;
		return false;
	});

	//If can't find it? return nullptr
	if (result == results.end())
		return nullptr; //means that we don't know what the player is looking at.

	//We got access to the node, we want the object
	return getFromNode(result->movable->getParentSceneNode());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getObjectFromID(std::string idString)
{
	auto object = identifiedObjects.find(idString);
	if (object != identifiedObjects.end())
		return object->second;
	return nullptr;
}

void AnnGameObjectManager::setImportParameter(bool halfPosition, bool halfTextureCoord, bool qTangents)
{
	halfPos = halfPosition;
	halfTexCoord = halfTextureCoord;
	qTan = qTangents;
}