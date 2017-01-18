#include "stdafx.h"
#include "AnnUserSpaceEvent.hpp"
#include "AnnGetter.hpp"
using namespace Annwvyn;

AnnUserSpaceEvent::AnnUserSpaceEvent(std::string typeName) :
	eventTypeName(typeName),
	eventTypeHash(std::hash <std::string>()(eventTypeName))
{
}

void AnnUserSpaceEventLauncher::dispatchEvent(std::shared_ptr<AnnUserSpaceEvent> e)
{
	AnnGetEventManager()->userSpaceDispatchEvent(e, this);
}

AnnUserSpaceEvent::AnnUserSpaceEventTypeHash AnnUserSpaceEvent::getType() const
{
	return eventTypeHash;
}

AnnUserSpaceEventLauncher::AnnUserSpaceEventLauncher()
{
}