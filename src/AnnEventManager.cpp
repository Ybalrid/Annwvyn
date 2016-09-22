#include "stdafx.h"
#include "AnnEventManager.hpp"
#include "AnnLogger.hpp"//to access logger static method
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnEventListener::AnnEventListener() :
	player(AnnGetPlayer().get())
{
}

float AnnEventListener::trim(float v, float dz)
{
	//Compute absolute value of v
	float abs(v);
	if (v < 0) abs = -v;

	//The test is done on the abs value. Return the actuall value, or 0 if under the deadzone
	if (abs >= dz) return v;
	return 0.0f;
}


AnnTextInputer::AnnTextInputer() :
	listen(false)
{
}

bool AnnTextInputer::keyPressed(const OIS::KeyEvent &arg)
{
	if (!listen) return true;
	//If backspace, pop last char if possible
	if (arg.key == OIS::KC_BACK && !input.empty())
		input.pop_back();
	else
		//Put typed char into the application 
		input.push_back((char)arg.text);
	return true;
}

bool AnnTextInputer::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

std::string AnnTextInputer::getInput()
{
	return input;
}

void AnnTextInputer::clearInput()
{
	input.clear();
}

void AnnTextInputer::startListening()
{
	clearInput();
	listen = true;
}

void AnnTextInputer::stopListening()
{
	listen = false;
}

void AnnTextInputer::setInput(std::string content)
{
	input = content;
}


AnnEventManager::AnnEventManager(Ogre::RenderWindow* w) : AnnSubSystem("EventManager"),
Keyboard(nullptr),
Mouse(nullptr),
defaultEventListener(nullptr),
knowXbox(false)
{
	for (size_t i(0); i < KeyCode::SIZE; i++) previousKeyStates[i] = false;
	for (size_t i(0); i < MouseButtonId::nbButtons; i++) previousMouseButtonStates[i] = false;

	//Should be a HWND under windows, but, whatever, it's an unsigned integer...
	size_t windowHnd; w->getCustomAttribute("WINDOW", &windowHnd);

	//Well, I think the best thing on the C++ standard library are the stream classes! :-D
	std::stringstream windowHndStr; windowHndStr << windowHnd;

	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	InputManager = OIS::InputManager::createInputSystem(pl);

	Keyboard = static_cast<OIS::Keyboard*>(InputManager->createInputObject(OIS::OISKeyboard, true));
	Mouse = static_cast<OIS::Mouse*>(InputManager->createInputObject(OIS::OISMouse, true));

	for (int nbStick(0); nbStick < InputManager->getNumberOfDevices(OIS::OISJoyStick); nbStick++)
	{
		OIS::JoyStick* Joystick = static_cast<OIS::JoyStick*>(InputManager->createInputObject(OIS::OISJoyStick, true));
		Joysticks.push_back(new JoystickBuffer(Joystick));
		AnnDebug() << "Detected joystick : " << Joystick->vendor();
		if (Joystick->vendor().find("Xbox") != std::string::npos 
		|| Joystick->vendor().find("XBOX") != std::string::npos)
		{
			knowXbox = true;
			xboxID = (StickAxisId)Joystick->getID();
			AnnDebug() << "Detected Xbox controller at ID " << xboxID;
		}
	}

	lastTimerCreated = 0;

	textInputer = new AnnTextInputer;
	Keyboard->setEventCallback(textInputer);
}

AnnTextInputer* AnnEventManager::getTextInputer()
{
	return textInputer;
}

AnnEventManager::~AnnEventManager()
{
	clearListenerList();
	defaultEventListener = nullptr;
	Keyboard->setEventCallback(nullptr);
	delete textInputer;
	delete Keyboard;
	delete Mouse;
	for (auto Joystick : Joysticks)
		delete Joystick;
	Joysticks.clear();
}

void Annwvyn::AnnEventManager::useDefaultEventListener()
{
	AnnDebug("Reconfiguring the engine to use the default event listener");
	AnnDebug("This unregister any current listener in use!");

	//Remove all event listeners
	removeListener();

	//If the event listenre isn't allready initialized, allocate one
	if (!defaultEventListener)
		defaultEventListener = std::make_shared<AnnDefaultEventListener>();

	//Set the default event listener to the event manager
	addListener(defaultEventListener);
}

std::shared_ptr<AnnEventListener> Annwvyn::AnnEventManager::getDefaultEventListener()
{
	return defaultEventListener;
}

void AnnEventManager::addListener(std::shared_ptr<AnnEventListener> l)
{
	if (l != NULL)
		listeners.push_back(l);
}

void AnnEventManager::clearListenerList()
{
	listeners.clear();
}

//l equals NULL by default 
void AnnEventManager::removeListener(std::shared_ptr<AnnEventListener> l)
{
	if (l == nullptr) { clearListenerList(); return; }
	auto iterator = listeners.begin();
	while (iterator != listeners.end())
		if (*iterator == l)
			iterator = listeners.erase(iterator);
		else iterator++;
}

void AnnEventManager::update()
{
	processInput();
	processTimers();
	processTriggerEvents();
}

unsigned int JoystickBuffer::idcounter = 0;

