#include "stdafx.h"
#include "AnnSubsystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnSubSystem::AnnSubSystem(std::string systemName) :
	name(systemName)
{
	AnnDebug() << "*-*-*-* Starting " << name << " SubSystem";
}

AnnSubSystem::~AnnSubSystem()
{
	AnnDebug() << "*-*-*-* Stoping " << name << " SubSystem";
}

bool AnnSubSystem::needUpdate()
{
	return true;
}

void AnnSubSystem::update()
{
	return;
}