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
	class DLL AnnSplashLevel : LEVEL
	{
	public:
		AnnSplashLevel(Ogre::String splashTexture);
		void load();
		void runLogic();
		void unload();

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
	};

}
#endif