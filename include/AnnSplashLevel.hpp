/**
* \file AnnSplashLevel.hpp
* \brief Scene (level) to put a splashing logo
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_SPLASH
#define ANN_SPLASH

#include "systemMacro.h"
#include "AnnLevel.hpp"

namespace Annwvyn
{
	///"Level" that display an image on a giant plane, then jump to another level after a timeout
	class DLL AnnSplashLevel : public AnnLevel
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
		void setBGM(std::string path, bool preload = true);

		///Set the level to jump to after timeout
		void setNextLevel(std::shared_ptr<AnnLevel> level);

		///Set timeout time in seconds
		void setTimeout(float time);

		///Set timeout time in milliseconds
		void setTimeoutMillisec(unsigned time);

	private:
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
		std::string bgmPath;
	};
}
#endif