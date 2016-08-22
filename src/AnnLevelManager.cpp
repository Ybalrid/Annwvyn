#include "stdafx.h"
#include "AnnLevelManager.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnLevelManager::AnnLevelManager() : AnnSubSystem("LevelManager"),
	current(NULL)
{
}

AnnLevelManager::~AnnLevelManager()
{
	AnnGetEngine()->log("Deleting the Level Manager. Destroing every level known by the Level Manager before.");
	//clear the levels
	/*for(auto it = levelList.begin(); it != levelList.end(); it++)
		delete *it;*/
	levelList.clear();
}

void AnnLevelManager::jump(level_id levelId)
{
	if(!(levelId < levelList.size())) return;
	if(current)
		current->unload();
	current = levelList[levelId];
	current->load();
	Sleep(10);
}

void AnnLevelManager::jump(std::shared_ptr<AnnLevel> level)
{
	for(level_id i(0); i < levelList.size(); i++)
		if(levelList[i] == level)
		{
			jump(i);
			break;
		}
}

void AnnLevelManager::addLevel(std::shared_ptr<AnnLevel> level)
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

std::shared_ptr<AnnLevel> AnnLevelManager::getLastLevelLoaded()
{
	return levelList.back();
}

std::shared_ptr<AnnLevel> AnnLevelManager::getFirstLevelLoaded()
{
	return levelList.front();
}

std::shared_ptr<AnnLevel> AnnLevelManager::getLevelByIndex(level_id id)
{
	if (id >= levelList.size()) return nullptr;
	return levelList[id];
}
