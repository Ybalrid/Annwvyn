#pragma once

#include "systemMacro.h"
#include "AnnTypes.h"

namespace Annwvyn
{
	///Abstract Class that represent a movable object. Define the API used to move simple objects in 3D space 
	class DLL AnnAbstractMovable
	{
		virtual void setPosition(AnnVect3 position) = 0;
		virtual void setOrientation(AnnQuaternion quaternion) = 0;
		virtual AnnVect3 getPosition() = 0;
		virtual AnnQuaternion getOrientation() = 0;
	};
}
