/**
* \file AnnLightObject.hpp
* \brief Object that represent a light source
* \author A. Brainville (Ybalrid)
*/
#ifndef ANN_LIGHT_OBJECT
#define ANN_LIGHT_OBJECT

#include "systemMacro.h"
#include <OgreLight.h>
#include "AnnVect3.hpp"

namespace Annwvyn
{
	class AnnEngine;

	///Light Object : Represent a light source
	class DLL AnnLightObject 
	{
	public:
		///Set the position of the light (if relevent)
		void setPosition(AnnVect3 position);
		///Set the direction of the light (if relevent)
		void setDirection(AnnVect3 direction);
		///Set the type of the light
		void setType(Ogre::Light::LightTypes type);
	private:
		///Create a light object. We use an Ogre Light becaus we just need to talk to Ogre...
		AnnLightObject(Ogre::Light* light);
		friend class AnnEngine;
		Ogre::Light* light;
	};
}

#endif