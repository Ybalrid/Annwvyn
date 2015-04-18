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

void AnnDefaultEventListener::KeyEvent(AnnKeyEvent e)
{
	//TODO having a pointer to the AnnPlayer object would be nice...
	if(e.isPressed())
	{
		//modify linear speed vector
		if(e.getKey() == forward)
			player->walking[walkDirection::forward] = true;
		if(e.getKey() == backward)
			player->walking[walkDirection::backward] = true;
		if(e.getKey() == straffleft)
			player->walking[walkDirection::left] = true;
		if(e.getKey() == straffright)
			player->walking[walkDirection::right] = true;
        if(e.getKey() == jump)
            player->jump();
        if(e.getKey() == run)
            player->run = true;
	}

	else if(e.isRelased())
	{
		//modify linear speed vector
		if(e.getKey() == forward)
			player->walking[walkDirection::forward] = false;
		if(e.getKey() == backward)
			player->walking[walkDirection::backward] = false;
		if(e.getKey() == straffleft)
			player->walking[walkDirection::left] = false;
		if(e.getKey() == straffright)
			player->walking[walkDirection::right] = false;
        if(e.getKey() == run)
            player->run = false;
	}
}

void AnnDefaultEventListener::MouseEvent(AnnMouseEvent e)
{
	player->applyMouseRelativeRotation(e.getAxis(MouseAxisId(X)).getRelValue());
}

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
	if(e.isRelased(buttons[b_run]))
		player->run = false;
}
