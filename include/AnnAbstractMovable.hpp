#pragma once

#include "systemMacro.h"

#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

namespace Annwvyn
{
	///Abstract Class that represent a an object that you can set the position. Define the API used to move simple objects in 3D space
	class AnnDllExport AnnAbstractTranslatable
	{
	public:
		virtual ~AnnAbstractTranslatable() = default;
	protected:
		virtual void setPosition(AnnVect3 position) = 0;
		virtual AnnVect3 getPosition() = 0;
	};

	///Abstract Class that represent an bobject that you can set the orientation.
	class AnnDllExport AnnAbstractMovable : public AnnAbstractTranslatable
	{
	public:
		virtual ~AnnAbstractMovable() = default;
	protected:
		virtual void setOrientation(AnnQuaternion quaternion) = 0;
		virtual AnnQuaternion getOrientation() = 0;
	};
}