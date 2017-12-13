#pragma once

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include "AnnUserSpaceEvent.hpp"

#include <sstream>
#include <string>
#include <iostream>

#include <memory>

#define AnnUserSystemAs(type) std::dynamic_pointer_cast<type>

//Build configuraiton to export symbols insiside
#ifdef _WIN32
#define AnnUserSubSystemPluginExport __declspec(dllexport)
#else
#define AnnUserSubSystemPluginExport //This is dealt with in the compiler call, not in the source code
#endif

//Macro definiton for the plugin "bootstrap" symbol declaration and implementation.
//AnnEngine expect to find a C function called "AnnBootPlugin_PLUGIN_NAME" inside the DLL
#define AnnUserSubSystemPluginBootReturnType Annwvyn::AnnUserSubSystem*
#define AnnUserSubSystemPluginCatNameImpl(a,b) a##b
#define AnnUserSubSystemPluginCatName(a,b) AnnUserSubSystemPluginCatNameImpl(a, b)
#define AnnBootPluginPrefix AnnBootPlugin_
#define AnnUserSubSystemPluginBootName(AnnUserSubSystemPluginClassName) AnnUserSubSystemPluginCatName(AnnBootPluginPrefix, AnnUserSubSystemPluginClassName)
#define AnnUserSubSystemPluginBootImpl(AnnUserSubSystemPluginClassName) AnnUserSubSystemPluginBootReturnType AnnUserSubSystemPluginBootName(AnnUserSubSystemPluginClassName)()
#define AnnUserSubSystemPluginBootDeclare(AnnUserSubSystemPluginClassName)  extern "C" AnnUserSubSystemPluginExport AnnUserSubSystemPluginBootImpl(AnnUserSubSystemPluginClassName)

namespace Annwvyn
{
	class AnnEngine;
	///User defined subsystem base class
	class AnnDllExport AnnUserSubSystem : public AnnSubSystem, AnnUserSpaceEventLauncher
	{
	public:
		///Construct the user subsystem. The systemName is mandatory
		/// \param systemName The name of the subsystem. Should be unique.
		AnnUserSubSystem(const std::string& systemName);

	protected:

		friend class AnnEngine;

		///Called at each refresh if this->needUpdate() returns true
		void update() override;

		///Marker if the system wants it's "update" method to be called by the engine. this is tested each frames.
		bool needUpdate() override;

		///Give an event to be dispatched to listeners via the event manager
		void dispatchEvent(AnnUserSpaceEventPtr e) override;

		///Consruct+distpatch user defined event
		template <class UserSpaceEventType, class ... Args> void dispatchEvent(Args&& ... args)
		{
			dispatchEvent(std::make_shared<UserSpaceEventType>(args ...));
		}
	};

	using AnnUserSubSystemPtr = std::shared_ptr<AnnUserSubSystem>;
}
