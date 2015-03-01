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