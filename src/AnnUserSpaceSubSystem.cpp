// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnUserSpaceSubSystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnUserSubSystem::AnnUserSubSystem(const std::string& systemName) :
 AnnSubSystem(systemName)
{
	AnnDebug() << "^^^^^ This system is user defined.";
}

void AnnUserSubSystem::update()
{
	return;
}

bool AnnUserSubSystem::needUpdate()
{
	return false;
}

void AnnUserSubSystem::dispatchEvent(AnnUserSpaceEventPtr e)
{
	AnnUserSpaceEventLauncher::dispatchEvent(e);
}
