#pragma once
#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include "OgreSceneManager.h"
#include "AnnTypes.h"
#include "OgreOculusRender.hpp"

namespace Annwvyn
{
	class DLL AnnSceneryManager : public AnnSubSystem
	{
	public:
		AnnSceneryManager(OgreOculusRender* renderer);
		bool needUpdate() { return false; }


		///Set the ogre material for the skydome with params
		/// \param activate if true put a skydome
		/// \param materialName name of a material known from the Ogre Resource group manager
		/// \param curvature curvature of the texture
		/// \param tilling tilling of the texture
		void setSkyDomeMaterial(bool activate,
			const char materialName[],
			float curvature = 2.0f,
			float tiling = 1.0f); //scene

		///Set the ogre material for the skybox with params
		/// \param activate if true put the skybox on the scene
		/// \param materialName name of a material declared on the resource manager
		/// \param distance distance of the sky from the camera
		/// \param renderedFirst if true, the skybox will be the first thing rendered
		void setSkyBoxMaterial(bool activate,
			const char materialName[],
			float distance = 8000,
			bool renderedFirst = true);

		///Set the viewports background color
		/// \param v background color
		void setWorldBackgroundColor(AnnColor color = AnnColor(0, 0.56, 1));

		///Remove the sky dome
		void removeSkyDome();

		///Remove the sky box
		void removeSkyBox();

		///Set the ambiant light
		/// \param v the color of the light
		void setAmbiantLight(AnnColor color);



	private:
		Ogre::SceneManager* smgr;
		OgreOculusRender* renderer;
	};
}