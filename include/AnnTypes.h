/*!
* \file AnnTypes.h
* \brief Data types definition for Annwvyn
* \author Arthur Brainville
* \version 0.0.2
*/

#ifndef ANNTYPES
#define ANNTYPES

#include "systemMacro.h"

#include <vector>
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

namespace Annwvyn
{
	//Predefinition of Annwvyn classes : 
    class AnnGameObject;
	class AnnTriggerObject;

	//Harmonise names :
	typedef Ogre::Light AnnLightObject;
	typedef Ogre::Vector2 AnnVect2;

	//Remove some ugliness : 
	typedef std::list<AnnTriggerObject *> AnnTriggerObjectList;
    typedef std::list<AnnGameObject *> AnnGameObjectList;
    typedef std::list<AnnLightObject *> AnnLightList;

	enum phyShapeType {
		staticShape, 
		convexShape, 
		boxShape, 
		cylinderShape, 
		capsuleShape,
		sphereShape
	};


#define MASK(x) (1<<(x))

}
#endif
