#ifndef ANN_ABSTRACTLEVEL
#define ANN_ABSTRACTLEVEL

#include "AnnTypes.h"

#define LEVEL public Annwvyn::AnnAbstractLevel
#define constructLevel() AnnAbstractLevel()

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
	};
}
#endif