void AnnEventManager::processInput()
{
	//Capture events
	Keyboard->capture();
	Mouse->capture();

	for (auto joystick : Joysticks)
		joystick->stick->capture();

	//if keyboard system initialized
	if (Keyboard)
	{
		//for each key of the keyboard
		for (size_t c(0); c < KeyCode::SIZE; c++)
		{
			//if it's pressed
			if (Keyboard->isKeyDown(OIS::KeyCode(c)) && !previousKeyStates[c])
			{
				//create a coresponding key event 
				AnnKeyEvent e;
				e.setCode((KeyCode::code)c);
				e.setPressed();
				e.populate();
				e.validate();

				for (size_t i(0); i < listeners.size(); i++)
					listeners[i]->KeyEvent(e);

				previousKeyStates[c] = true;
			}
			else if (!Keyboard->isKeyDown(OIS::KeyCode(c)) && previousKeyStates[c]) //key not pressed atm
			{
				//same thing
				AnnKeyEvent e;
				e.setCode((KeyCode::code)c);
				e.setReleased();
				e.populate();
				e.validate();

				for (auto listener : listeners)
					listener->KeyEvent(e);

				previousKeyStates[c] = false;
			}
		}
	}

	if (Mouse)
	{
		OIS::MouseState state(Mouse->getMouseState());

		AnnMouseEvent e;

		for (size_t i(0); i < MouseButtonId::nbButtons; i++)
			e.setButtonStatus(MouseButtonId(i), state.buttonDown(OIS::MouseButtonID(i)));

		e.setAxisInformation(MouseAxisId::X, AnnMouseAxis(MouseAxisId::X, state.X.rel, state.X.abs));
		e.setAxisInformation(MouseAxisId::Y, AnnMouseAxis(MouseAxisId::Y, state.Y.rel, state.Y.abs));
		e.setAxisInformation(MouseAxisId::Z, AnnMouseAxis(MouseAxisId::Z, state.Z.rel, state.Z.abs));

		e.populate();
		e.validate();

		for (size_t i(0); i < listeners.size(); i++)
			listeners[i]->MouseEvent(e);
	}

	for (auto Joystick : Joysticks)
	{
		OIS::JoyStickState state(Joystick->stick->getJoyStickState());
		AnnStickEvent e;

		//Get all butons imediate data
		e.buttons = state.mButtons;

		//Get all axes imediate data
		for (int i(0); i < state.mAxes.size(); i++)
		{
			AnnStickAxis axis(i, state.mAxes[i].rel, state.mAxes[i].abs);
			if (state.mAxes[i].absOnly)
				axis.noRel = true;
			e.axes.push_back(axis);
		}
		
		//The joystick state object allwas have 4 Pov but the AnnStickEvent has the number of Pov the stick has
		for (size_t i(0); i < Joystick->stick->getNumberOfComponents(OIS::ComponentType::OIS_POV); i++)
			e.povs.push_back(AnnStickPov(state.mPOV[i].direction));

		//Get press and release event lists
		for (unsigned short i(0); i < state.mButtons.size() && i < Joystick->previousStickButtonStates.size(); i++)
			if (!Joystick->previousStickButtonStates[i] && state.mButtons[i])
				e.pressed.push_back(i);
			else if (Joystick->previousStickButtonStates[i] && !state.mButtons[i])
				e.released.push_back(i);

		//Save current buttons state for next frame
		Joystick->previousStickButtonStates = state.mButtons;
		e.vendor = Joystick->stick->vendor();
		e.populate();
		e.validate();
		e.stickID = Joystick->getID();
		if (knowXbox)
			if (e.stickID == (unsigned int)xboxID)
				e.xbox = true;

		for (auto listener : listeners)
			listener->StickEvent(e);
	}

	for (auto listener : listeners)
		listener->tick();
}

timerID AnnEventManager::fireTimerMillisec(double delay)
{
	timerID newID = lastTimerCreated++;
	futureTimers.push_back(AnnTimer(newID, delay));
	return newID;
}

timerID AnnEventManager::fireTimer(double delay)
{
	return fireTimerMillisec(1000 * delay);
}

void AnnEventManager::processTimers()
{
	//This permit listeners to set timers without invalidating the iterator declared below
	if (!futureTimers.empty())
	{
		for (size_t i(0); i < futureTimers.size(); i++)
			activeTimers.push_back(futureTimers[i]);
		futureTimers.clear();
	}

	auto iterator = activeTimers.begin();
	while (iterator != activeTimers.end())
	{
		if ((*iterator).isTimeout())
		{
			AnnTimeEvent e;
			e.populate();
			e.setTimerID((*iterator).tID);
			e.validate();
			for (size_t i(0); i < listeners.size(); ++i)
				listeners[i]->TimeEvent(e);
			iterator = activeTimers.erase(iterator);
		}
		else
		{
			++iterator;
		}
	}
}

void AnnEventManager::processTriggerEvents()
{
	for (auto triggerIterator = triggerEventBuffer.begin(); triggerIterator != triggerEventBuffer.end(); triggerIterator++)
		for (auto listenerIterator = listeners.begin(); listenerIterator != listeners.end(); listenerIterator++)
		{
			(*triggerIterator).validate();
			(*listenerIterator)->TriggerEvent(*triggerIterator);
		}
	triggerEventBuffer.clear();
}

void AnnEventManager::spatialTrigger(std::shared_ptr<AnnTriggerObject> sender)
{
	AnnTriggerEvent e;
	e.sender = sender;
	e.contact = sender->getContactInformation();
	e.populate();
	triggerEventBuffer.push_back(e);
}

size_t AnnEventManager::getNbStick()
{
	return Joysticks.size();
}
