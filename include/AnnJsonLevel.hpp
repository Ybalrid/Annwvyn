#pragma once

#include <systemMacro.h>
#include <AnnLevel.hpp>

#include <memory>
namespace Annwvyn
{
	class AnnDllExport AnnJsonLevel : LEVEL
	{
		struct AnnJson;
		using AnnJsonOpaquePtr = std::unique_ptr<AnnJson>;

	public:
		AnnJsonLevel(std::string path);
		AnnJsonLevel(bool, std::string json);
		virtual ~AnnJsonLevel();
		void load() override;
		void runLogic() override;

	private:
		AnnJsonOpaquePtr jsonFile;

		void processJson();
	};
}
