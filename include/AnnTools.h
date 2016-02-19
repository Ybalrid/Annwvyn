/**
 * \file AnnTools.h
 * \brief Misc tools for Annwvyn
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANN_TOOLS
#define ANN_TOOLS

#include "systemMacro.h"

#include <cmath>

#include "AnnGameObject.hpp"
#include "AnnTypes.h"
using namespace Ogre;
namespace Annwvyn
{
	///Return the identifier of a shape form it's name in text
	phyShapeType DLL getShapeTypeFromString(std::string str);
	namespace Tools
	{
		namespace Geometry //for testing in gemometry
		{
			///Return the distance between the 2 objects centers
			float DLL distance(Annwvyn::AnnGameObject* a, Annwvyn::AnnGameObject* b);

			///Return the distance between 2 points in space
			float DLL distance(Ogre::Vector3 a, Ogre::Vector3 b);
		}
	}
}

#endif
