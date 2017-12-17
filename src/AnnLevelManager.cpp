// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnLevelManager.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnLevelManager::AnnLevelManager() : AnnSubSystem("LevelManager"),
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

void AnnLevelManager::jump(level_id levelId)
{
	if (!(levelId < levelList.size())) return;

	//Deferred level jump
	if (!jumpRequested)
	{
		jumpRequested = true;
		jumpTo = levelId;
		return;
	}

	AnnDebug() << "LevelManager jumping to levelId : " << levelId;
	jumpRequested = false;
	jumpTo = 0;
	unloadCurrentLevel();
	current = levelList[levelId];
	current->load();
}

void AnnLevelManager::jump(std::shared_ptr<AnnLevel> level)
{
	for (level_id i(0); i < levelList.size(); i++)
		if (levelList[i] == level)
		{
			jump(i);
			break;
		}
}

void AnnLevelManager::addLevel(std::shared_ptr<AnnLevel> level)
{
	AnnDebug() << "Adding level " << level << " to LevelManager";
	levelList.push_back(level);
}

void AnnLevelManager::jumpToFirstLevel()
{
	jump(level_id(0));
}

void AnnLevelManager::update()
{
	if (jumpRequested)
		return jump(jumpTo);
	if (current) current->runLogic();
}

void AnnLevelManager::unloadCurrentLevel()
{
	if (current) current->unload();
	current = nullptr;
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

void AnnLevelManager::addToCurrentLevel(std::shared_ptr<AnnGameObject> obj) const
{
	if (!current || !obj) return;
	current->levelContent.push_back(obj);
}

void AnnLevelManager::removeFromCurrentLevel(std::shared_ptr<AnnGameObject> obj) const
{
	if (!current || !obj) return;
	current->levelContent.erase(
		remove(begin(current->levelContent), end(current->levelContent), obj), 
		end(current->levelContent));
}

std::shared_ptr<AnnLevel> AnnLevelManager::getCurrentLevel() const
{
	return current;
}