#ifndef ANN_LIGHT_OBJECT
#define ANN_LIGHT_OBJECT

#include "systemMacro.h"
#include <OgreLight.h>
#include "AnnVect3.hpp"

namespace Annwvyn
{
	class AnnEngine;
	class DLL AnnLightObject 
	{
	public:
		void setPosition(AnnVect3 position);
		void setDirection(AnnVect3 direction);
		void setType(Ogre::Light::LightTypes type);
	private:
		AnnLightObject(Ogre::Light* light);
		friend class AnnEngine;
		Ogre::Light* light;
	};
}

#endif