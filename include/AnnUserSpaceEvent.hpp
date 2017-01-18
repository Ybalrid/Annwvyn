#pragma once

#include <memory>
#include <string>
#include <functional>

#include "systemMacro.h"

namespace Annwvyn
{
	class DLL AnnUserSpaceEvent
	{
	public:
		explicit AnnUserSpaceEvent(std::string typeName);
		virtual ~AnnUserSpaceEvent() = default;
		using AnnUserSpaceEventTypeHash = size_t;
		AnnUserSpaceEventTypeHash getType() const;
	private:
		const std::string eventTypeName;
		const AnnUserSpaceEventTypeHash eventTypeHash;
	};

	class DLL AnnUserSpaceEventLauncher
	{
	public:
		virtual ~AnnUserSpaceEventLauncher() = default;
		AnnUserSpaceEventLauncher();
	protected:
		virtual void dispatchEvent(AnnUserSpaceEvent& e);
	};
}