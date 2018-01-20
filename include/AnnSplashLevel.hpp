/**
* \file AnnSplashLevel.hpp
* \brief Scene (level) to put a splashing logo
* \author A. Brainville (Ybalrid)
*/

#pragma once

#include "systemMacro.h"
#include "AnnLevel.hpp"

namespace Annwvyn
{
	///"Level" that display an image on a giant plane, then jump to another level after a timeout
	class AnnDllExport AnnSplashLevel : public AnnLevel
	{
	public:
		///Construct a SplashLevel.
		///\param splashTexture Name of the resource (image) to put in front of the player
		AnnSplashLevel(Ogre::String splashTexture, std::shared_ptr<AnnLevel> nextLevel = nullptr, float timeoutTime = 10);

		///Load the level. This create a manual material and a manual object to present the splash image
		void load() override;

		///Run the logic of the splash screen. This will check for the time and jump to next level if timeout
		void runLogic() override;

		///Clean up the manually allocated object
		void unload() override;

		///Set the background music
		void setBGM(std::string name, bool preload = true);

		///Set the level to jump to after timeout
		void setNextLevel(std::shared_ptr<AnnLevel> level);

		///Set timeout time in seconds
		void setTimeout(float time);

		///Set timeout time in milliseconds
		void setTimeoutMillisec(unsigned time);

	private:
		///Create the plane where the splash will go
		void createSplashCurvedPlane();

		///Create and return the list of points to describe the wanted "curved" plane. Pivot is plane center
		/// \param curvature The curve is parabolic. The equation that define each Z point is [Z = - (1/curvature) * X^2] (higher values makes plane flatter)
		/// \param width the Width of the plane
		/// \param height the Height of the plane
		/// \param definition The number of points used to describe the plane in the X axis. Higher values makes smoother planes.
		std::vector<AnnVect3> createCurvedPlaneVertices(float curvature, float width, float height, float definition);

		///Create the hlms unlit datablock.
		/// \param unlit pointer to the HlmsUnlit object
		Ogre::HlmsUnlitDatablock* createSplashDatablock(Ogre::HlmsUnlit* unlit);

		///Time values
		float timeout, currentTime, startTime;

		///Pointer to the next level to load
		std::weak_ptr<AnnLevel> next;

		///The plane that present the object
		Ogre::ManualObject* CurvedPlane;

		///Node where the splash screen is attached
		Ogre::SceneNode* Splash;

		///Name of the image of the splash-screen
		Ogre::String splashImageName;

		///True if music is set
		bool hasBGM;

		///Path to the file that is loaded as an audio buffer
		std::string bgmName;
	};
}
