#include "stdafx.h"
#include "AnnEventManager.hpp"
#include "AnnLogger.hpp"//to access logger static method
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnAbstractEventListener::AnnAbstractEventListener()
{
	player = AnnEngine::Instance()->getPlayer();
}

float AnnAbstractEventListener::trim(float v, float dz)
{
	//Compute absolute value of v
	float abs(v); 
	if(v < 0) abs = -v;
	
	//The test is done on the abs value. Return the actuall value, or 0 if under the deadzone
	if(abs >= dz) return v; 
	return 0.0f;
}


AnnEventManager::AnnEventManager(Ogre::RenderWindow* w) :
	Keyboard(NULL),
	Mouse(NULL),
	Joystick(NULL)
{

	for(size_t i(0); i < KeyCode::SIZE; i++) previousKeyStates[i] = false;
	for(size_t i(0); i < MouseButtonId::nbButtons; i++) previousMouseButtonStates[i] = false;
	InputManager = NULL;

	//Should be a HWND under windows, but, whatever, it's an unsigned integer...
	size_t windowHnd;
	w->getCustomAttribute("WINDOW", &windowHnd);
	
	//Well, I think the best thing on the C++ standard library are the stream classes! :-D
	std::stringstream windowHndStr;
	windowHndStr << windowHnd;

	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	InputManager = OIS::InputManager::createInputSystem(pl);

	Keyboard = static_cast<OIS::Keyboard*>(InputManager->createInputObject(OIS::OISKeyboard, true));
	Mouse = static_cast<OIS::Mouse*>(InputManager->createInputObject(OIS::OISMouse, true));
	if(InputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
	{
		Joystick = static_cast<OIS::JoyStick*>(InputManager->createInputObject(OIS::OISJoyStick, true));
		AnnDebug()<< "Detected joystick : " << Joystick->vendor();
	}
	lastTimerCreated = 0;
}

AnnEventManager::~AnnEventManager()
{
	delete Keyboard;
	delete Mouse;
	delete Joystick;
}

void AnnEventManager::addListener(AnnAbstractEventListener* l)
{
	if(l != NULL)
	listeners.push_back(l);
}

void AnnEventManager::clearListenerList()
{
	listeners.clear();
}

//l equals NULL by default 
void AnnEventManager::removeListener(AnnAbstractEventListener* l)
{
	if(l == NULL) {clearListenerList(); return;}

	auto iterator = listeners.begin();
	while(iterator != listeners.end())
		if(*iterator == l)
			iterator = listeners.erase(iterator);
}

void AnnEventManager::update()
{
	processInput();
	processTimers();
}

void AnnEventManager::processInput()
{
	//Capture events
	Keyboard->capture();
	Mouse->capture();
	if(Joystick)
		Joystick->capture();

	//if keyboard system initialized
	if(Keyboard)
	{
		//for each key of the keyboard
		for(size_t c (0); c < KeyCode::SIZE; c++)
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

					for(size_t i(0); i < listeners.size(); i++)
						listeners[i]->KeyEvent(e);

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
					e.setReleased();
					e.populate();
					e.validate();

					for(size_t i(0); i < listeners.size(); i++)
						listeners[i]->KeyEvent(e);

					previousKeyStates[c] = false;
				}
			}
		}
	}

	if(Mouse)
	{
		OIS::MouseState state(Mouse->getMouseState());

		AnnMouseEvent e;

		for(size_t i(0); i < MouseButtonId::nbButtons; i++)
			e.setButtonStatus(MouseButtonId(i),state.buttonDown(OIS::MouseButtonID(i)));

		e.setAxisInformation(MouseAxisId::X, AnnMouseAxis(MouseAxisId::X, state.X.rel, state.X.abs));
		e.setAxisInformation(MouseAxisId::Y, AnnMouseAxis(MouseAxisId::Y, state.Y.rel, state.Y.abs));
		e.setAxisInformation(MouseAxisId::Z, AnnMouseAxis(MouseAxisId::Z, state.Z.rel, state.Z.abs));

		e.populate();
		e.validate();

		for(size_t i(0); i < listeners.size(); i++)
			listeners[i]->MouseEvent(e);
	}

	if(Joystick)
	{
        OIS::JoyStickState state(Joystick->getJoyStickState());
        
        AnnStickEvent e;

        //Get all butons imediate data
        e.buttons = state.mButtons;
        //Get all axes imediate data
        for(size_t i = 0; i < state.mAxes.size(); i++)
        {
            AnnStickAxis axis(i, state.mAxes[i].rel, state.mAxes[i].abs);
            if(state.mAxes[i].absOnly)
                axis.noRel = true;

            e.axes.push_back(axis);
        }

        //Get press and release event lists
        for(unsigned short i(0); i < state.mButtons.size() && i < this->previousStickButtonStates.size(); i++)
            if(!previousStickButtonStates[i] &&  state.mButtons[i])
                e.pressed.push_back(i);
            else if(previousStickButtonStates[i] && !state.mButtons[i])
                e.released.push_back(i);

        //Save current buttons state for next frame
        previousStickButtonStates = state.mButtons; 
        e.vendor = Joystick->vendor();
        e.populate();
        e.validate();

		for(size_t i(0); i < listeners.size(); i++)
			listeners[i]->StickEvent(e);
	}
}

timerID AnnEventManager::fireTimer(double delay)
{
	timerID newID = lastTimerCreated++;
	futureTimers.push_back(AnnTimer(newID, delay));
	return newID;
}

void AnnEventManager::processTimers()
{
	//This permit listeners to set timers without invalidating the iterator declared below
	if(!futureTimers.empty())
	{
		for(size_t i(0); i < futureTimers.size(); i++)
			activeTimers.push_back(futureTimers[i]);
		futureTimers.clear();
	}

	auto iterator = activeTimers.begin();
	while(iterator != activeTimers.end())
	{
		if(iterator.operator*().isTimeout())
		{
			AnnTimeEvent e;
			e.populate();
			e.setTimerID((*iterator).tID);
			e.validate();
			for(size_t i(0); i < listeners.size(); ++i)
				listeners[i]->TimeEvent(e);
			iterator = activeTimers.erase(iterator);
		}
		else
		{
			++iterator;
		}
	}
}


