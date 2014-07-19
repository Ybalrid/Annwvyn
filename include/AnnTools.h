#ifndef ANN_TOOLS
#define ANN_TOOLS

#include "systemMacro.h"


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
