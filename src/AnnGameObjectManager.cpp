// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>

#include "AnnGameObjectManager.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;

AnnGameObjectManager::AnnGameObjectManager() :
 AnnSubSystem("GameObjectManager"), halfPos(true), halfTexCoord(true), qTan(true)
{
	//There will only be one manager, set the id to 0
	autoID = 0;

	if(const auto plugin = Ogre_glTF::gltfPluginAccessor::findPlugin(); plugin)
		glTFLoader = plugin->getLoader();
	else
		AnnDebug(Log::Important) << "Could not get glTFLoader!, please check if the plugin is located next to the executable!";
}

void AnnGameObjectManager::update()
{
	//Run animations and update OpenAL sources position
	for(auto gameObject : Objects)
	{
		gameObject->addAnimationTime(AnnGetEngine()->getFrameTime());
		gameObject->updateOpenAlPos();
		gameObject->update();
		gameObject->callUpdateOnScripts();
	}
}

Ogre::MeshPtr AnnGameObjectManager::getAndConvertFromV1Mesh(const char* meshName, Ogre::v1::MeshPtr& v1Mesh, Ogre::MeshPtr& v2Mesh) const
{
	static const std::string sufix = "_V2mesh";
	const auto meshManager		   = Ogre::MeshManager::getSingletonPtr();

	v1Mesh = Ogre::v1::MeshManager::getSingleton().load(meshName,
														Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
														Ogre::v1::HardwareBuffer::HBU_STATIC,
														Ogre::v1::HardwareBuffer::HBU_STATIC);

	//Generate the name of the v2 mesh
	const auto v2meshName = meshName + sufix;
	AnnDebug() << "Mesh v2 name : " << v2meshName;

	//v2Mesh
	v2Mesh = meshManager->getByName(v2meshName);
	if(!v2Mesh) //create and import
	{
		AnnDebug() << v2meshName << " doesn't exist yet in the v2 MeshManager, creating it and loading the v1 " << meshName << " geometry";
		v2Mesh = meshManager->createManual(v2meshName, AnnResourceManager::getDefaultResourceGroupName());
		v2Mesh->importV1(v1Mesh.get(), halfPos, halfTexCoord, qTan);
	}

	return v2Mesh;
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::createGameObject(const std::string& meshName, std::string identifier, std::shared_ptr<AnnGameObject> obj)
{
	AnnDebug("Creating a game object from the mesh file: " + std::string(meshName));
	auto smgr { AnnGetEngine()->getSceneManager() };

	Ogre::Item* item = nullptr;

	//Check filename extension:
	auto ext = meshName.substr(meshName.find_last_of('.') + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), [](char c) { return char(::tolower(int(c))); });

	if(ext == "mesh")
	{
		Ogre::v1::MeshPtr v1Mesh;
		Ogre::MeshPtr v2Mesh;
		(void)getAndConvertFromV1Mesh(meshName.c_str(), v1Mesh, v2Mesh);
		v1Mesh.setNull();

		//Create an item
		item = smgr->createItem(v2Mesh);
	}
	else if(ext == "glb")
	{
		item = glTFLoader->getModelData(meshName, Ogre_glTF::glTFLoader::LoadFrom::ResourceManager).makeItem(smgr);
	}

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
	if(identifier.empty())
		identifier = meshName + std::to_string(nextID());

	AnnDebug() << "The object " << identifier << " has been created. Annwvyn memory address " << obj;
	AnnDebug() << "This object take " << sizeof *obj.get() << " bytes";

	obj->name					  = identifier;
	identifiedObjects[identifier] = obj;
	Objects.push_back(obj);

	obj->postInit();
	return obj;
}

