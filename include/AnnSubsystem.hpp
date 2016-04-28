#ifndef ANN_SUB_SYSTEM
#define ANN_SUB_SYSTEM
#include "systemMacro.h"
#include <string>
namespace Annwvyn
{
	class AnnEngine;
	class DLL AnnSubSystem
	{
	public:
		AnnSubSystem(std::string systemName);

	protected:
		friend class AnnEngine;
		virtual void update();
		virtual bool needUpdate();

	};
}
#endif //ANN_SUB_SYSTEM