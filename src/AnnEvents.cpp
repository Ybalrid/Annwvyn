#include "AnnEventManager.hpp"

using namespace Annwvyn;

AnnKeyEvent::AnnKeyEvent() : AnnEvent()
{
	key = KeyCode::unassigned;
	pressed = false;
	relased = false;
}

void AnnKeyEvent::setPressed()
{
	pressed = true;
	relased = false;
}

void AnnKeyEvent::setRelased()
{
	pressed = false;
	relased = true;
}

void AnnKeyEvent::setCode(KeyCode::code c)
{
	key = c;
}

bool AnnKeyEvent::isPressed()
{
	return pressed;
}

bool AnnKeyEvent::isRelased()
{
	return relased;
}


Annwvyn::KeyCode::code AnnKeyEvent::getKey()
{
	return key;
}