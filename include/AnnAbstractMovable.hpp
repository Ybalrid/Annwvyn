#pragma once

#include "systemMacro.h"

#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

namespace Annwvyn
{
	///Abstract Class that represent a movable object. Define the API used to move simple objects in 3D space
	class DLL AnnAbstractTranslatable
	{
		virtual void setPosition(AnnVect3 position) = 0;
		virtual AnnVect3 getPosition() = 0;
	};

	class DLL AnnAbstractMovable : public AnnAbstractTranslatable
	{
		virtual void setOrientation(AnnQuaternion quaternion) = 0;
		virtual AnnQuaternion getOrientation() = 0;
	};
}