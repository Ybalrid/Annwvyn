#ifndef ANN_CHARACTER
#define ANN_CHARACTER

#include "systemMacro.h"
#include "AnnGameObject.hpp"

#define dead false
#define living true
typedef bool lifeStatus;

namespace Annwvyn
{
	class DLL AnnCharacter : public Annwvyn::AnnGameObject
	{
	public:
		AnnCharacter();

		lifeStatus isAlive();

		int getCurrentLifeLevel();
		
		void setMaximumLife(int newLifeMax);

		void heal(int amount);
		void harm(int amount);

		virtual void onDeath(){return;}

		void attack(AnnCharacter* enemy);
		void attack(AnnCharacter* enemy, int amount);
		
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
