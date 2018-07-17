// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnEvents.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"

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
AnnKeyEvent::AnnKeyEvent() :
 AnnEvent(),
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
	setAxis(InvalidAxis);
	setRelValue(0);
	setAbsValue(0);
}

AnnMouseAxis::AnnMouseAxis(MouseAxisID ax, int rel, int abs)
{
	setAxis(ax);
	setRelValue(rel);
	setAbsValue(abs);
}

void AnnMouseAxis::setAxis(MouseAxisID ax)
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

MouseAxisID AnnMouseAxis::getMouseAxisId() const
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

AnnMouseEvent::AnnMouseEvent() :
 AnnEvent()
{
	for(size_t i(0); i < ButtonCount; i++)
		buttonsStatus[i] = false;

	type = USER_INPUT;
}

bool AnnMouseEvent::getButtonState(MouseButtonId id)
{
	if(id == InvalidButton) return false;

	if(int(id) < int(ButtonCount))
		return buttonsStatus[id];

	return false;
}

AnnMouseAxis AnnMouseEvent::getAxis(MouseAxisID id)
{
	if(id == InvalidAxis) return AnnMouseAxis(InvalidAxis, 0, 0);

	if(int(id) < int(AxisCount))
		return axes[id];

	return AnnMouseAxis(InvalidAxis, 0, 0);
}

void AnnMouseEvent::setButtonStatus(MouseButtonId id, bool value)
{
	if(int(id) < int(ButtonCount))
		buttonsStatus[id] = value;
}

void AnnMouseEvent::setAxisInformation(MouseAxisID id, AnnMouseAxis information)
{
	if(int(id) < int(AxisCount))
		axes[id] = information;
}

AnnControllerAxis::AnnControllerAxis()
{
	setAxis(InvalidStickAxisId);
	setRelValue(0);
	setAbsValue(0);
	noRel = true;
}

ControllerAxisID AnnControllerAxis::getAxisId() const
{
	return id;
}

AnnControllerAxis::AnnControllerAxis(ControllerAxisID ax, int rel, int abs)
{
	setAxis(ax);
	setRelValue(rel);
	setAbsValue(abs);
}

void AnnControllerAxis::setRelValue(int rel)
{
	r	 = rel;
	noRel = false;
}

void AnnControllerAxis::setAbsValue(int abs)
{
	a = abs;
}

void AnnControllerAxis::setAxis(ControllerAxisID ax)
{
	id = ax;
}

float AnnControllerAxis::getRelValue() const
{
	if(noRel) return INVALID;
	return float(r) / float(OIS::JoyStick::MAX_AXIS);
}

float AnnControllerAxis::getAbsValue() const
{
	return float(a) / float(OIS::JoyStick::MAX_AXIS);
}

AnnControllerPov::AnnControllerPov() :
 north(false),
 south(false),
 east(false),
 west(false)
{
}

bool AnnControllerPov::getNorth() const
{
	return north;
}

bool AnnControllerPov::getSouth() const
{
	return south;
}

bool AnnControllerPov::getEast() const
{
	return east;
}

bool AnnControllerPov::getWest() const
{
	return west;
}

bool AnnControllerPov::getNorthEast() const
{
	return north && east;
}

bool AnnControllerPov::getSouthEast() const
{
	return south && east;
}

bool AnnControllerPov::getNorthWest() const
{
	return north && west;
}

bool AnnControllerPov::getSouthWest() const
{
	return south && west;
}

bool AnnControllerPov::isCentred() const
{
	return !(north || south || east || west);
}

