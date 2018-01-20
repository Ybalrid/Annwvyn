// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnUserSpaceEvent.hpp"
#include "AnnGetter.hpp"
using namespace Annwvyn;

AnnUserSpaceEvent::AnnUserSpaceEvent(const std::string& typeName) :
 eventTypeName(typeName),
 eventTypeHash(AnnGetStringUtility()->hash(eventTypeName))
{
}

void AnnUserSpaceEventLauncher::dispatchEvent(AnnUserSpaceEventPtr e)
{
	AnnGetEventManager()->userSpaceDispatchEvent(e, this);
}

AnnUserSpaceEvent::AnnUserSpaceEventTypeHash AnnUserSpaceEvent::getType() const
{
	return eventTypeHash;
}
