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
#include "AnnAngle.hpp"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"
#include "AnnColor.hpp"

namespace Annwvyn
{
	//Predefinition of Annwvyn classes :
	class AnnGameObject;
	class AnnTriggerObject;
	class AnnLightObject;

	///Harmonize names :
	using AnnVect2 = Ogre::Vector2;
	using AnnMatrix3 = Ogre::Matrix3;
	using AnnMatrix4 = Ogre::Matrix4;

	//Remove some ugliness :
	using AnnTriggerObjectList = std::list<std::shared_ptr<AnnTriggerObject>>;
	using AnnGameObjectList = std::list<std::shared_ptr<AnnGameObject>>;
	using AnnLightList = std::list <std::shared_ptr<AnnLightObject>>;

	///Because sometimes, after one byte you're full...
	using byte = uint8_t;
	using uID = unsigned long long;

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
}
#endif
