#include "stdafx.h"
#include "AnnEventManager.hpp"
#include "AnnDefaultEventListener.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnEventListener::AnnEventListener() :
	player(AnnGetPlayer().get())
{
}

float AnnEventListener::trim(float v, float dz)
{
	//Compute absolute value of v
	auto abs(v);
	if (v < 0) abs = -v;

	//The test is done on the abs value. Return the actual value, or 0 if under the dead-zone
	if (abs >= dz) return v;
	return 0.0f;
}

std::shared_ptr<AnnEventListener> AnnEventListener::getSharedListener()
{
	return shared_from_this();
}

AnnTextInputer::AnnTextInputer() :
	listen(false),
	asciiOnly{ true }
{
}

bool AnnTextInputer::keyPressed(const OIS::KeyEvent &arg)
{
	if (!listen) return true;
	//If backspace, pop last char if possible
	if (arg.key == OIS::KC_BACK && !input.empty())
		input.pop_back();
	else if (arg.text < 127 && arg.text > 31 || arg.text == 13 || !asciiOnly)
		//Put typed char into the application
		input.push_back(char(arg.text));
	return true;
}

bool AnnTextInputer::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

std::string AnnTextInputer::getInput() const
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
lastTimerCreated(0),
defaultEventListener(nullptr),
knowXbox(false),
keyboardIgnore{ false }
{
	//Reserve some memory
	keyEventBuffer.reserve(10);
	mouseEventBuffer.reserve(10);
	stickEventBuffer.reserve(10);
	handControllerEventBuffer.reserve(10);

	//Init all bool array to false
	for (auto& keyState : previousKeyStates) keyState = false;
	for (auto& mouseButtonState : previousMouseButtonStates) mouseButtonState = false;

	//Configure and create the input system
	size_t windowHnd;
	w->getCustomAttribute("WINDOW", &windowHnd);
	pl.insert(make_pair(std::string("WINDOW"), std::to_string(windowHnd)));
	InputManager = OIS::InputManager::createInputSystem(pl);

	//Get the keyboard, mouse and joysticks objects
	Keyboard = static_cast<OIS::Keyboard*>(InputManager->createInputObject(OIS::OISKeyboard, true));
	Mouse = static_cast<OIS::Mouse*>(InputManager->createInputObject(OIS::OISMouse, true));
	for (auto nbStick(0); nbStick < InputManager->getNumberOfDevices(OIS::OISJoyStick); nbStick++)
	{
		//Create joystick object
		auto Joystick = static_cast<OIS::JoyStick*>(InputManager->createInputObject(OIS::OISJoyStick, true));
		Joysticks.push_back(new JoystickBuffer(Joystick));
		AnnDebug() << "Detected joystick : " << Joystick->vendor();

		//Test for the stick being an Xbox controller (Oculus, and PC in general uses Xbox as *standard* controller)
		if (Joystick->vendor().find("Xbox") != std::string::npos ||
			Joystick->vendor().find("XBOX") != std::string::npos)
		{
			knowXbox = true;
			xboxID = StickAxisId(Joystick->getID());
			AnnDebug() << "Detected Xbox controller at ID " << xboxID;
		}
	}

	textInputer = std::make_unique<AnnTextInputer>();
	Keyboard->setEventCallback(textInputer.get());
}

AnnTextInputer* AnnEventManager::getTextInputer() const
{
	return textInputer.get();
}

AnnEventManager::~AnnEventManager()
{
	clearListenerList();
	defaultEventListener = nullptr;
	Keyboard->setEventCallback(nullptr);
	delete Keyboard;
	delete Mouse;
	for (auto Joystick : Joysticks)
		delete Joystick;
	Joysticks.clear();
}

void AnnEventManager::useDefaultEventListener()
{
	AnnDebug("Reconfiguring the engine to use the default event listener");
	AnnDebug("This unregister any current listener in use!");

	//Remove all event listeners
	removeListener();

	//If the event listener isn't already initialized, allocate one
	if (!defaultEventListener)
		defaultEventListener = std::make_shared<AnnDefaultEventListener>();

	//Set the default event listener to the event manager
	addListener(defaultEventListener);
}

std::shared_ptr<AnnEventListener> AnnEventManager::getDefaultEventListener() const
{
	return defaultEventListener;
}

