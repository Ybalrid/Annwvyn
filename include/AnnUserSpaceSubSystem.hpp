#ifndef ANN_USR_SUBSYSTEM
#define ANN_USR_SUBSYSTEM

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include "AnnUserSpaceEvent.hpp"
#include <memory>

#define AnnUserSystemAs(type) std::dynamic_pointer_cast<type>

#define AnnUserSubSystemPluginBootReturnType Annwvyn::AnnUserSubSystem*
#define AnnUserSubSystemPluginCatNameImpl(a,b) a##b
#define AnnUserSubSystemPluginCatName(a,b) AnnUserSubSystemPluginCatNameImpl(a, b)
#define AnnBootPluginPrefix AnnBootPlugin_
#define AnnUserSubSystemPluginBootName(AnnUserSubSystemPluginClassName) AnnUserSubSystemPluginCatName(AnnBootPluginPrefix, AnnUserSubSystemPluginClassName)
#define AnnUserSubSystemPluginBootImpl(AnnUserSubSystemPluginClassName) AnnUserSubSystemPluginBootReturnType AnnUserSubSystemPluginBootName(AnnUserSubSystemPluginClassName)()
#define AnnUserSubSystemPluginBootDeclare(AnnUserSubSystemPluginClassName)  extern "C" __declspec(dllexport) AnnUserSubSystemPluginBootImpl(AnnUserSubSystemPluginClassName)

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
		void dispatchEvent(AnnUserSpaceEventPtr e);

		///Consruct+distpatch user defined event
		template <class UserSpaceEventType, class ... Args> void dispatchEvent(Args&& ... args)
		{
			dispatchEvent(std::make_shared<UserSpaceEventType>(args ...));
		}
	};

	using AnnUserSubSystemPtr = std::shared_ptr<AnnUserSubSystem>;
}

#endif