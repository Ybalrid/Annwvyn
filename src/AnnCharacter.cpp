#include "stdafx.h"
#include "AnnCharacter.hpp"

using namespace Annwvyn;

AnnCharacter::AnnCharacter() : AnnGameObject()
{
}

int AnnCharacter::getCurrentLifeLevel()
{
	return life;
}

void AnnCharacter::setMaximumLife(int newLifeMax)
{
	if(newLifeMax > 0)
		lifeMax = newLifeMax;
}

void AnnCharacter::heal(int amount)
{
	if(amount < 0)return;

	if(life += amount > lifeMax)
		life = lifeMax;
}

void AnnCharacter::harm(int amount)
{
	if(amount < 0)return;

	if(life -= amount <= 0)
	{
		status = dead;
		life = 0;
		onDeath();
	}
}

void AnnCharacter::attack(AnnCharacter* enemy)
{
	enemy->harm(damage);
}

void AnnCharacter::attack(AnnCharacter* enemy, int amount)
{
	enemy->harm(amount);
}

void AnnCharacter::setName(std::string newName)
{
	if(!newName.empty())
		name = newName;
}
