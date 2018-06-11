#pragma once

#include <systemMacro.h>
#include <AnnLevel.hpp>
#include <memory>

namespace Annwvyn
{
	///Level object loaded from a JSON file
	class AnnDllExport AnnJsonLevel : LEVEL
	{
		///Pimpl struct
		struct AnnJson;
		///PimplPointerType
		using AnnJsonOpaquePtr = std::unique_ptr<AnnJson>;

	public:
		///Construct a JSON level from a file
		AnnJsonLevel(std::string path, const bool preload = true);
		///Construct a json level from code. First boolean is thrown away
		AnnJsonLevel(bool, std::string jsonCode, const bool preload = true);
		///Dtor
		virtual ~AnnJsonLevel();
		///Load method. This goes through the loaded JSON and create the object for you
		void load() override;
		///Run logic, actually empty here
		void runLogic() override;

	private:
		///Pimpl
		AnnJsonOpaquePtr jsonFile;
		///Do initial processing on JSON
		void processJson();
		///If set to false, resource group will not be initialized
		const bool preloadResources;
	};
}
