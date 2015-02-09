#include "AnnEventManager.hpp"

using namespace Annwvyn;

AnnAbstractEventListener::AnnAbstractEventListener(AnnPlayer* p)
{
	player = p;
}

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
	for(size_t i(0); i < KeyCode::SIZE; i++) previousKeyStates[i] = false;
}

AnnEventManager::~AnnEventManager()
{
}

void AnnEventManager::setListener(AnnAbstractEventListener* l)
{
	listener = l;
}

void AnnEventManager::removeListener()
{
	listener = NULL;
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
	//if keyboard system initialized
	if(Keyboard)
	{
		//for each key of the keyboard
		for(int c (0); c < KeyCode::SIZE; c++)
		{
			//if it's pressed
			if(Keyboard->isKeyDown(static_cast<OIS::KeyCode>(c)))
			{
				//and wasn't before
				if(!previousKeyStates[c])
				{
					//create a coresponding key event 
					AnnKeyEvent e;
					e.setCode((KeyCode::code)c);
					e.setPressed();
					e.populate();
					e.validate();
					if(listener) //notify an eventual listener
						listener->KeyEvent(e);

					previousKeyStates[c] = true;
				}
			}
			else //key not pressed atm
			{
				//but was pressed just before
				if(previousKeyStates[c])
				{
					//same thing
					AnnKeyEvent e;
					e.setCode((KeyCode::code)c);
					e.setRelased();
					e.populate();
					e.validate();
					if(listener)
						listener->KeyEvent(e);

					previousKeyStates[c] = false;
				}
			}

			 
		}
	}

	if(Mouse)
	{
		//not implemented yet
	}

	if(Joystick)
	{
		//not implemented yet
	}
}