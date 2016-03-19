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
		AnnLightObject();
		AnnLightObject(Ogre::Light* light);
		void setPosition(AnnVect3 position);
		void setDirection(AnnVect3 direction);
		void setType(Ogre::Light::LightTypes type);
	private:
		friend class AnnEngine;
		Ogre::Light* light;
	};
}

#endif