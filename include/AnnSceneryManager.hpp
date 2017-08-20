#ifndef	ANN_SCENERY_MANAGER
#define ANN_SCENERY_MANAGER

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include "OgreSceneManager.h"
#include "AnnTypes.h"
#include "AnnOgreVRRenderer.hpp"
#include <memory>
#include <algorithm>

namespace Annwvyn
{
	///Scenery Manager. Set the scene rendering parameters, like the exposure or the ambient lighting...
	class DLL AnnSceneryManager : public AnnSubSystem
	{
	public:

		///Construct the AnnSceneryManager
		AnnSceneryManager(std::shared_ptr<AnnOgreVRRenderer> renderer);

		///This subsystem doesn't need to be updated
		bool needUpdate() override { return false; }

		///Set the ogre material for the sky-dome with params
		/// \param activate if true put a sky-dome
		/// \param materialName name of a material known from the Ogre Resource group manager
		/// \param curvature curvature of the texture
		/// \param tiling tilling of the texture
		void setSkyDomeMaterial(bool activate,
			const std::string& materialName,
			float curvature = 2.0f,
			float tiling = 1.0f) const;

		///Set the ogre material for the sky-box with params
		/// \param activate if true put the sky-box on the scene
		/// \param materialName name of a material declared on the resource manager
		/// \param distance distance of the sky from the camera
		/// \param renderedFirst if true, the sky-box will be the first thing rendered
		void setSkyBoxMaterial(bool activate,
			const std::string& materialName,
			float distance = 8000,
			bool renderedFirst = true) const;

		///Remove the sky dome
		void removeSkyDome() const;

		///Remove the sky box
		void removeSkyBox() const;

		///Set the ambient light. Ambiant light is 2 hemisphere, and upper one and a lower one. HDR light values are color * multiplier
		void setAmbientLight(AnnColor upperColor, float upperMul, AnnColor lowerColor, float lowerMul, AnnVect3 direction, float environementMapScaling = 16) const;

		///Set the defautl ambient light
		void setDefaultAmbientLight() const;

		///Set the exposure value in "ev" (what's used in photography for exposure compensation). Also set the delta of adjustmen
		void setExposure(float exposure, float minExposure, float maxExposure) const;

		///Reset the default exposure
		void setDefaultExposure() const;

		///Set the color of the sky. Converted into HDR light values with the multiplier
		void setSkyColor(AnnColor color, float multiplier) const;

		///Reset the engine default sky color
		void setDefaultSkyColor() const;

		///Set the threshold that define where to put the bloom effect
		void setBloomThreshold(float threshold) const;

		///Reset the default bloom effect
		void setDefaultBloomThreshold() const;

		///Reset everything to engine default
		void resetSceneParameters() const;

	private:
		///Scene manager created by the VR renderer
		Ogre::SceneManager* smgr;

		///Pointer to the VR renderer
		std::shared_ptr<AnnOgreVRRenderer> renderer;

		///Defaults environmental floats
		const float defaultExposure, defaultMinAutoExposure, defaultMaxAutoExposure, defaultSkyColorMultiplier, defaultBloom, defaultUpperAmbientLightMul, defaultLowerAmbientLightMul;
		///Default sky color
		const AnnColor defaultSkyColor, defaultUpperAmbient, defaultLowerAmbient;
	};
}

#endif