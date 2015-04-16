#include "AnnEventManager.hpp"
#include "AnnEngine.hpp"//to access logger static method
using namespace Annwvyn;

AnnAbstractEventListener::AnnAbstractEventListener(AnnPlayer* p)
{
	player = p;
}

float AnnAbstractEventListener::trim(float v, float dz)
{
	float abs(v); 
	if(v < 0) abs = -v;
	
	if(abs >= dz) return v; 
	
	return 0.0f;
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

AnnEventManager::AnnEventManager(Ogre::RenderWindow* w) :
	listener(NULL),
	Keyboard(NULL),
	Mouse(NULL),
	Joystick(NULL)
{

	for(size_t i(0); i < KeyCode::SIZE; i++) previousKeyStates[i] = false;
	for(size_t i(0); i < MouseButtonId::nbButtons; i++) previousMouseButtonStates[i] = false;
	InputManager = NULL;

	//Should be a HWND under windows, but, whatever, it's an unsigned integer...
	size_t windowHnd;
	std::stringstream windowHndStr;
	w->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;

	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	InputManager = OIS::InputManager::createInputSystem(pl);

	Keyboard = static_cast<OIS::Keyboard*>(InputManager->createInputObject(OIS::OISKeyboard, true));
	Mouse = static_cast<OIS::Mouse*>(InputManager->createInputObject(OIS::OISMouse, true));

	if(InputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
	{
			Joystick = static_cast<OIS::JoyStick*>(InputManager->createInputObject(OIS::OISJoyStick, true));
			Annwvyn::AnnEngine::log(Joystick->vendor());
	}
}

AnnEventManager::~AnnEventManager()
{
	delete Keyboard;
	delete Mouse;
	delete Joystick;
	//delete InputManager;
}

void AnnEventManager::setListener(AnnAbstractEventListener* l)
{
	listener = l;
}

void AnnEventManager::removeListener()
{
	listener = NULL;
}

void AnnEventManager::update()
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
		OIS::MouseState state(Mouse->getMouseState());

		AnnMouseEvent e;

		for(size_t i(0); i < MouseButtonId::nbButtons; i++)
			e.setButtonStatus(MouseButtonId(i),state.buttonDown(OIS::MouseButtonID(i)));

		e.setAxisInformation(MouseAxisId::X, AnnMouseAxis(MouseAxisId::X, state.X.rel, state.X.abs));
		e.setAxisInformation(MouseAxisId::Y, AnnMouseAxis(MouseAxisId::Y, state.Y.rel, state.Y.abs));
		e.setAxisInformation(MouseAxisId::Z, AnnMouseAxis(MouseAxisId::Z, state.Z.rel, state.Z.abs));

		e.populate();
		e.validate();

		if(listener)
			listener->MouseEvent(e);
	}

	if(Joystick)
	{
        OIS::JoyStickState state(Joystick->getJoyStickState());
        
        AnnStickEvent e;

        //Get all butons imediate data
        e.buttons = state.mButtons;
        //Get all axes imediate data
        for(int i = 0; i < state.mAxes.size(); i++)
        {
            AnnStickAxis axis(i, state.mAxes[i].rel, state.mAxes[i].abs);
            if(state.mAxes[i].absOnly)
                axis.noRel = true;

            e.axes.push_back(axis);
        }

        //Get press and release event lists
        for(int i(0); i < state.mButtons.size() && i < this->previousStickButtonStates.size(); i++)
            if(!previousStickButtonStates[i] &&  state.mButtons[i])
                e.pressed.push_back(i);
            else if(previousStickButtonStates[i] && !state.mButtons[i])
                e.relased.push_back(i);

        //Save current buttons state for next frame
        previousStickButtonStates = state.mButtons; 
        e.vendor = Joystick->vendor();
        e.populate();
        e.validate();

        if(listener)
            listener->StickEvent(e);
	}
}
