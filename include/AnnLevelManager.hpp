/**
 * \file AnnLevelManager.hpp
 * \brief Main class of the level system
 *	      The Level Manager load and unload levels from the internal Ogre Scene.
 *		  It also permit to switchToLevel from a level to another one.
 * \author A. Brainville (Ybalrid)
 */
#pragma once

#include <vector>
#include "AnnLevel.hpp"
#include "AnnSubsystem.hpp"
#include "AnnGameObject.hpp"

namespace Annwvyn
{
	using AnnLevelID = size_t;

	///Class that take care of switching between different levels dynamically and clearing the memory afterwards
	class AnnDllExport AnnLevelManager : public AnnSubSystem
	{
	public:
		///Construct LevelManager
		AnnLevelManager();

		///Destroy the level manager
		~AnnLevelManager();

		///Jump to an index referenced level
		///\param levelId Index of the level in the order they have been declared
		void switchToLevel(AnnLevelID levelId);

		///Jump to a pointer referenced level
		///\param level address of a subclass instance of AnnLevel
		void switchToLevel(AnnLevelPtr level);

		///Add a level to the level manager
		///\param level address of a subclass instance of AnnLevel
		void addLevel(AnnLevelPtr level);

		///Uitility overload to easilly create and add levels at the same time
		template <class LevelType, class... Args>
		decltype(auto) addLevel(Args&&... args)
		{
			auto level = std::make_shared<LevelType>(args...);
			addLevel(level);
			return level;
		}

		///Jump to the first level that was loaded into the engine
		void switchToFirstLoadedLevel();

		///Jump to the first level that was loaded into the engine
		void switchToLastLoadedLevel();

		///Run level logic
		void update() override;

		///Unload the level currently running
		void unloadCurrentLevel();

		///Retrieve the last loaded level pointer
		AnnLevelPtr getLastLoadedLevel();

		///Retrieve the first loaded level pointer
		AnnLevelPtr getFirstLoadedLevel();

		///Retrieve the `id`th  loaded level pointer
		AnnLevelPtr getLevelByIndex(AnnLevelID id);

		///Get the current level
		AnnLevelPtr getCurrentLevel() const;

		///Add an orphan object to the current level
		void addToCurrentLevel(AnnGameObjectPtr obj) const;

		///Remove an object from the current level (make it orphan)
		void removeFromCurrentLevel(AnnGameObjectPtr obj) const;

	private:
		///List of levels
		std::vector<AnnLevelPtr> loadedLevels;

		///Address to the currently running level
		AnnLevelPtr current;

		///Will jumpt to a level at next update
		bool jumpRequested;

		///Level to switchToLevel to at next update
		AnnLevelID jumpTo;
	};

	using AnnLevelManagerPtr = std::shared_ptr<AnnLevelManager>;
}
