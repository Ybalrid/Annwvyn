/*!
* \file AnnTypes.h
* \brief Data types definition for Annwvyn
* \author Arthur Brainville
* \version 0.0.2
*/

#ifndef ANNTYPES
#define ANNTYPES

#include "systemMacro.h"

#include <list>
#include <vector>
#include <memory>
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"
#include "AnnLightObject.hpp"
#include "AnnColor.hpp"

namespace Annwvyn
{
	//Predefinition of Annwvyn classes : 
	class AnnGameObject;
	class AnnTriggerObject;

	///Harmonise names :
	typedef Ogre::Vector2 AnnVect2;

	//Remove some ugliness : 
	typedef std::list<AnnTriggerObject *> AnnTriggerObjectList;
	typedef std::list<std::shared_ptr<AnnGameObject> > AnnGameObjectList;
	typedef std::list<AnnLightObject *> AnnLightList;

	///Because sometimes, after one byte you're full...
	typedef unsigned char byte;


	///Name of the objects physical shapes
	///Theses shapes are available to create objects's rigid body for the physics
	enum phyShapeType {
		staticShape, 
		convexShape, 
		boxShape, 
		cylinderShape, 
		capsuleShape,
		sphereShape
	};

	///Macro used 
#define MASK(x) (1<<(x))
}
#endif
