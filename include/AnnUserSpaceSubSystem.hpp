#ifndef ANN_USR_SUBSYSTEM
#define ANN_USR_SUBSYSTEM

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include <memory>

#define AnnUserSystemAs(type) std::dynamic_pointer_cast<type>

namespace Annwvyn
{
	class AnnEngine;
	///User defined subsystem base class
	class DLL AnnUserSubSystem : public AnnSubSystem
	{
	public:
		///Construct the user subsystem. The systemName is mandatory
		/// \param systemName The name of the subsystem. Should be unique.
		AnnUserSubSystem(std::string systemName);
	protected:
		friend class AnnEngine;

		///Called at each refresh if this->needUpdate() returns true
		virtual void update();

		///Marker if the system wants it's "update" method to be called by the engine. this is tested each frames.
		virtual bool needUpdate();
	};
}

#endif