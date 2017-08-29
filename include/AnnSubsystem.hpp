#ifndef ANN_SUB_SYSTEM
#define ANN_SUB_SYSTEM

#include "systemMacro.h"

#include <string>
#include <memory>

namespace Annwvyn
{
	class AnnEngine;

	///Parent class of all Annwvyn SubSystem
	class AnnDllExport AnnSubSystem
	{
	public:
		///Construct a SubSystem
		AnnSubSystem(std::string systemName);

		///Destruct a SubSystem
		virtual ~AnnSubSystem();

	protected:
		friend class AnnEngine;

		///To be called by AnnEngine : update the subsystem for the next frame
		virtual void update();

		///To be called by AnnEngine : Return if the subsystem wants to be updated
		virtual bool needUpdate();

		///Name of the subsystem
		std::string name;
	};

	using AnnSubSystemPtr = std::shared_ptr<AnnSubSystem>;
}
#endif //ANN_SUB_SYSTEM