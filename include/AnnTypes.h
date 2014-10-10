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

#include <Ogre.h>
#include <OIS.h>
#include "euler.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "AnnGameObject.hpp"
#include "AnnTriggerObject.hpp"

namespace Annwvyn
{
	//Predefinition of Annwvyn classes : 
    class AnnGameObject;
	class AnnTriggerObject;

	///Harmonise names : 
	typedef Ogre::Light AnnLightObject; //Yeah, it's a cheater thing here...

	///Remove some ugliness : 
	typedef std::vector<AnnTriggerObject * > AnnTriggerObjectVect;
    typedef std::vector<Annwvyn::AnnGameObject * > AnnGameObjectVect;
    typedef std::vector<AnnLightObject * > AnnLightVect; 

	///Parameters of the VirtualBody	
	struct bodyParams
	{
		float eyeHeight;
		float walkSpeed;
		float turnSpeed;
		float mass;
		Ogre::Vector3 Position;
		Ogre::Euler Orientation;
		Ogre::Quaternion HeadOrientation;
		//bullet
		btCollisionShape* Shape;
		btRigidBody* Body;
	};

	enum phyShapeType {
		staticShape, 
		convexShape, 
		boxShape, 
		cylinderShape, 
		capsuleShape,
		sphereShape
	};

	
	//controls
	namespace Keys
	{
		enum FPS_controllSheme
		{
			FW,		//Foward
			BK,		//Backward
			SL,		//Straff Left
			SR,		//Straff Right
			JUMP,	//Jump
			RUN		//Run
		};
	}
}
#endif
