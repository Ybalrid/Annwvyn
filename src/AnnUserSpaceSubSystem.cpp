#include "stdafx.h"
#include "AnnUserSpaceSubSystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnUserSubSystem::AnnUserSubSystem(std::string systemName) : AnnSubSystem(systemName)
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