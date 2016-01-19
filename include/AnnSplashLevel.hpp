/**
* \file AnnSplashLever.hpp
* \brief Scene (level) to put a splashing logo
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_SPLASH
#define ANN_SPLASH

#include "systemMacro.h"
#include "AnnAbstractLevel.hpp"

namespace Annwvyn
{
	class DLL AnnSplashLevel : public AnnAbstractLevel
	{
	public:
		///Construct a SplashLevel.
		///\param splashTexture Name of the resource (image) to put in front of the player
		AnnSplashLevel(Ogre::String splashTexture, AnnAbstractLevel* nextLevel = nullptr, float timeoutTime = 10);
		void load();
		void runLogic();
		void unload();
		void setBGM(std::string path, bool preload = true);

		///Set the level to jump to after timeout
		void setNextLevel(AnnAbstractLevel* level);
		///Set timeout time in seconds
		void setTimeout(float time);
	private:
		float timeout, currentTime, startTime;
		AnnAbstractLevel* next;
		Ogre::ManualObject* CurvedPlane;
		Ogre::SceneNode* Splash;
		Ogre::String splashImage;
		bool hasBGM;
		std::string bgmPath;
	};

}
#endif