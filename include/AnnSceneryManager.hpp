#ifndef	ANN_SCENERY_MANAGER
#define ANN_SCENERY_MANAGER

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include "OgreSceneManager.h"
#include "AnnTypes.h"
#include "OgreVRRender.hpp"
#include <memory>

namespace Annwvyn
{
	///Scenery Manager, scene configuration for lighting and sky.
	class DLL AnnSceneryManager : public AnnSubSystem
	{
	public:

		///Construct the AnnSceneryManager
		AnnSceneryManager(std::shared_ptr<OgreVRRender> renderer);

		///This subsystem doesn't need to be updated
		bool needUpdate() { return false; }

		///Set the ogre material for the sky-dome with params
		/// \param activate if true put a sky-dome
		/// \param materialName name of a material known from the Ogre Resource group manager
		/// \param curvature curvature of the texture
		/// \param tilling tilling of the texture
		void setSkyDomeMaterial(bool activate,
								const char materialName[],
								float curvature = 2.0f,
								float tiling = 1.0f); //scene

							///Set the ogre material for the sky-box with params
							/// \param activate if true put the sky-box on the scene
							/// \param materialName name of a material declared on the resource manager
							/// \param distance distance of the sky from the camera
							/// \param renderedFirst if true, the sky-box will be the first thing rendered
		void setSkyBoxMaterial(bool activate,
							   const char materialName[],
							   float distance = 8000,
							   bool renderedFirst = true);

						   ///Set the view-ports background color
						   /// \param v background color
		void setWorldBackgroundColor(AnnColor color = AnnColor(0, 0.56f, 1));

		///Remove the sky dome
		void removeSkyDome();

		///Remove the sky box
		void removeSkyBox();

		///Set the ambient light
		/// \param v the color of the light
		void setAmbiantLight(AnnColor color);

	private:
		///Scene manager created by the VR renderer
		Ogre::SceneManager* smgr;

		///Pointer to the VR renderer
		std::shared_ptr<OgreVRRender> renderer;
	};
}

#endif