void AnnEventManager::addListener(std::shared_ptr<AnnEventListener> l)
{
	AnnDebug() << "Adding an event listener : " << l.get();
	if (l != nullptr)
		listeners.push_back(l);
}

void AnnEventManager::clearListenerList()
{
	listeners.clear();
}

//l equals NULL by default
void AnnEventManager::removeListener(std::shared_ptr<AnnEventListener> l)
{
	AnnDebug() << "Removing an event listener : " << l.get();
	if (l == nullptr) { clearListenerList(); return; }

	//TODO write this with better
	auto iterator = listeners.begin();
	while (iterator != listeners.end())
		if ((*iterator).lock() && (*iterator).lock().get() == l.get())
			iterator = listeners.erase(iterator);
		else ++iterator;
}

void AnnEventManager::update()
{
	processCollisionEvents();
	processInput();
	processTriggerEvents();
	processTimers();
	processUserSpaceEvents();
}

unsigned int JoystickBuffer::idcounter = 0;

void AnnEventManager::captureEvents()
{
	//Capture events
	Keyboard->capture();
	Mouse->capture();

	for (auto& joystick : Joysticks)
		joystick->stick->capture();
}

void AnnEventManager::processKeyboardEvents()
{
	//for each key of the keyboard
	for (size_t c(0); c < KeyCode::SIZE; c++)
		if (Keyboard->isKeyDown(OIS::KeyCode(c)) && !previousKeyStates[c] ||
			!Keyboard->isKeyDown(OIS::KeyCode(c)) && previousKeyStates[c])
		{
			//create a corresponding key event
			AnnKeyEvent e;
			e.setCode(KeyCode::code(c));
			e.ignored = keyboardIgnore;
			bool(previousKeyStates[c] = Keyboard->isKeyDown(OIS::KeyCode(c))) ? e.setPressed() : e.setReleased();

			keyEventBuffer.push_back(e);
		}
}

void AnnEventManager::processMouseEvents()
{
	auto state(Mouse->getMouseState());

	AnnMouseEvent e;

	for (size_t i(0); i < nbButtons; i++)
		e.setButtonStatus(MouseButtonId(i), state.buttonDown(OIS::MouseButtonID(i)));

	e.setAxisInformation(X, AnnMouseAxis(X, state.X.rel, state.X.abs));
	e.setAxisInformation(Y, AnnMouseAxis(Y, state.Y.rel, state.Y.abs));
	e.setAxisInformation(Z, AnnMouseAxis(Z, state.Z.rel, state.Z.abs));

	mouseEventBuffer.push_back(e);
}

void AnnEventManager::processJoystickEvents()
{
	for (auto Joystick : Joysticks)
	{
		const auto& state(Joystick->stick->getJoyStickState());
		AnnStickEvent e;

		//Get all buttons immediate data
		e.buttons = state.mButtons;

		//Get all axes immediate data
		for (auto i(0u); i < state.mAxes.size(); i++)
		{
			AnnStickAxis axis(i, state.mAxes[i].rel, state.mAxes[i].abs);
			if (state.mAxes[i].absOnly)
				axis.noRel = true;
			e.axes.push_back(axis);
		}

		//The joystick state object always have 4 Pov but the AnnStickEvent has the number of Pov the stick has
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
		e.stickID = Joystick->getID();
		if (knowXbox)
			if (e.stickID == xboxID)
				e.xbox = true;

		stickEventBuffer.push_back(e);
	}
}

void AnnEventManager::processHandControllerEvents()
{
	if (AnnGetVRRenderer()->handControllersAvailable())
	{
		for (auto handController : AnnGetVRRenderer()->getHandControllerArray())
		{
			if (!handController) continue;
			AnnHandControllerEvent e;
			e.sender = handController.get();

			handControllerEventBuffer.push_back(e);
		}
	}
}

void AnnEventManager::pushEventsToListeners()
{
	for (auto& weak_listener : listeners) if (auto listener = weak_listener.lock())
	{
		for (auto& e : keyEventBuffer) listener->KeyEvent(e);
		for (auto& e : mouseEventBuffer) listener->MouseEvent(e);
		for (auto& e : stickEventBuffer) listener->StickEvent(e);
		for (auto& e : handControllerEventBuffer) listener->HandControllerEvent(e);

		listener->tick();
	}

	keyEventBuffer.clear();
	mouseEventBuffer.clear();
	stickEventBuffer.clear();
	handControllerEventBuffer.clear();
}

