#pragma once

#include "systemMacro.h"

#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

namespace Annwvyn
{
	///Abstract Class that represent a an object that you can set the position. Define the API used to move simple objects in 3D space
	class DLL AnnAbstractTranslatable
	{
	public:
		virtual ~AnnAbstractTranslatable() {}
	protected:
		virtual void setPosition(AnnVect3 position) = 0;
		virtual AnnVect3 getPosition() = 0;
	};

	///Abstract Class that represent an bobject that you can set the orientation.
	class DLL AnnAbstractMovable : public AnnAbstractTranslatable
	{
	protected:
		virtual void setOrientation(AnnQuaternion quaternion) = 0;
		virtual AnnQuaternion getOrientation() = 0;
	};
}