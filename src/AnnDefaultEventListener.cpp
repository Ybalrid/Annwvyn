#include "AnnEventManager.hpp"
#include "AnnEngine.hpp"
using namespace Annwvyn;

AnnDefaultEventListener::AnnDefaultEventListener(AnnPlayer* p) : AnnAbstractEventListener(p),
	forward(KeyCode::w),
	backward(KeyCode::s),
	straffleft(KeyCode::a),
	straffright(KeyCode::d),
	jump(KeyCode::space),
	run(KeyCode::lshift)
{
	//Use 1st analog stick for displacement
	axes[ax_walk] = 0;
	axes[ax_straff] = 1;
	//Use second analog stick for horizontal rotation
	axes[ax_rotate] = 3;
	//Trim before 1st quarter of the stick
	deadzone = 1.0f/12.0f;

	buttons[b_run] = 2;
	buttons[b_jump] = 0;
}

void AnnDefaultEventListener::setKeys(KeyCode::code fw, 
									  KeyCode::code bw, 
									  KeyCode::code sl, 
									  KeyCode::code sr, 
									  KeyCode::code jmp, 
									  KeyCode::code rn)
{
	forward = fw;
	backward = bw;
	straffleft = sl;
	straffright = sr; 
	jump = jmp;
	run = rn;
}

//Each key press an release are a separated event. in contain a state "pressed" or "relased" and the Annwvyn keycode corespondng to the key.
//They are derivated from the OIS keycode list, see AnnKeyCode.h for the complete list.
//All keys are regarded as their equivalent on the american QWERTY layout, independently of the operating system behaviour.
//For compatibility purposes, it's possible that the engine has done a system call to set the current keyboard layout to the American QWERTY layout. 
//It will normally switch the keyboard back to it's original configuration once the AnnEngine object is destroyed
void AnnDefaultEventListener::KeyEvent(AnnKeyEvent e)
{
	//If the coresponding key is pressed, set the direction to true.
	if(e.getKey() == forward)
		player->walking[walkDirection::forward] = e.isPressed();
	if(e.getKey() == backward)
		player->walking[walkDirection::backward] = e.isPressed();
	if(e.getKey() == straffleft)
		player->walking[walkDirection::left] = e.isPressed();
	if(e.getKey() == straffright)
		player->walking[walkDirection::right] = e.isPressed();
	if(e.getKey() == run)
		player->run = e.isPressed();

	//Jumping is a function call because it's an action and not a "state" the player has. 
	if(e.isPressed() && e.getKey() == jump)
		player->jump();
}

//The mouse event contain all information about the mouse. Mouse mouvement are integer and are represented by 3 axis 
// X : horizontal mouvement to the right in pixels
// Y : Vertical mouvement, to the front in pixels
// Z : Scroll whell mouvement, scroll up is positive, in "line" increments
void AnnDefaultEventListener::MouseEvent(AnnMouseEvent e)
{
	player->applyMouseRelativeRotation(e.getAxis(MouseAxisId(X)).getRelValue());
}

//The stick event contain all the data for a specific joystick. In includes buttons current states, press and release events, stick relative and absolute values
void AnnDefaultEventListener::StickEvent(AnnStickEvent e)
{
	if(e.getNbAxis() >= 4) //If we have 2 analog stick (or equivalent) available
	{
		player->analogWalk = trim(e.getAxis(axes[ax_walk]).getAbsValue(), deadzone);
		player->analogStraff = trim(e.getAxis(axes[ax_straff]).getAbsValue(), deadzone);
		player->analogRotate = trim(e.getAxis(axes[ax_rotate]).getAbsValue(), deadzone);
	}
	if(e.isPressed(buttons[b_jump]))
		player->jump();
	if(e.isPressed(buttons[b_run]))
		player->run = true;
	if(e.isReleased(buttons[b_run]))
		player->run = false;
}
