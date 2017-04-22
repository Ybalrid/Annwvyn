/**
 * \file AnnLevelManager.hpp
 * \brief Main class of the level system
 *	      The Level Manager load and unload levels from the internal Ogre Scene.
 *		  It also permit to jump from a level to another one.
 * \author A. Brainville (Ybalrid)
 */
#ifndef ANN_LEVELMANAGER
#define ANN_LEVELMANAGER

#include <vector>
#include "AnnLevel.hpp"
#include "tinyxml2.h"
#include "AnnSubsystem.hpp"

namespace Annwvyn
{
	using level_id = size_t;

	///Class that take care of switching between different levels dynamically and clearing the memory afterwards
	class DLL AnnLevelManager : public AnnSubSystem
	{
	public:
		///Construct LevelManager
		AnnLevelManager();

		///Destroy the level manager
		~AnnLevelManager();

		///Jump to an index referenced level
		///\param levelId Index of the level in the order they have been declared
		void jump(level_id levelId);

		///Jump to a pointer referenced level
		///\param level address of a subclass instance of AnnLevel
		void jump(std::shared_ptr<AnnLevel> level);

		///Add a level to the level manager
		///\param level address of a subclass instance of AnnLevel
		void addLevel(std::shared_ptr<AnnLevel> level);

		///Jump to the 1st level
		void jumpToFirstLevel();

		///Run level logic
		void update() override;

		///Unload the level currently running
		void unloadCurrentLevel();

		///Retrieve the last loaded level pointer
		std::shared_ptr<AnnLevel> getLastLevelLoaded();

		///Retrieve the first loaded level pointer
		std::shared_ptr<AnnLevel> getFirstLevelLoaded();

		///Retrieve the `id`th  loaded level pointer
		std::shared_ptr<AnnLevel> getLevelByIndex(level_id id);

		///Get the current level
		std::shared_ptr<AnnLevel> getCurrentLevel() const;

		///Add an orphan object to the current level
		void addToCurrentLevel(std::shared_ptr<AnnGameObject> obj) const;

		///Remove an object from the current level (make it orphan)
		void removeFromCurrentLevel(std::shared_ptr<AnnGameObject> obj) const;

	private:

		///List of levels
		std::vector<std::shared_ptr<AnnLevel>> levelList;

		///Address to the currently running level
		std::shared_ptr<AnnLevel> current;

		///Will jumpt to a level at next update
		bool jumpRequested;

		///Level to jump to at next update
		level_id jumpTo;
	};
}
#endif //LEVELMANAGER
