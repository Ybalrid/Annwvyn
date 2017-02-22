#ifndef ANN_GAME_OBJECT_MANAGER
#define ANN_GAME_OBJECT_MANAGER
#include "systemMacro.h"

#include "AnnGameObject.hpp"
#include "AnnSubsystem.hpp"
#include "AnnTypes.h"
#include "AnnTriggerObject.hpp"

#include <OgreMesh.h>
#include <OgreMesh2.h>
#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>

#include <memory>

namespace Annwvyn
{
	class AnnEngine;

	///Game object manager. Create, destroy and keep track of objects, lights and other movable stuff
	class DLL AnnGameObjectManager : public AnnSubSystem
	{
	public:
		AnnGameObjectManager();

		///Update from the game engine
		void update() override;

		///Create a game object form the name of an entity.
		/// \param entityName Name of an entity loaded to the Ogre ResourceGroupManager
		/// \param object An instance of an empty AnnGameObject. Useful for creating object of inherited class
		std::shared_ptr<AnnGameObject> createGameObject(const char entityName[], std::string identifier = "",
														std::shared_ptr<AnnGameObject> object = std::make_shared<AnnGameObject>()); //object factory

		///Remove object from the manager. Object will be destroyed when no more references are in scope
		/// \param object the object to remove
		void removeGameObject(std::shared_ptr<AnnGameObject> object);

		///Search for an AnnGameObject that holds this node, returns it if found. Return nullptr if not found.
		std::shared_ptr<AnnGameObject> getFromNode(Ogre::SceneNode * node);

		///Remove the given light from the scene
		/// \param light shared pointer to the light
		void removeLightObject(std::shared_ptr<AnnLightObject> light);

		///Add a light source to the scene. return a pointer to the new light
		std::shared_ptr<AnnLightObject> createLightObject();

		///Create a trigger object
		/// \param trigger an empty trigger object
		std::shared_ptr<AnnTriggerObject> createTriggerObject(std::shared_ptr<AnnTriggerObject> trigger = std::make_shared<AnnSphericalTriggerObject>()); //object factory

		///Remove the object from the engine
		void removeTriggerObject(std::shared_ptr<AnnTriggerObject> trigger);

		///Get the AnnGameObject the player is looking at
		std::shared_ptr<AnnGameObject> playerLookingAt(unsigned short limit = 5); //physics

		///Get an AnnGameObject for the required string; return nullptr if object cannot be found
		std::shared_ptr<AnnGameObject> getObjectFromID(std::string idString);

	private:
		friend class AnnEngine;

		///Dynamic container for triggers objects present in engine.
		std::list<std::shared_ptr<AnnTriggerObject>> Triggers;
		///Dynamic container for lights objects present in engine.
		std::list<std::shared_ptr<AnnLightObject>> Lights;
		///Dynamic container for Game objects present in engine
		std::list<std::shared_ptr<AnnGameObject>> Objects;

		///objects mapped to ID strings
		std::unordered_map<std::string, std::shared_ptr<AnnGameObject>> identifiedObjects;

		static unsigned long long id;
	};
}

#endif