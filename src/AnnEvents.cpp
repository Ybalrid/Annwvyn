#include "AnnEventManager.hpp"

using namespace Annwvyn;


//---------------------------------------KEYBOARD
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

//---------------------------------------MOUSE
AnnMouseAxis::AnnMouseAxis()
{
	setAxis(MouseAxisId::invalidAxis);
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

MouseAxisId AnnMouseAxis::getMouseAxisId()
{ 
	return id;
}

int AnnMouseAxis::getRelValue()
{
	return rel;
}

int AnnMouseAxis::getAbsValue()
{
	return abs;
}

AnnMouseEvent::AnnMouseEvent()
{
	for(size_t i(0); i < MouseButtonId::nbButtons; i++)
		buttonsStatus[i] = false;
}

bool AnnMouseEvent::getButtonState(MouseButtonId id)
{
	if(id == MouseButtonId::invalidButton) return false;
	
	if((int)id < (int)MouseButtonId::nbButtons)
		return buttonsStatus[id];

	return false;
}

AnnMouseAxis AnnMouseEvent::getAxis(MouseAxisId id)
{
	if(id == MouseAxisId::invalidAxis) return AnnMouseAxis(MouseAxisId::invalidAxis,0,0);

	if((int)id < (int)MouseAxisId::nbAxes)
		return axes[id];

	return AnnMouseAxis(MouseAxisId::invalidAxis,0,0);
}

void AnnMouseEvent::setButtonStatus(MouseButtonId id, bool value)
{
	if((int)id < (int)MouseButtonId::nbButtons)
		buttonsStatus[id]=value;
}

void AnnMouseEvent::setAxisInformation(MouseAxisId id, AnnMouseAxis information)
{
	if((int)id < (int)MouseAxisId::nbAxes)
		axes[id] = information;
}

AnnStickAxis::AnnStickAxis()
{
    setAxis(InvalidStickAxisId);
    setRelValue(0);
    setAbsValue(0);
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

float AnnStickAxis::getRelValue()
{
    if(noRel) return INVALID;
    return float(r)/float(OIS::JoyStick::MAX_AXIS);
}

float AnnStickAxis::getAbsValue()
{
    return float(a)/float(OIS::JoyStick::MAX_AXIS);
}

AnnStickEvent::AnnStickEvent() : AnnEvent()
{
    
}

bool AnnStickEvent::isDown(ButtonId id)
{
    if (id >= buttons.size()) return false;
    return buttons[id];
}

size_t AnnStickEvent::getNbButtons()
{
    return buttons.size();
}


std::vector<unsigned short> AnnStickEvent::getPressed()
{
    return pressed;
}

std::vector<unsigned short> AnnStickEvent::getRelased()
{
    return relased;
}

AnnStickAxis AnnStickEvent::getAxis(StickAxisId ax)
{
	return axes[ax];
}

size_t AnnStickEvent::getNbAxis()
{
	return axes.size();
}

bool AnnStickEvent::isPressed(ButtonId id)
{
	//if id is not a valid buton
	if (id >= buttons.size()) return false;

	for(ButtonId i(0); i < pressed.size();  i++)
		if(pressed[i] == id) return true;
	return false;
}

bool AnnStickEvent::isRelased(ButtonId id)
{
	//if id is not a valid buton
	if (id >= buttons.size()) return false;

	for(ButtonId i(0); i < relased.size();  i++)
		if(relased[i] == id) return true;
	return false;
}