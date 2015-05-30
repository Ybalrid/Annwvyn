//Remove some visual studio stupidity
#pragma warning (disable : 4244)

//Object-Oriented Graphical Rendering Engine
#include <Ogre.h>
#include <OgrePrerequisites.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreLight.h>

//Object-Oriented Input System
#include <OIS.h>

//Bullet
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>

//OpenAl
#include <al.h>
#include <alc.h>

//libsndfile
#include <sndfile.h>

//Oculus VR API
#include <OVR.h>
#include <OVR_CAPI.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 
#include "windows.h"
#endif