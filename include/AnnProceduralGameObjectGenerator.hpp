#pragma once

#include "systemMacro.h"
#include "AnnGameObject.hpp"

namespace Annwvyn
{
	class AnnDllExport AnnProceduralGameObjectGenerator
	{

	public:
		AnnProceduralGameObjectGenerator();

		AnnGameObjectPtr getBox(std::string name, AnnVect3 size = AnnVect3::UNIT_SCALE, std::string identifier = "");
	};
}
