#include "stdafx.h"
#include "AnnSubsystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnSubSystem::AnnSubSystem(std::string systemName)
{
	AnnDebug() << "*-*-*-* Starting " << systemName << " subsystem";
}

bool AnnSubSystem::needUpdate()
{ 
	return true;
}

void AnnSubSystem::update()
{
	return;
}
