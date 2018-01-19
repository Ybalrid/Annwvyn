#pragma once
/*!
* \file Annwvyn.h
* \brief Main Annwvyn include file (to be used by client application)
* \author Arthur Brainville
*/

/*!
 *
 *\mainpage Annwvyn Game Engine API Reference Documentation
 *
 * Welcome to the Annwvyn API documentation.
 *
 * Annwvyn is a free and Open-Source Virtual reality development framework.
 *
 * <em>Return to the <a href="https://www.annwvyn.org/">main website</a></em>
 *
 * This site document the whole API of the engine, both internal and external. As
 * it is intended to serve as a resource for the engien developement, and the engine use.
 * Anything marqued as "public" is available for games using Annwvyn, and for plugins
 * implementing a subsystem.
 *
 * A partial documentation of the scripting API is available. It's generated from
 * "fake" C++ headers.
 * If you are looking for the Scripting API, Annwvyn::ChaiScriptAPIDoc is the
 * namespace you should look for. Theses are C++ functions prototypes that match
 * the ChaiScript functions you can call, with comments.
 *
 * <em>The project is under constant development and there isn't yet any stable
 * release available. It comes without any warranty and is distributed under the
 * terms of the MIT License agreement. Full copy of the license is available on
 * the source code repository</em>
 *
 * <em>Also, please note that, because of the on-going development nature of
 * this project, any part of the presented API may change without any notice.
 * </em>
 *
 * Here you'll find the complete & exhaustive documentation of the engine
 * (last version available in the master branch at the date this site has been
 * generated).
 *
 * I strongly recommend you to go to <a href="http://wiki.annwvyn.org/">the
 * wiki</a> to get a more comprehensible help to start using the engine,
 * there is a quick-start tutorial you can check-out and more contend will be
 * added regularly
 *
 * The aim of the project is to making an high-level (yet really fast) framework
 * to create Oculus Rift compatible content.
 * You don't have to worry about doing the rendering,
 * the tracking, simulating basic physics, etc.
 *
 * If you have any suggestion, feel free to email me at
 * <a href="mailto:developer@annwvyn.org">developer@annwvyn.org</a>.
 *
 * Enjoy your stay and make great virtual reality experiences ;-)
 *
 * N.B: This Doxygen site is intended to be hosted at
 * <a href="http://api.annwvyn.org">api.annwvyn.org</a>. If it's the case, you
 * have to know that it's based on the master branch of a local mirror of the
 * repository. Modification of the engine can take up to 5 hours to appear on
 * this website. While the engine is in "experimental" phase, This site doesn't
 * mirror the content of the last release of the engine, but the current state
 * of the master branch code.
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
#include <chrono>
#include <ctime>

#include <Ogre.h>

//Annwvyn classes
#include <AnnEngine.hpp>
#include <AnnGameObjectManager.hpp>
#include <AnnGameObject.hpp>
#include <AnnTriggerObject.hpp>
#include <AnnAudioEngine.hpp>
#include <AnnEventManager.hpp>
#include <AnnLogger.hpp>
#include <AnnLevel.hpp>
#include <AnnXmlLevel.hpp>
#include <AnnSplashLevel.hpp>
#include <AnnFilesystem.hpp>
#include <AnnResourceManager.hpp>
#include <Ann3DTextPlane.hpp>
#include <AnnUserSpaceSubSystem.hpp>
#include <AnnUserSpaceEvent.hpp>
#include <AnnException.hpp>
#include <AnnStringUtility.hpp>
#include <AnnScriptManager.hpp>

//Other Annwvyn
#include <AnnTypes.h>
#include <AnnVect3.hpp>
#include <AnnQuaternion.hpp>
#include <AnnGetter.hpp>
#include <AnnOgreVRRenderer.hpp>

///Namespace containing the totality of Annwvyn components
namespace Annwvyn
{
	inline std::string getHMDFromCmdLine(const char* cmd)
	{
		//If no args, default
		if(strlen(cmd) == 0)
			return "DefaultRender";

		//Args should start by a '-'
		if(cmd[0] != '-')
			return "arg_error";

		//Convert to C++ string object
		const std::string strCmd{ cmd };

		//User want to use a rift
		if(strCmd == "-rift" || strCmd == "-ovr")
			return "OculusRender";

		//User want to use a Vive or another OpenVR headset
		if(strCmd == "-vive" || strCmd == "-openVR")
			return "OpenVRRender";

		//Not implemented. I don't have OSVR hardware to try it out.
		if(strCmd == "-osvr")
			return "OSVRRender";

		//Bodge to run game on a flat screen on hardware that is not compatible
		//with
		if(strCmd == "-noVR")
			return "NoVRRender";

		return "arg_error";
	}

	inline void preStart()
	{
#ifdef ANNWVYN_DEVEL
		AnnEngine::openConsole();
		std::cout << "ANNWVYN DEVEL\n";
		std::cout << "Console output enabled\n";
#endif
	}

	inline void postQuit()
	{
#ifdef ANNWVYN_DEVEL
		std::stringstream filename;
		filename << "AnnwvynDevel-";
		auto t = time(nullptr);
		struct tm now;
		localtime_s(&now, &t);
		filename
			<< now.tm_year + 1900 << '-'
			<< now.tm_mon + 1 << '-'
			<< now.tm_mday << '-'
			<< now.tm_hour << '-'
			<< now.tm_min << '-'
			<< now.tm_sec << ".log";

		auto src{ std::ifstream(AnnEngine::logFileName, std::ios::binary) };
		auto dst{ std::ofstream(filename.str(), std::ios::binary) };

		dst << "Annwvyn DEVEL LOG\n"
			<< std::endl;

		dst << src.rdbuf();

#endif
	}
}

///Annwvyn initialization macro
#define AnnInit(AppName) auto GameEngine = std::make_unique<AnnEngine>(AppName, detectedHMD)

#define AnnQuit()              \
	GameEngine.reset(nullptr); \
	Annwvyn::postQuit();

//===================Application Entry-point definition=================//
/*Main definition :
 *
 *	For more simplicity, Program start by a "AnnMain" function at the library
 *	user side. This allow to select proper entry point for the application, and
 *	maybe add pre-starting treatments here.
 */

#ifdef _WIN32
#	include "windows.h"

	///Application entry point
#	define AnnMain()                                                                       \
		int AnnwvynStart();                                                                 \
		std::string detectedHMD;                                                            \
		INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)               \
		{                                                                                   \
			detectedHMD = Annwvyn::getHMDFromCmdLine(static_cast<const char*>(strCmdLine)); \
			Annwvyn::preStart();                                                            \
			return AnnwvynStart();                                                          \
		}                                                                                   \
		int AnnwvynStart()

#else

	///Application entry point
#	define AnnMain()                                              \
		int AnnwvynStart();                                        \
		std::string detectedHMD;                                   \
		int main(int argc, char** argv)                            \
		{                                                          \
			if(argc < 2)                                           \
				detectedHMD = Annwvyn::getHMDFromCmdLine("");      \
			else                                                   \
				detectedHMD = Annwvyn::getHMDFromCmdLine(argv[1]); \
			Annwvyn::preStart();                                   \
			return AnnwvynStart();                                 \
		}                                                          \
		int AnnwvynStart()

#endif
//=======================================================================//
