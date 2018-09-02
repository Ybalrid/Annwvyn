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
 * it is intended to serve as a resource for the engine development, and the engine use.
 * Anything marked as "public" is available for games using Annwvyn, and for plugin
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
 * kiwi</a> to get a more comprehensible help to start using the engine,
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


///Namespace containing the totality of Annwvyn components
namespace Annwvyn
{}

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



/*Main definition :
 *
 *	For more simplicity, Program start by a "AnnMain" function at the library
 *	user side. This allow to select proper entry point for the application, and
 *	maybe add pre-starting treatments here.
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
///Application entry point
#define AnnMain()                                                         \
	int AnnwvynStart();                                                   \
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT) \
	{                                                                     \
		return AnnwvynStart();                                            \
	}                                                                     \
	int AnnwvynStart()

#else

///Application entry point
#define AnnMain()                   \
	int AnnwvynStart();             \
	int main(int argc, char** argv) \
	{                               \
		Annwvyn::preStart();        \
		return AnnwvynStart();      \
	}                               \
	int AnnwvynStart()

#endif
