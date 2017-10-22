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
	//The test is done on the abs value. Return the actual value, or 0 if under the dead-zone
	if (std::abs(v) >= dz) return v;
	return 0.0f;
}

std::shared_ptr<AnnEventListener> AnnEventListener::getSharedListener()
{
	return shared_from_this();
}

AnnTextInputer::AnnTextInputer() :
	listen(false),
	asciiOnly{ true },
	cursorOffset{ 0 }
{
}

bool AnnTextInputer::keyPressed(const OIS::KeyEvent &arg)
{
	if (!listen) return true;
	//If backspace, pop last char if possible
	if (arg.key == OIS::KC_BACK && !input.empty())
	{
		if (cursorOffset > input.size()) cursorOffset = int(input.size());
		if (!input.empty()) input.erase(end(input) - std::min<int>(input.size(), ((1 + (cursorOffset)))));
	}
	else if ((arg.text < 127 && arg.text > 31 && arg.text != '\r') || !asciiOnly)
		//Put typed char into the application
		input.insert(std::max(0, int(input.size()) - int(cursorOffset)), 1, char(arg.text));
	else if (arg.text == '\r')
		input.push_back('\r');
	else if (arg.key == OIS::KC_UP || arg.key == OIS::KC_DOWN || arg.key == OIS::KC_LEFT || arg.key == OIS::KC_RIGHT)
		AnnGetOnScreenConsole()->notifyNavigationKey(KeyCode::code(arg.key));
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
	cursorOffset = 0;
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

void AnnTextInputer::setInput(const std::string& content)
{
	input = content;
	cursorOffset = 0;
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
		const auto oisJoystick = static_cast<OIS::JoyStick*>(InputManager->createInputObject(OIS::OISJoyStick, true));
		Joysticks.emplace_back(oisJoystick);

		const auto& vendor = oisJoystick->vendor();
		AnnDebug() << "Detected joystick : " << vendor;

		//Test for the stick being an Xbox controller (Oculus, and PC in general uses Xbox as *standard* controller)
		if (vendor.find("Xbox") != std::string::npos ||
			vendor.find("XBOX") != std::string::npos)
		{
			knowXbox = true;
			xboxID = StickAxisId(oisJoystick->getID());
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

	listeners.erase(std::remove_if(begin(listeners), end(listeners),
		[&](std::weak_ptr<AnnEventListener> weak_listener)
	{
		if (auto listener = weak_listener.lock()) return listener == l;
		return false;
	}), end(listeners));
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
		joystick.oisJoystick->capture();
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
	for (auto& Joystick : Joysticks)
	{
		const auto& state(Joystick.oisJoystick->getJoyStickState());
		AnnStickEvent stickEvent;

		//Get all buttons immediate data
		//stickEvent.buttons = state.mButtons;

		const auto buttonSize = state.mButtons.size();
		stickEvent.buttons.resize(buttonSize);
		for (auto i = 0u; i < buttonSize; ++i)
		{
			if (state.mButtons[i])
				stickEvent.buttons[i] = 1;
			else
				stickEvent.buttons[i] = 0;
		}

		stickEvent.vendor = Joystick.oisJoystick->vendor();
		stickEvent.stickID = Joystick.getID();

		//Get all axes immediate data
		auto axisID = 0;
		for (const auto& axis : state.mAxes)
		{
			AnnStickAxis annAxis{ axisID++, axis.rel, axis.abs };
			annAxis.noRel = axis.absOnly;
			stickEvent.axes.push_back(annAxis);
		}

		//The joystick state object always have 4 Pov but the AnnStickEvent has the number of Pov the stick has
		const auto nbPov = size_t(Joystick.oisJoystick->getNumberOfComponents(OIS::ComponentType::OIS_POV));
		for (auto i(0u); i < nbPov; i++)
			stickEvent.povs.push_back({ unsigned(state.mPOV[i].direction) });

		//Get press and release event lists
		const auto nbButton{ std::min(state.mButtons.size(), Joystick.previousStickButtonStates.size()) };
		for (auto button(0u); button < nbButton; button++)
			if (!Joystick.previousStickButtonStates[button] && state.mButtons[button])
				stickEvent.pressed.push_back(static_cast<unsigned short>(button));
			else if (Joystick.previousStickButtonStates[button] && !state.mButtons[button])
				stickEvent.released.push_back(static_cast<unsigned short>(button));

		//Save current buttons state for next frame
		Joystick.previousStickButtonStates = stickEvent.buttons;
		if (knowXbox) if (stickEvent.stickID == xboxID)
			stickEvent.xbox = true;

		stickEventBuffer.push_back(stickEvent);
	}
}

void AnnEventManager::processHandControllerEvents()
{
	if (AnnGetVRRenderer()->handControllersAvailable())
		for (auto handController : AnnGetVRRenderer()->getHandControllerArray())
		{
			if (!handController) continue;
			handControllerEventBuffer.push_back({ handController.get() });
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
	activeTimers.erase(std::remove_if(begin(activeTimers), end(activeTimers),
		[&](const AnnTimer& timer) { return timer.isTimeout(); }), end(activeTimers));
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
		for (const auto& collisionBuffer : collisionBuffers)
		{
			const auto aMov = static_cast<AnnAbstractMovable*>(std::get<0>(collisionBuffer));
			const auto bMov = static_cast<AnnAbstractMovable*>(std::get<1>(collisionBuffer));
			const auto position = std::get<2>(collisionBuffer);
			const auto normal = std::get<3>(collisionBuffer);

			if (auto a = dynamic_cast<AnnGameObject*>(aMov)) if (auto b = dynamic_cast<AnnGameObject*>(bMov))
			{
				listener->CollisionEvent({ a, b, position, normal });
			}
		}

		for (auto playerCollision : playerCollisionBuffer)
			listener->PlayerCollisionEvent({ playerCollision });
	}

	collisionBuffers.clear();
	playerCollisionBuffer.clear();
}

size_t AnnEventManager::getNbStick() const
{
	return Joysticks.size();
}

AnnEventListener::~AnnEventListener()
{
}

void AnnEventManager::detectedCollision(void* a, void* b, AnnVect3 position, AnnVect3 normal)
{
	//The only body that doesn't have an "userPointer" set is the Player's rigidbody.
	//If one of the pair is null, it's a player collision that has been detected on this manifold.
	//Not an object-object collision
	if (!a) return playerCollision(b);
	if (!b) return playerCollision(a);

	//push the object-object collision in the buffer
	collisionBuffers.emplace_back(a, b, position, normal);
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
		for (auto weakListener : listeners) if (auto listener = weakListener.lock())
			listener->EventFromUserSubsystem(*userSpaceEvent.first, userSpaceEvent.second);
	userSpaceEventBuffer.clear();
}