/**
 * \file AnnCharacter.hpp
 * \brief Class representing an in-game (non-player) character
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANN_CHARACTER
#define ANN_CHARACTER

#include "systemMacro.h"
#include "AnnGameObject.hpp"

#define dead false
#define living true
typedef bool lifeStatus;

namespace Annwvyn
{
	///An NPC
	class DLL AnnCharacter : public Annwvyn::AnnGameObject
	{
	public:
		///Construct a character
		AnnCharacter();

		///Know if it's alive
		lifeStatus isAlive();

		///Get the lifepoints
		int getCurrentLifeLevel();
		
		///Set the maximum life value
		void setMaximumLife(int newLifeMax);

		///Heal a certain amount of life
		void heal(int amount);

		///Harm a certain amout of life
		void harm(int amount);

		///Action to be performed at character death
		virtual void onDeath(){return;}

		///Make the character attack another one
		void attack(AnnCharacter* enemy);
		
		///Make the character attack another one with a presice amount of damage
		void attack(AnnCharacter* enemy, int amount);
		
		///Set the name of the character
		void setName(std::string newName);
	protected:
		std::string name;
		lifeStatus status;

		int life;
		int lifeMax;

		int damage;
	};
}

#endif //ANN_CHARACTER 
