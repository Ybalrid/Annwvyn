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
	
}

#endif
