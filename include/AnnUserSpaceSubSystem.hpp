#ifndef ANN_USR_SUBSYSTEM
#define ANN_USR_SUBSYSTEM

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include <memory>

#define AnnUserSystemAs(type) std::dynamic_pointer_cast<type>

namespace Annwvyn
{
	class AnnEngine;
	class DLL AnnUserSubSystem : public AnnSubSystem
	{
	public:
		AnnUserSubSystem(std::string systemName);
	protected:
		friend class AnnEngine;
		virtual void update();
		virtual bool needUpdate();
	};
}

#endif
