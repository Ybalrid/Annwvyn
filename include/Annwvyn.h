#ifndef ANNWVYN
#define ANNWVYN

/*!
* \file Annwvyn.h
* \brief Main Annwvyn include file (to be used by client application)
* \author Arthur Brainville
* \version 0.0.5
*/

/*!
 *
 *\mainpage Annwvyn Game Engine API Reference documentation
 *
 * Welcome to the Annwvyn API documentation.
 *
 * Here you'll find the complete & exhaustive documentation of the engine (last version avilable in the master branch at the moment this site has been generated).
 *
 * I strongly recomned you to go to <a href="http://wiki.annwvyn.org/">the wiki</a> to get a more comprehensible help to start using the engine. There is a quick-start tutorial you can check-out.
 * The engine is based on Ogre and take for reference Ogre's resource management, file format, datastructures and geometric classes. 
 *
 * The aim of the project is to create something pretty strait-forward to make 3D interactive content. Making an high-level framework to create Oculus Rift compatible content. You dont't have to worry about doing the rendering, the tracking, simulating basic physics, ect.
 * 
 * If you have any suggestion, feel free to email me at <a href="mailto:developer@annwvyn.org">developer@annwvyn.org</a>.
 *
 * Enjoy your stay and make great virtual reality experiencies ;-)
 *
 */

//#include "OgreOculus.h"

//Annwvyn classes
#include "AnnEngine.hpp"
#include "AnnGameObject.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnAudioEngine.hpp"
#include "AnnMap.hpp"
#include "AnnJoystickController.hpp"
#include "AnnEventManager.hpp"

//Other Annwvyn
#include "AnnTypes.h"
#include "AnnTools.h"

//external that has to be showed to user
#include <OIS.h>

///Namespace containing the totality of Annwvyn components
namespace Annwvyn{}
#endif //ANNWVYN	
