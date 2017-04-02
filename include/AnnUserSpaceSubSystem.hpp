#ifndef ANN_USR_SUBSYSTEM
#define ANN_USR_SUBSYSTEM

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include "AnnUserSpaceEvent.hpp"
#include <memory>

#define AnnUserSystemAs(type) std::dynamic_pointer_cast<type>

namespace Annwvyn
{
	class AnnEngine;
	///User defined subsystem base class
	class DLL AnnUserSubSystem : public AnnSubSystem, AnnUserSpaceEventLauncher
	{
	public:
		///Construct the user subsystem. The systemName is mandatory
		/// \param systemName The name of the subsystem. Should be unique.
		AnnUserSubSystem(std::string systemName);

	protected:

		friend class AnnEngine;

		///Called at each refresh if this->needUpdate() returns true
		void update() override;

		///Marker if the system wants it's "update" method to be called by the engine. this is tested each frames.
		bool needUpdate() override;

		///Give an event to be dispatched to listeners via the event manager
		void dispatchEvent(std::shared_ptr<AnnUserSpaceEvent> e) final;
	};
}

#endif