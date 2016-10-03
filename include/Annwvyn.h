#ifndef ANNWVYN
#define ANNWVYN

/*!
* \file Annwvyn.h
* \brief Main Annwvyn include file (to be used by client application)
* \author Arthur Brainville
* \version 0.2.0
*/

/*!
 *
 *\mainpage Annwvyn Game Engine API Reference Documentation
 *
 * Welcome to the Annwvyn API documentation.
 * 
 * Annwvyn is a free and Open-Soucre Virtual reality developement framework.
 *
 * <em>The project is under cosntant developement and there isn't yet any stable
 * release available. It comes without any warenty and is distributed under the
 * terms of the MIT Licence agreement. Full copy of the licence is available on the 
 * source code repository</em>
 *
 * <em>Also, please note that, because of the on-going developement nature of this 
 * project, any part of the presented API may change without any notice.</em>
 *
 * Here you'll find the complete & exhaustive documentation of the engine 
 * (last version avilable in the master branch at the date this site has been 
 * generated).
 *
 * I strongly recomned you to go to <a href="http://wiki.annwvyn.org/">the wiki</a>
 * to get a more comprehensible help to start using the engine, 
 * there is a quick-start tutorial you can check-out and more contend will be added 
 * regulary
 *
 * The engine is based on Ogre and take for reference Ogre's resource management, 
 * file format, datastructures and geometric classes. Although, the user of the engine
 * is not intended to have to know Ogre, some part of the public API still rely directly 
 * On Ogre componants, notably the Resource management and the materials.<br>
 * Everything 3D related on Annwvyn is handeled by Ogre and is only intended to be used 
 * with the GL render system, as it will force the call of RenderSystem_GL classes, instead
 * of the parents.
 *
 * The aim of the project is to making an high-level (yet really fast) framework 
 * to create Oculus Rift compatible content. 
 * You dont't have to worry about doing the rendering, 
 * the tracking, simulating basic physics, ect.
 * 
 * If you have any suggestion, feel free to email me at 
 * <a href="mailto:developer@annwvyn.org">developer@annwvyn.org</a>.
 *
 * Enjoy your stay and make great virtual reality experiencies ;-)
 *
 * N.B: This Doxygen site is intended to be hosted at <a href="http://api.annwvyn.org">api.annwvyn.org</a>. *
 * If it's the case, you have to know that it's based on the master branch of a local mirror of the repository. 
 * Modification of the engine can take up to 5 hours to apear on this website. While the engine is in "experimental" phase, 
 * This site doesn't mirror the content of the last release of the engine, but the current state of the master branch code. 
 *
 */

//Some C++ misc utils
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <unordered_map>
#include <cstring>
//Annwvyn classes
#include "AnnEngine.hpp"
#include "AnnGameObjectManager.hpp"
#include "AnnGameObject.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnAudioEngine.hpp"
#include "AnnEventManager.hpp"
#include "AnnLogger.hpp"
#include "AnnLevel.hpp"
#include "AnnXmlLevel.hpp"
#include "AnnSplashLevel.hpp"
#include "AnnFilesystem.hpp"
#include "AnnResourceManager.hpp"
#include "Ann3DTextPlane.hpp"
#include "AnnUserSpaceSubSystem.hpp"

//Other Annwvyn
#include "AnnTypes.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

///Namespace containing the totality of Annwvyn components
namespace Annwvyn
{
	inline std::string getHMDFromCmdLine(const char* cmd)
	{
		//If no args, default
		if (strlen(cmd) == 0)
			return "OgreDefaultRender";

		//Args should start by a '-'
		const char argChar = '-';
		if (cmd[0] != argChar)
			return "arg_error";

		//Convert to C++ string object
		std::string strCmd{ cmd };

		//User want to use a rift
		if (strCmd == "-rift")
			return "OgreOculusRender";

		//User want to use a Vive or another OpenVR headset
		if (strCmd == "-vive" || strCmd == "-openVR")
			return "OgreOpenVRRender";

		//Not implemented. I don't have OSVR hardware to try it out.
		if (strCmd == "-osvr")
			return "OgreOSVRRender";

		//Bodge to run game on a flatscreen on hardware that is not compatible with x
		if (strCmd == "-noVR")
			return "OgreNoVRRender";

		return "arg_error";
	}

	inline void preStart()
	{

	}
}



///Annwvyn initialization macro 
#define AnnInit(AppName) new AnnEngine(AppName, detectedHMD)
#define AnnQuit() delete AnnGetEngine()

//===================Application Entrypoint definition=================//
/*Main definition :
 *
 *	For more simplicity, Program start by a "AnnMain" function at the library user
 *	side. This allow to select proper entry proint for the aplication, and maybe
 *	add pre-starting treatements here.
 */


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 
#include "windows.h"

///Application entry point
#define AnnMain() int AnnwvynStart();\
std::string detectedHMD;\
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT) \
{\
	detectedHMD = Annwvyn::getHMDFromCmdLine(static_cast<const char*>(strCmdLine));\
	Annwvyn::preStart();\
	return AnnwvynStart();\
}\
int AnnwvynStart()

#else
///Application entry point
#define AnnMain() int AnnwvynStart();\
std::string detectedHMD;\
int main(int argc, char** argv)\
{\
	if(argc < 2) detectedHMD = Annwvyn::getHMDFromCmdLine("");\
	else detectedHMD = Annwvyn::getHMDFromCmdLine(argv[1]);\
	Annwvyn::presart();\
	return AnnwvynStart();\
}\
int AnnwvynStart()
#endif
//=======================================================================//

#endif //ANNWVYN	

#ifndef ANN_NAMESPACE
#define ANN_NAMESPACE
#endif
