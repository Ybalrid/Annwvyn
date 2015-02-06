#include "AnnEventManager.hpp"

using namespace Annwvyn;

AnnEvent::AnnEvent() :
	accepted(false),
	rejected(false),
	unpopulated(true),
	valid(false)
{
}

void AnnEvent::validate()
{
	valid = true;
}

void AnnEvent::populate()
{
	unpopulated = false;
}

AnnEventManager::AnnEventManager() :
	listener(NULL),
	Keyboard(NULL),
	Mouse(NULL),
	Joystick(NULL)
{
}

AnnEventManager::~AnnEventManager()
{
}

//////////////// PRIVATE SETTERS
void AnnEventManager::setKeyboard(OIS::Keyboard* k)
{
	Keyboard = k;
}

void AnnEventManager::setMouse(OIS::Mouse* m)
{
	Mouse = m;
}

void AnnEventManager::setJoystick(OIS::JoyStick* stick)
{
	Joystick = stick;
}
//////////////////////////////////////

void AnnEventManager::update()
{
	if(Keyboard)
	{
		for(int c (0) ; c < KeyCode::SIZE; c++)
		{
			if(Keyboard->isKeyDown(static_cast<OIS::KeyCode>(c)))
			{
				AnnKeyEvent e;
				e.setCode((KeyCode::code)c);
				e.setPressed();
				e.validate();
				if(listener)
					listener->KeyEvent(e);
			}
		}
	}

	if(Mouse)
	{
	}

	if(Joystick)
	{
	}
}