AnnControllerPov::AnnControllerPov(unsigned int binaryDirection) :
 north(false),
 south(false),
 east(false),
 west(false)
{
	switch(binaryDirection)
	{
		case OIS::Pov::North:
			north = true;
			break;

		case OIS::Pov::NorthEast:
			north = true;
			east  = true;
			break;

		case OIS::Pov::NorthWest:
			north = true;
			west  = true;
			break;

		case OIS::Pov::South:
			south = true;
			break;

		case OIS::Pov::SouthEast:
			south = true;
			east  = true;
			break;

		case OIS::Pov::SouthWest:
			south = true;
			west  = true;
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

AnnControllerEvent::AnnControllerEvent() :
 AnnEvent(),
 xbox(false),
 stickID(-1)
{
	type = USER_INPUT;
}

AnnControllerEvent::~AnnControllerEvent() {}

ControllerID AnnControllerEvent::getControllerID() const
{
	return stickID;
}

bool AnnControllerEvent::isDown(ButtonId id)
{
	if(id >= buttons.size()) return false;
	return buttons[id] != 0;
}

size_t AnnControllerEvent::getNbButtons() const
{
	return buttons.size();
}

std::vector<unsigned short> AnnControllerEvent::getPressed() const
{
	return pressed;
}

std::vector<unsigned short> AnnControllerEvent::getReleased() const
{
	return released;
}

AnnControllerAxis AnnControllerEvent::getAxis(ControllerAxisID ax)
{
	return axes[ax];
}

size_t AnnControllerEvent::getAxisCount() const
{
	return axes.size();
}

bool AnnControllerEvent::isPressed(ButtonId id)
{
	//if id is not a valid button
	if(id >= buttons.size()) return false;

	for(auto i(0); i < pressed.size(); i++)
		if(pressed[i] == id) return true;
	return false;
}

bool AnnControllerEvent::isReleased(ButtonId id)
{
	//if id is not a valid button
	if(id >= buttons.size()) return false;

	for(auto i(0); i < released.size(); i++)
		if(released[i] == id) return true;
	return false;
}

std::string AnnControllerEvent::getVendor() const
{
	return vendor;
}

AnnControllerPov AnnControllerEvent::getPov(PovId pov)
{
	if(pov < getPovCount())
		return povs[pov];
	AnnControllerPov p;
	return p;
}

bool AnnControllerEvent::isXboxController() const
{
	return xbox;
}

size_t AnnControllerEvent::getPovCount() const
{
	return povs.size();
}

AnnTimeEvent::AnnTimeEvent() :
 AnnEvent()
{
	type = TIMER_TIMEOUT;
	tID  = -1;
}

void AnnTimeEvent::setTimerID(AnnTimerID id)
{
	tID = id;
}

AnnCollisionEvent::AnnCollisionEvent(AnnGameObject* first, AnnGameObject* second, AnnVect3 position, AnnVect3 normal) :
 a{ first },
 b{ second },
 position{ position },
 normal{ normal }
{
	type = COLLISION;
}

bool AnnCollisionEvent::hasObject(AnnGameObject* obj) const
{
	if(obj == a || obj == b)
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

AnnVect3 AnnCollisionEvent::getPosition() const
{
	return position;
}

AnnVect3 AnnCollisionEvent::getNormal() const
{
	return normal;
}

bool AnnCollisionEvent::isGroundCollision(const float scalarApprox) const
{
	return !isWallCollision(scalarApprox) && normal.y > 0.0f;
}

bool AnnCollisionEvent::isCeilingCollision(const float scalarApprox) const
{
	return !isWallCollision(scalarApprox) && normal.y < 0.0f;
}

bool AnnCollisionEvent::isWallCollision(const float scalarApprox) const
{
	return Ogre::Math::RealEqual(AnnVect3::UNIT_Y.dotProduct(normal), 0, scalarApprox);
}

AnnPlayerCollisionEvent::AnnPlayerCollisionEvent(AnnGameObject* collided) :
 AnnEvent(),
 col{ collided }
{
	type = PLAYER_COLLISION;
}

AnnGameObject* AnnPlayerCollisionEvent::getObject() const
{
	return col;
}

AnnTimerID AnnTimeEvent::getID() const
{
	return tID;
}

AnnTimer::AnnTimer(AnnTimerID id, double delay)
{
	tID			= id;
	timeoutTime = delay + AnnGetEngine()->getTimeFromStartUp();
}

bool AnnTimer::isTimeout() const
{
	if(AnnGetEngine()->getTimeFromStartUp() >= timeoutTime)
		return true;
	return false;
}

AnnControllerBuffer::AnnControllerBuffer(OIS::JoyStick* joystick) :
 oisJoystick(joystick)
{
	id = idcounter++;
}

AnnControllerBuffer::~AnnControllerBuffer()
{
	AnnDebug() << "Deleted iosJoystick";
	AnnGetEventManager()->_getOISInputManager()->destroyInputObject(oisJoystick);
}

void AnnControllerBuffer::capture() const
{
	oisJoystick->capture();
}

AnnTriggerEvent::AnnTriggerEvent() :
 AnnEvent(),
 sender{ nullptr }
{
	type	= TRIGGER_CONTACT;
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

AnnHandControllerEvent::AnnHandControllerEvent(AnnHandController* controller) :
 AnnEvent(),
 controller{ controller }
{
	type = HAND_CONTROLLER;
}

AnnHandController* AnnHandControllerEvent::_getController() const
{
	return controller;
}

AnnTimeEvent::AnnTimeEvent(const AnnTimer& timer) :
 AnnEvent(),
 tID{ timer.getID() }
{
	type = TIMER_TIMEOUT;
}

AnnTimerID AnnTimer::getID() const
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

size_t AnnHandControllerEvent::getAxisCount() const
{
	return controller->getNbAxes();
}

size_t AnnHandControllerEvent::getButtonCount() const
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
