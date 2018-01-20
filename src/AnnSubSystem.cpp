// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnSubsystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnSubSystem::AnnSubSystem(const std::string& systemName) :
 name(systemName)
{
	AnnDebug() << "*-*-*-* Starting " << name << " SubSystem";
}

AnnSubSystem::~AnnSubSystem()
{
	AnnDebug() << "*-*-*-* Stopping " << name << " SubSystem";
}

bool AnnSubSystem::needUpdate()
{
	return true;
}

void AnnSubSystem::update()
{
	return;
}
