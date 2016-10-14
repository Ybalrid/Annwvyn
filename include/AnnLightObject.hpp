/**
* \file AnnLightObject.hpp
* \brief Object that represent a light source
* \author A. Brainville (Ybalrid)
*/
#ifndef ANN_LIGHT_OBJECT
#define ANN_LIGHT_OBJECT

#include "systemMacro.h"

#include "AnnAbstractMovable.hpp"

#include <OgreLight.h>
#include "AnnVect3.hpp"
#include "AnnColor.hpp"

namespace Annwvyn
{
	class AnnEngine;

	///Light Object : Represent a light source
	class DLL AnnLightObject : public AnnAbstractTranslatable
	{
	public:
		AnnLightObject(Ogre::Light* light);
		virtual ~AnnLightObject();

		/// Defines the type of light
		enum LightTypes
		{
			/// Point light sources give off light equally in all directions, so require only position not direction
			ANN_LIGHT_POINT = Ogre::Light::LightTypes::LT_POINT,
			/// Directional lights simulate parallel light beams from a distant source, hence have direction but no position
			ANN_LIGHT_DIRECTIONAL = Ogre::Light::LightTypes::LT_DIRECTIONAL,
			/// Spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
			ANN_LIGHT_SPOTLIGHT = Ogre::Light::LightTypes::LT_SPOTLIGHT
		};

		static LightTypes getLightTypeFromString(std::string ltype);

		///Set the position of the light (if relevant)
		void setPosition(AnnVect3 position);
		///Get the position of the light (if relevant)
		AnnVect3 getPosition();
		///Set the direction of the light (if relevant)
		void setDirection(AnnVect3 direction);
		///Set the type of the light
		void setType(LightTypes type);
		///Set the diffuse color of this light source
		void setDiffuseColor(AnnColor color);
		///Set the specular color of this light source
		void setSpecularColor(AnnColor color);
		///Get the diffuse color of this light source
		AnnColor getDiffuseColor();
		///Get the specular color of this light source
		AnnColor getSpecularColor();

	private:
		friend class AnnEngine;
		friend class AnnGameObjectManager;
		Ogre::Light* light;
	};
}

#endif