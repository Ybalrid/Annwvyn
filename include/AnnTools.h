#ifndef ANN_TOOLS
#define ANN_TOOLS
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

#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <cmath>

#include "AnnGameObject.hpp"

using namespace Ogre;
namespace Annwvyn
{
	namespace Tools
	{

		namespace Geometry //for testing in gemometry
		{
			//Return the distance between the 2 objects centers 
			float DLL distance(Annwvyn::AnnGameObject* a, Annwvyn::AnnGameObject* b);
			float DLL distance(Ogre::Vector3 a, Ogre::Vector3 b);
		}

	}
}

#endif
