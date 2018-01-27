// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnLevelManager.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnLevelManager::AnnLevelManager() :
 AnnSubSystem("LevelManager"),
 current(nullptr),
 jumpRequested(false),
 jumpTo(0)
{
}

AnnLevelManager::~AnnLevelManager()
{
	AnnDebug() << "Deleting the Level Manager. Unloading current level and releasing all level pointers";
	unloadCurrentLevel();
}

void AnnLevelManager::switchToLevel(AnnLevelID levelId)
{
	if(!(levelId < loadedLevels.size())) return;

	//Deferred level jump
	if(!jumpRequested)
	{
		jumpRequested = true;
		jumpTo		  = levelId;
		return;
	}

	AnnDebug() << "LevelManager jumping to levelId : " << levelId;
	jumpRequested = false;
	jumpTo		  = 0;
	unloadCurrentLevel();
	current = loadedLevels[levelId];
	current->load();
}

void AnnLevelManager::switchToLevel(std::shared_ptr<AnnLevel> level)
{
	for(AnnLevelID i(0); i < loadedLevels.size(); i++)
		if(loadedLevels[i] == level)
		{
			switchToLevel(i);
			break;
		}
}

void AnnLevelManager::addLevel(std::shared_ptr<AnnLevel> level)
{
	AnnDebug() << "Adding level " << level << " to LevelManager";
	loadedLevels.push_back(level);
}

void AnnLevelManager::switchToFirstLoadedLevel()
{
	switchToLevel(AnnLevelID(0));
}

void AnnLevelManager::switchToLastLoadedLevel()
{
	switchToLevel(getLastLoadedLevel());
}

void AnnLevelManager::update()
{
	if(jumpRequested)
		return switchToLevel(jumpTo);
	if(current) current->runLogic();
}

void AnnLevelManager::unloadCurrentLevel()
{
	if(current) current->unload();
	current = nullptr;
}

std::shared_ptr<AnnLevel> AnnLevelManager::getLastLoadedLevel()
{
	return loadedLevels.back();
}

std::shared_ptr<AnnLevel> AnnLevelManager::getFirstLoadedLevel()
{
	return loadedLevels.front();
}

std::shared_ptr<AnnLevel> AnnLevelManager::getLevelByIndex(AnnLevelID id)
{
	if(id >= loadedLevels.size()) return nullptr;
	return loadedLevels[id];
}

void AnnLevelManager::addToCurrentLevel(std::shared_ptr<AnnGameObject> obj) const
{
	if(!current || !obj) return;
	current->levelContent.push_back(obj);
}

void AnnLevelManager::removeFromCurrentLevel(std::shared_ptr<AnnGameObject> obj) const
{
	if(!current || !obj) return;
	current->levelContent.erase(
		remove(begin(current->levelContent), end(current->levelContent), obj),
		end(current->levelContent));
}

std::shared_ptr<AnnLevel> AnnLevelManager::getCurrentLevel() const
{
	return current;
}
