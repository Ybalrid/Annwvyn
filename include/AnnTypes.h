#ifndef ANNTYPES
#define ANNTYPES
#undef DLL
//windows DLL
#ifdef DLLDIR_EX
   #define DLL  __declspec(dllexport)   // export DLL information
#else
   #define DLL  __declspec(dllimport)   // import DLL information
#endif

//bypass on linux
#ifdef __gnu_linux__
#undef DLL
#define DLL
#endif

#include <vector>

#include <Ogre.h>
#include <OIS.h>
#include "euler.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "AnnGameObject.hpp"

namespace Annwvyn
{
    class AnnGameObject;
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
	enum {
		staticShape, 
		convexShape, 
		boxShape, 
		cylinderShape, 
		capsuleShape
	};

	typedef Ogre::Light AnnLightObject;

    typedef std::vector<Annwvyn::AnnGameObject * > AnnGameObjectVect;
    typedef std::vector<AnnLightObject * > AnnLightVect; 
	//controls
	namespace Keys
	{
		enum 
		{
			FW, BK, LT, RT, JUMP
		};
	}
}
#endif
