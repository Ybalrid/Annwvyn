#ifndef ANNTYPES
#define ANNTYPES

/*!
* \file AnnTypes.h
* \brief Data types definition for Annwvyn
* \author Arthur Brainville
* \version 0.0.2
*/

#include "systemMacro.h"

#include <vector>
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

namespace Annwvyn
{
	//Predefinition of Annwvyn classes : 
    class AnnGameObject;
	class AnnTriggerObject;

	///Harmonise names :
	class Ogre::Light;
	typedef Ogre::Light AnnLightObject; //Yeah, it's a cheater thing here...

	///Remove some ugliness : 
	typedef std::vector<AnnTriggerObject *> AnnTriggerObjectVect;
    typedef std::vector<AnnGameObject *> AnnGameObjectVect;
    typedef std::vector<AnnLightObject *> AnnLightVect; 

	enum phyShapeType {
		staticShape, 
		convexShape, 
		boxShape, 
		cylinderShape, 
		capsuleShape,
		sphereShape
	};


#define BIT(x) (1<<(x))

}
#endif
