#include "stdafx.h"
#include "AnnEvents.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnEvent::AnnEvent() :
	type(NO_TYPE)
{
}

AnnEventType AnnEvent::getType() const
{
	return type;
}

//---------------------------------------KEYBOARD
AnnKeyEvent::AnnKeyEvent() : AnnEvent(),
key(KeyCode::unassigned),
pressed(false),
ignored(false)
{
	type = USER_INPUT;
}

void AnnKeyEvent::setPressed()
{
	pressed = true;
}

void AnnKeyEvent::setReleased()
{
	pressed = false;
}

void AnnKeyEvent::setCode(KeyCode::code c)
{
	key = c;
}

bool AnnKeyEvent::isPressed() const
{
	return pressed;
}

bool AnnKeyEvent::isReleased() const
{
	return !pressed;
}

KeyCode::code AnnKeyEvent::getKey() const
{
	return key;
}

bool AnnKeyEvent::shouldIgnore() const
{
	return ignored;
}

//---------------------------------------MOUSE
AnnMouseAxis::AnnMouseAxis()
{
	setAxis(invalidAxis);
	setRelValue(0);
	setAbsValue(0);
}

AnnMouseAxis::AnnMouseAxis(MouseAxisId ax, int rel, int abs)
{
	setAxis(ax);
	setRelValue(rel);
	setAbsValue(abs);
}

void AnnMouseAxis::setAxis(MouseAxisId ax)
{
	id = ax;
}

void AnnMouseAxis::setRelValue(int v)
{
	rel = v;
}

void AnnMouseAxis::setAbsValue(int v)
{
	abs = v;
}

MouseAxisId AnnMouseAxis::getMouseAxisId() const
{
	return id;
}

int AnnMouseAxis::getRelValue() const
{
	return rel;
}

int AnnMouseAxis::getAbsValue() const
{
	return abs;
}

AnnMouseEvent::AnnMouseEvent() : AnnEvent()
{
	for (size_t i(0); i < nbButtons; i++)
		buttonsStatus[i] = false;

	type = USER_INPUT;
}

bool AnnMouseEvent::getButtonState(MouseButtonId id)
{
	if (id == invalidButton) return false;

	if (int(id) < int(nbButtons))
		return buttonsStatus[id];

	return false;
}

AnnMouseAxis AnnMouseEvent::getAxis(MouseAxisId id)
{
	if (id == invalidAxis) return AnnMouseAxis(invalidAxis, 0, 0);

	if (int(id) < int(nbAxes))
		return axes[id];

	return AnnMouseAxis(invalidAxis, 0, 0);
}

void AnnMouseEvent::setButtonStatus(MouseButtonId id, bool value)
{
	if (int(id) < int(nbButtons))
		buttonsStatus[id] = value;
}

void AnnMouseEvent::setAxisInformation(MouseAxisId id, AnnMouseAxis information)
{
	if (int(id) < int(nbAxes))
		axes[id] = information;
}

AnnStickAxis::AnnStickAxis()
{
	setAxis(InvalidStickAxisId);
	setRelValue(0);
	setAbsValue(0);
	noRel = true;
}

StickAxisId AnnStickAxis::getAxisId() const
{
	return id;
}

AnnStickAxis::AnnStickAxis(StickAxisId ax, int rel, int abs)
{
	setAxis(ax);
	setRelValue(rel);
	setAbsValue(abs);
}

void AnnStickAxis::setRelValue(int rel)
{
	r = rel;
}

void AnnStickAxis::setAbsValue(int abs)
{
	a = abs;
}

void AnnStickAxis::setAxis(StickAxisId ax)
{
	id = ax;
}

float AnnStickAxis::getRelValue() const
{
	if (noRel) return INVALID;
	return float(r) / float(OIS::JoyStick::MAX_AXIS);
}

float AnnStickAxis::getAbsValue() const
{
	return float(a) / float(OIS::JoyStick::MAX_AXIS);
}

AnnStickPov::AnnStickPov() :
	north(false),
	south(false),
	east(false),
	west(false)
{
}

bool AnnStickPov::getNorth() const
{
	return north;
}

bool AnnStickPov::getSouth() const
{
	return south;
}

bool AnnStickPov::getEast() const
{
	return east;
}

bool AnnStickPov::getWest() const
{
	return west;
}

bool AnnStickPov::getNorthEast() const
{
	return north && east;
}

bool AnnStickPov::getSouthEast() const
{
	return south && east;
}

bool AnnStickPov::getNorthWest() const
{
	return north && west;
}

bool AnnStickPov::getSouthWest() const
{
	return south && west;
}

bool AnnStickPov::isCentred() const
{
	return !(north || south || east || west);
}

AnnStickPov::AnnStickPov(unsigned int binaryDirection) :
	north(false),
	south(false),
	east(false),
	west(false)
{
	switch (binaryDirection)
	{
	case OIS::Pov::North:
		north = true;
		break;

	case OIS::Pov::NorthEast:
		north = true;
		east = true;
		break;

	case OIS::Pov::NorthWest:
		north = true;
		west = true;
		break;

	case OIS::Pov::South:
		south = true;
		break;

	case OIS::Pov::SouthEast:
		south = true;
		east = true;
		break;

	case OIS::Pov::SouthWest:
		south = true;
		west = true;
		break;

	case OIS::Pov::East:
		east = true;
		break;

	case OIS::Pov::West:
		west = true;
		break;

	case OIS::Pov::Centered:
	default:
		break;
	}
}

AnnStickEvent::AnnStickEvent() : AnnEvent(),
xbox(false),
stickID(-1)
{
	type = USER_INPUT;
}

AnnStickEvent::~AnnStickEvent() {}

