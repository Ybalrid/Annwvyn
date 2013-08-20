#ifndef ANNTYPES
#define ANNTYPES

#include <Ogre.h>
#include <OIS.h>
#include "euler.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "AnnGameObject.hpp"
namespace Annwvyn
{
	typedef struct bodyParams bodyParams;
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

	typedef int phyShapeType;
	enum {staticShape, 
		convexShape, 
		boxShape, 
		cylinderShape, 
		capsuleShape};

	typedef Ogre::Light AnnLightObject;	
}
#endif
