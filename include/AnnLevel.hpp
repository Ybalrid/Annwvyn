/**
* \file AnnLevel.hpp
* \brief Pure virtual class that represent a level
*        AnnLevelManager class only handle object that derive from this class
You need to overload at least the load() and runLogic() methods to create a level.
To simplify level declaration, you can use the keyword "LEVEL" to mark inheritance and
constructLevel() to call the constructor of the abstract class.
This class also provide a 'simple, stupid' random string generator to set IDs to object automatically
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_LEVEL
#define ANN_LEVEL

#include <unordered_map>

#include "AnnTypes.h"
#include "AnnTriggerObject.hpp"
#define LEVEL public Annwvyn::AnnLevel
#define constructLevel() AnnLevel()

#include "AnnAbstractMovable.hpp"

namespace Annwvyn
{
	///Base class for all Levels. Is abstract
	class AnnDllExport AnnLevel
	{
	public:
		///Construct the level
		AnnLevel();

		///Pure virtual methods that loads the level
		virtual void load() = 0;

		///Destroy the level
		virtual ~AnnLevel();

		///Unload the level by destroying every objects in "levelContent" and every lights in "levelLighting"
		virtual void unload();

		///Run logic code from the level
		virtual void runLogic() = 0;

		///Get the list of objects
		AnnGameObjectList& getContent();

		///Get the list of lights
		AnnLightList& getLights();

	protected:
		friend class AnnLevelManager;
		AnnGameObjectList levelContent;
		AnnLightList levelLighting;
		AnnTriggerObjectList levelTrigger;

		///List of movable on the level
		std::list<std::shared_ptr<AnnAbstractMovable>> levelMovable;

		///Add a light object to the level
		std::shared_ptr<AnnLightObject> addLightObject(std::string id = "");

		///Add a trigger object to the level
		std::shared_ptr<AnnTriggerObject> addTrggerObject(std::string id = "");

		///Add a Game object to the level
		std::shared_ptr<AnnGameObject> addGameObject(std::string entityName, std::string name = "");

		///Add a manual movable object
		void addManualMovableObject(std::shared_ptr<AnnAbstractMovable> movable);

		///Construct+add a manual movable object
		template<class AbstractMovableType, class ... Args> decltype(auto) addManualMovableObject(Args&& ... args)
		{
			auto movable = std::make_shared<AbstractMovableType>(args ...);
			addManualMovableObject(movable);
			return movable;
		}

		///Name of the level
		std::string name;
	};

	using AnnLevelPtr = std::shared_ptr<AnnLevel>;
}
#endif