void AnnEventManager::processInput()
{
	captureEvents();
	processKeyboardEvents();
	processMouseEvents();
	processJoystickEvents();
	processHandControllerEvents();
	pushEventsToListeners();
}

timerID AnnEventManager::fireTimerMillisec(double delay)
{
	auto newID = lastTimerCreated++;
	futureTimers.push_back(AnnTimer(newID, delay));
	return newID;
}

timerID AnnEventManager::fireTimer(double delay)
{
	return fireTimerMillisec(1000 * delay);
}

void AnnEventManager::processTimers()
{
	//Append timers
	for (const auto& futureTimer : futureTimers)
		activeTimers.push_back(futureTimer);
	futureTimers.clear();

	//Send events
	for (auto weak_listener : listeners) if (auto listener = weak_listener.lock())
		for (const auto& timer : activeTimers)
			if (timer.isTimeout()) listener->TimeEvent({ timer });

	//Cleanup
	std::remove_if(activeTimers.begin(), activeTimers.end(),
		[&](const AnnTimer& timer) {return timer.isTimeout(); });
}

void AnnEventManager::processTriggerEvents()
{
	for (const auto& triggerEvent : triggerEventBuffer)
		for (auto weakListener : listeners) if (auto listener = weakListener.lock())
			listener->TriggerEvent(triggerEvent);

	triggerEventBuffer.clear();
}

void AnnEventManager::processCollisionEvents()
{
	for (auto weakListener : listeners) if (auto listener = weakListener.lock())
	{
		for (auto& collisionPair : collisionBuffer)
		{
			const auto aMov = static_cast<AnnAbstractMovable*>(collisionPair.first);
			const auto bMov = static_cast<AnnAbstractMovable*>(collisionPair.second);
			if (auto a = dynamic_cast<AnnGameObject*>(aMov)) if (auto b = dynamic_cast<AnnGameObject*>(bMov))
			{
				AnnCollisionEvent e{ a, b };
				listener->CollisionEvent(e);
			}
		}

		for (auto playerCollision : playerCollisionBuffer)
		{
			AnnPlayerCollisionEvent e{ playerCollision };
			listener->PlayerCollisionEvent(e);
		}
	}

	collisionBuffer.clear();
	playerCollisionBuffer.clear();
}

size_t AnnEventManager::getNbStick() const
{
	return Joysticks.size();
}

AnnEventListener::~AnnEventListener()
{
}

void AnnEventManager::detectedCollision(void* a, void* b)
{
	//The only body that doesn't have an "userPointer" set is the Player's rigidbody.
	//If one of the pair is null, it's a player collision that has been detected on this manifold.
	//Not an object-object collision
	if (!a) return playerCollision(b);
	if (!b) return playerCollision(a);

	//push the object-object collision in the buffer
	collisionBuffer.push_back(std::make_pair(a, b));
}

void AnnEventManager::playerCollision(void* object)
{
	auto movable = static_cast<AnnAbstractMovable*>(object);
	if (auto gameObject = dynamic_cast<AnnGameObject*>(movable))
	{
		playerCollisionBuffer.push_back(gameObject);
	}
	else if (auto triggerObject = dynamic_cast<AnnTriggerObject*>(movable))
	{
		AnnTriggerEvent e;
		e.sender = triggerObject;
		e.contact = true;
		triggerEventBuffer.push_back(e);
	}
}

void AnnEventManager::keyboardUsedForText(bool state)
{
	keyboardIgnore = state;
}

void AnnEventManager::userSpaceDispatchEvent(std::shared_ptr<AnnUserSpaceEvent> e, AnnUserSpaceEventLauncher* l)
{
	userSpaceEventBuffer.push_back(make_pair(e, l));
}

void AnnEventManager::processUserSpaceEvents()
{
	for (auto userSpaceEvent : userSpaceEventBuffer)
	{
		for (auto weakListener : listeners) if (auto listener = weakListener.lock())
			listener->EventFromUserSubsystem(*userSpaceEvent.first, userSpaceEvent.second);
	}
	userSpaceEventBuffer.clear();
}