#pragma once
#include "systemMacro.h"

#include "AnnGameObject.hpp"
#include "AnnSubsystem.hpp"
#include "AnnTypes.h"

namespace Annwvyn
{	
	class AnnEngine;
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

	private:
		friend class AnnEngine;
		AnnGameObjectList Objects;
	};
}