unsigned int AnnStickEvent::getStickID() const
{
	return stickID;
}

bool AnnStickEvent::isDown(ButtonId id)
{
	if (id >= buttons.size()) return false;
	return buttons[id];
}

size_t AnnStickEvent::getNbButtons() const
{
	return buttons.size();
}

std::vector<unsigned short> AnnStickEvent::getPressed() const
{
	return pressed;
}

std::vector<unsigned short> AnnStickEvent::getReleased() const
{
	return released;
}

AnnStickAxis AnnStickEvent::getAxis(StickAxisId ax)
{
	return axes[ax];
}

size_t AnnStickEvent::getNbAxis() const
{
	return axes.size();
}

bool AnnStickEvent::isPressed(ButtonId id)
{
	//if id is not a valid button
	if (id >= buttons.size()) return false;

	for (auto i(0); i < pressed.size(); i++)
		if (pressed[i] == id) return true;
	return false;
}

bool AnnStickEvent::isReleased(ButtonId id)
{
	//if id is not a valid button
	if (id >= buttons.size()) return false;

	for (auto i(0); i < released.size(); i++)
		if (released[i] == id) return true;
	return false;
}

std::string AnnStickEvent::getVendor() const
{
	return vendor;
}

AnnStickPov AnnStickEvent::getPov(PovId pov)
{
	if (pov < getNbPov())
		return povs[pov];
	AnnStickPov p;
	return p;
}

bool AnnStickEvent::isXboxController() const
{
	return xbox;
}

size_t AnnStickEvent::getNbPov() const
{
	return povs.size();
}

AnnTimeEvent::AnnTimeEvent() : AnnEvent()
{
	type = TIMER_TIMEOUT;
	tID = -1;
}

void AnnTimeEvent::setTimerID(timerID id)
{
	tID = id;
}

AnnCollisionEvent::AnnCollisionEvent(AnnGameObject* first, AnnGameObject* second) : AnnEvent(),
a{ first },
b{ second }
{
	type = COLLISION;
}

bool AnnCollisionEvent::hasObject(AnnGameObject* obj) const
{
	if (obj == a || obj == b)
		return true;
	return false;
}

AnnGameObject* AnnCollisionEvent::getA() const
{
	return a;
}

AnnGameObject* AnnCollisionEvent::getB() const
{
	return b;
}

AnnPlayerCollisionEvent::AnnPlayerCollisionEvent(AnnGameObject* collided) : AnnEvent(),
col{ collided }
{
	type = PLAYER_COLLISION;
}

AnnGameObject* AnnPlayerCollisionEvent::getObject() const
{
	return col;
}

timerID AnnTimeEvent::getID() const
{
	return tID;
}

AnnTimer::AnnTimer(timerID id, double delay)
{
	tID = id;
	timeoutTime = delay + AnnGetEngine()->getTimeFromStartUp();
}

bool AnnTimer::isTimeout() const
{
	if (AnnGetEngine()->getTimeFromStartUp() >= timeoutTime)
		return true;
	return false;
}

JoystickBuffer::JoystickBuffer(OIS::JoyStick* joystick) : oisJoystick(joystick)
{
	id = idcounter++;
}

AnnTriggerEvent::AnnTriggerEvent() : AnnEvent(),
sender{ nullptr }
{
	type = TRIGGER_CONTACT;
	contact = false;
}

bool AnnTriggerEvent::getContactStatus() const
{
	return contact;
}

AnnTriggerObject* AnnTriggerEvent::getSender() const
{
	return sender;
}

AnnHandControllerEvent::AnnHandControllerEvent() :
	controller(nullptr)
{
	type = HAND_CONTROLLER;
}

AnnHandControllerEvent::AnnHandControllerEvent(AnnHandController* controller) : AnnEvent(),
controller{ controller }
{
	type = HAND_CONTROLLER;
}

AnnHandController* AnnHandControllerEvent::_getController() const
{
	return controller;
}

AnnTimeEvent::AnnTimeEvent(const AnnTimer& timer) : AnnEvent(),
tID{ timer.getID() }
{
	type = TIMER_TIMEOUT;
}

timerID AnnTimer::getID() const
{
	return tID;
}

AnnVect3 AnnHandControllerEvent::getPosition() const
{
	return controller->getWorldPosition();
}

AnnQuaternion AnnHandControllerEvent::getOrientation() const
{
	return controller->getWorldOrientation();
}

AnnVect3 AnnHandControllerEvent::getPointingDirection() const
{
	return controller->getPointingDirection();
}

AnnVect3 AnnHandControllerEvent::getLinearSpeed() const
{
	return controller->getLinearSpeed();
}

AnnVect3 AnnHandControllerEvent::getAngularSpeed() const
{
	return controller->getAngularSpeed();
}

AnnHandControllerAxis& AnnHandControllerEvent::getAxis(const uint8_t id) const
{
	return controller->getAxis(id);
}

size_t AnnHandControllerEvent::getNbAxes() const
{
	return controller->getNbAxes();
}

size_t AnnHandControllerEvent::getNbButton() const
{
	return controller->getNbButton();
}

bool AnnHandControllerEvent::buttonPressed(const uint8_t id) const
{
	return controller->hasBeenPressed(id);
}

bool AnnHandControllerEvent::buttonReleased(const uint8_t id) const
{
	return controller->hasBeenReleased(id);
}

bool AnnHandControllerEvent::buttonState(const uint8_t id) const
{
	return controller->getButtonState(id);
}

AnnHandController::AnnHandControllerSide AnnHandControllerEvent::getSide() const
{
	return controller->getSide();
}


AnnHandController::AnnHandControllerTypeHash AnnHandControllerEvent::getType() const
{
	return controller->getType();
}
