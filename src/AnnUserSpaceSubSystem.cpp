#include "stdafx.h"
#include "AnnUserSpaceSubSystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnUserSubSystem::AnnUserSubSystem(std::string systemName) : AnnSubSystem(systemName)
{
	AnnDebug() << "^^^^^ This system is user defined.";
}

void Annwvyn::AnnUserSubSystem::update()
{
	return; 
}

bool Annwvyn::AnnUserSubSystem::needUpdate()
{
	return false;
}
