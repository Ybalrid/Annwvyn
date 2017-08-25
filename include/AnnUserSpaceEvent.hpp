#pragma once

#include <memory>
#include <string>
#include <functional>

#include "systemMacro.h"

namespace Annwvyn
{
	///Base class of an event created by user space code
	class AnnDllExport AnnUserSpaceEvent
	{
	public:
		///Create the event
		explicit AnnUserSpaceEvent(std::string typeName);

		///Destruct the event
		virtual ~AnnUserSpaceEvent() = default;

		///Define hash type
		using AnnUserSpaceEventTypeHash = size_t;

		///Get the hash of the type of this event
		AnnUserSpaceEventTypeHash getType() const;

	private:
		///Name of the type
		const std::string eventTypeName;

		///Hash of the type
		const AnnUserSpaceEventTypeHash eventTypeHash;
	};

	using AnnUserSpaceEventPtr = std::shared_ptr<AnnUserSpaceEvent>;

	///Class that can send a user space event to the event manager
	class AnnDllExport AnnUserSpaceEventLauncher
	{
	public:
		///Destruct the event launcher
		virtual ~AnnUserSpaceEventLauncher() = default;

		///Construct the event launcher
		AnnUserSpaceEventLauncher() = default;

	protected:
		///Dispatch an event via the event manager for you
		virtual void dispatchEvent(AnnUserSpaceEventPtr e);
	};
}