/**
 * \file AnnAbstractLevel.hpp
 * \brief Pure virtual class that represent a level
 *        AnnLevelManager class only handle object that derive from this class
		  You need to overload at least the load() and runLogic() methods to create a level.
		  To simplify level declaration, you can use the keyword "LEVEL" to mark inheritance and
		  constructLevel() to call the constructor of the abstract class.
		  This class also provide a 'simple, stupid' random string generator to set IDs to object automaticaly
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANN_ABSTRACTLEVEL
#define ANN_ABSTRACTLEVEL

#include "AnnTypes.h"
#include "AnnTriggerObject.hpp"
#define LEVEL public Annwvyn::AnnAbstractLevel
#define constructLevel() AnnAbstractLevel()
#define noID "noID"
#define defaultIdLen 15

namespace Annwvyn
{
	//AnnEngine should really be a singleton class with a static method to get the instance... 
	class DLL AnnAbstractLevel
	{
	public:
		///Construct the level 
		AnnAbstractLevel();
		
		///Pure virtual methods that loads the level
		virtual void load() = 0;

		///Destroy the level
		virtual ~AnnAbstractLevel();
		
		///Unload the level by destroying every objects in "levelContent" and every lights in "levelLighting"
		virtual void unload();
			
		///Run logic code from the level
		virtual void runLogic() =0;

	protected:
		AnnGameObjectVect levelContent;
		AnnLightVect levelLighting;
		AnnTriggerObjectVect levelTrigger;

		AnnLightObject* addLight(std::string id = noID);
		AnnTriggerObject* addTrggerObject(AnnTriggerObject* obj = new AnnSphericalTriggerObject, std::string id = noID);
		AnnGameObject* addGameObject(std::string entityName, std::string id = noID);

		std::string name;

		std::string generateRandomID(size_t len = defaultIdLen);
	};
}
#endif