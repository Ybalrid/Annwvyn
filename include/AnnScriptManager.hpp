#pragma once

#include <chaiscript.hpp>
#include <chaiscript_stdlib.hpp>

#include "systemMacro.h"
#include "AnnSubsystem.hpp"

namespace Annwvyn
{
	///Script Manager, serve as an interface between ChaiScript and the rest of the engine
	class DLL AnnScriptManager : public AnnSubSystem
	{
	public:

		///Construct the script manager, initialize ChaiScript and add global functions
		AnnScriptManager();

		///This subsystem doesn't need to be updated
		bool needUpdate() { return false; };

		///This method does nothing
		void update() { return; }
	private:

		///ChaiScript engine
		chaiscript::ChaiScript chai;
	};
}