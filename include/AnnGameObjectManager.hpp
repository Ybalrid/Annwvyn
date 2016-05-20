#ifndef ANN_GAME_OBJECT_MANAGER
#define ANN_GAME_OBJECT_MANAGER
#include "systemMacro.h"

#include "AnnGameObject.hpp"
#include "AnnSubsystem.hpp"
#include "AnnTypes.h"
#include "AnnTriggerObject.hpp"
namespace Annwvyn
{	
	class AnnEngine;

	///Game object manager. Create, destroy and keep track of objects, lights and other movable stuff
	class DLL AnnGameObjectManager : public AnnSubSystem
	{
	public:
		AnnGameObjectManager();
		~AnnGameObjectManager();
		void update();

		///Create a game object form the name of an entity.
		/// \param entityName Name of an entity loaded to the Ogre ResourceGroupManager
		/// \param object An instance of an empty AnnGameObject. Usefull for creating object of herited class
		AnnGameObject* createGameObject(const char entityName[], AnnGameObject* object = new AnnGameObject()); //object factory

		///Destroy the given object
		/// \param object the object to be destroyed
		bool destroyGameObject(AnnGameObject* object); //object factory

		Annwvyn::AnnGameObject * getFromNode(Ogre::SceneNode * node);

		///Destroy the given light
		/// \param light pointer to the light to destroy
		void destroyLightObject(AnnLightObject* light);

		///Add a light source to the scene. return a pointer to the new light
		AnnLightObject* createLightObject();

		///Create a trigger object
		/// \param trigger an empty trigger object
		AnnTriggerObject* createTriggerObject(AnnTriggerObject* trigger = new AnnSphericalTriggerObject); //object factory

		///Remove the object from the engine
		void destroyTriggerObject(AnnTriggerObject* obj);

		///Get the AnnObject the player is looking at
		AnnGameObject* playerLookingAt(); //physics


	private:
		friend class AnnEngine;
		AnnGameObjectList Objects;

		///Dynamic container for triggers objects present in engine.
		std::list<AnnTriggerObject*> Triggers;
		///Dynamic container for lights objects present in engine.
		std::list<AnnLightObject*> Lights;

	};
}

#endif