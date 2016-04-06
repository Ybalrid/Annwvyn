#include "stdafx.h"
#include "AnnLevelManager.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnLevelManager::AnnLevelManager() :
	current(NULL)
{
	AnnDebug() << "Starting Level Subsystem";
}

AnnLevelManager::~AnnLevelManager()
{
	AnnEngine::Instance()->log("Deleting the Level Manager. Destroing every level known by the Level Manager before.");
	//clear the levels
	for(auto it = levelList.begin(); it != levelList.end(); it++)
		delete *it;
	levelList.clear();
}

void AnnLevelManager::jump(size_t levelId)
{
	if(!(levelId < levelList.size())) return;
	if(current)
		current->unload();
	current = levelList[levelId];
	current->load();
}

void AnnLevelManager::jump(AnnAbstractLevel* level)
{
	for(size_t i(0); i < levelList.size(); i++)
		if(levelList[i] == level)
		{
			jump(i);
			break;
		}
}

void AnnLevelManager::addLevel(AnnAbstractLevel* level)
{
	if(!level) return;
	levelList.push_back(level);
}

void AnnLevelManager::jumpToFirstLevel()
{
	jump(level_id(0));
}

void AnnLevelManager::update()
{
	if(current) current->runLogic();
}

void AnnLevelManager::unloadCurrentLevel()
{
	if(current) current->unload();
	current = NULL;
}