void AnnGameObjectManager::removeGameObject(std::shared_ptr<AnnGameObject> object)
{
	AnnDebug() << "Removed object " << object->getName();

	if(!object) throw AnnNullGameObjectError();

	Objects.erase(
		std::remove(std::begin(Objects), std::end(Objects), object),
		std::end(Objects));

	identifiedObjects.erase(object->getName());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getFromNode(Ogre::SceneNode* node)
{
	AnnDebug() << "Trying to identify object at address " << static_cast<void*>(node);

	const auto result = std::find_if(begin(Objects), end(Objects), [&](std::shared_ptr<AnnGameObject> object) { return object->getNode() == node; });
	if(result != end(Objects)) return *result;

	AnnDebug() << "The Scene Node" << static_cast<void*>(node) << " doesn't belong to any AnnGameObject";
	return nullptr;
}

void AnnGameObjectManager::removeLightObject(std::shared_ptr<AnnLightObject> light)
{
	if(!light) throw AnnNullGameObjectError();
	Lights.erase(
		std::remove(std::begin(Lights), std::end(Lights), light),
		std::end(Lights));

	identifiedLights.erase(light->getName());
}

std::shared_ptr<AnnLightObject> AnnGameObjectManager::createLightObject(std::string lightObjectName)
{
	AnnDebug("Creating a light");
	if(lightObjectName.empty()) lightObjectName = "light" + std::to_string(nextID());
	auto Light = std::make_shared<AnnLightObject>(AnnGetEngine()->getSceneManager()->createLight(), lightObjectName);
	Light->setType(AnnLightObject::LightTypes::ANN_LIGHT_POINT);
	Lights.push_back(Light);
	identifiedLights[lightObjectName] = Light;
	return Light;
}

std::shared_ptr<AnnTriggerObject> AnnGameObjectManager::createTriggerObject(std::string triggerObjectName)
{
	AnnDebug("Creating a trigger object");
	if(triggerObjectName.empty()) triggerObjectName = "trigger" + std::to_string(nextID());
	auto trigger = std::make_shared<AnnTriggerObject>(triggerObjectName);
	Triggers.push_back(trigger);
	identifiedTriggerObjects[triggerObjectName] = trigger;
	return trigger;
}

void AnnGameObjectManager::removeTriggerObject(std::shared_ptr<AnnTriggerObject> trigger)
{
	Triggers.erase(
		std::remove(std::begin(Triggers), std::end(Triggers), trigger),
		std::end(Triggers));

	identifiedTriggerObjects.erase(trigger->getName());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::playerLookingAt(unsigned short limit)
{
	//Origin vector of the ray is the HMD pose position
	const auto pose = AnnGetVRRenderer()->trackedHeadPose;
	const auto hmdPosition { AnnVect3(pose.position) };
	const auto rayDirection { AnnQuaternion(pose.orientation).getAtVector() };

	//create ray
	const Ogre::Ray ray(hmdPosition, rayDirection);

	//create query
	auto raySceneQuery(AnnGetEngine()->getSceneManager()->createRayQuery(ray));
	//Sort by distance. Nearest is first. Limit to `limit` results.
	raySceneQuery->setSortByDistance(true, limit);

	//execute and get the results
	auto& results(raySceneQuery->execute());

	//read the result list
	const auto result = std::find_if(begin(results), end(results), [](const Ogre::RaySceneQueryResultEntry& entry) {
		return entry.movable && entry.movable->getMovableType() == "Item";
	});

	//If can't find it? return nullptr
	if(result == end(results))
		return nullptr; //means that we don't know what the player is looking at.

	//We got access to the node, we want the object
	return getFromNode(result->movable->getParentSceneNode());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getGameObject(std::string gameObjectName)
{
	const auto object = identifiedObjects.find(gameObjectName);
	if(object != end(identifiedObjects))
		return object->second;
	return nullptr;
}

std::shared_ptr<AnnLightObject> AnnGameObjectManager::getLightObject(std::string lightObjectName)
{
	const auto light = identifiedLights.find(lightObjectName);
	if(light != end(identifiedLights))
		return light->second;
	return nullptr;
}

std::shared_ptr<AnnTriggerObject> Annwvyn::AnnGameObjectManager::getTriggerObject(std::string triggerObjectName)
{
	const auto trigger = identifiedTriggerObjects.find(triggerObjectName);
	if(trigger != end(identifiedTriggerObjects))
		return trigger->second;
	return nullptr;
}

void AnnGameObjectManager::setImportParameter(bool halfPosition, bool halfTextureCoord, bool qTangents)
{
	halfPos		 = halfPosition;
	halfTexCoord = halfTextureCoord;
	qTan		 = qTangents;
}

uID AnnGameObjectManager::nextID()
{
	return ++autoID;
}
