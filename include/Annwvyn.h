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
 */

//Some C++ misc utils
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <unordered_map>

//Annwvyn classes
#include "AnnEngine.hpp"
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

//Other Annwvyn
#include "AnnTypes.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

#define AnnInit(AppName) new AnnEngine(AppName)

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
#define AnnMain() INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)

#else
///Application entry point
#define AnnMain() int main(int argc, char** argv)
#endif
//=======================================================================//

#endif //ANNWVYN	

#ifndef ANN_NAMESPACE
#define ANN_NAMESPACE
///Namespace containing the totality of Annwvyn components
namespace Annwvyn
{
}
#endif