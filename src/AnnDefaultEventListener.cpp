#include "AnnEventManager.hpp"

using namespace Annwvyn;

AnnDefaultEventListener::AnnDefaultEventListener(AnnPlayer* p) : AnnAbstractEventListener(p),
	forward(KeyCode::w),
	backward(KeyCode::s),
	straffleft(KeyCode::a),
	straffright(KeyCode::d),
	jump(KeyCode::space),
	run(KeyCode::lshift)
{
	//Use WASD, SPACE and LEFT_SHIFT fot the controll
	setKeys(KeyCode::w,
		KeyCode::s,
		KeyCode::a,
		KeyCode::d,
		KeyCode::space,
		KeyCode::lshift);
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
		{
			player->walking[AnnPlayer::walkDirection::forward] = true;
		}
		if(e.getKey() == backward)
		{
			player->walking[AnnPlayer::walkDirection::backward] = true;
		}
		if(e.getKey() == straffleft)
		{
			player->walking[AnnPlayer::walkDirection::left] = true;
		}
		if(e.getKey() == straffright)
		{
			player->walking[AnnPlayer::walkDirection::right] = true;
		}
        if(e.getKey() == jump)
        {
            player->jump();
        }
        if(e.getKey() == run)
        {
            player->run = true;
        }
	}

	else if(e.isRelased())
	{
		//modify linear speed vector
		if(e.getKey() == forward)
		{
			player->walking[AnnPlayer::walkDirection::forward] = false;
		}
		if(e.getKey() == backward)
		{
			player->walking[AnnPlayer::walkDirection::backward] = false;
		}
		if(e.getKey() == straffleft)
		{
			player->walking[AnnPlayer::walkDirection::left] = false;
		}
		if(e.getKey() == straffright)
		{
			player->walking[AnnPlayer::walkDirection::right] = false;
		}
        if(e.getKey() == jump)
        {
        }
        if(e.getKey() == run)
        {
            player->run = false;
        }
	}
}

void AnnDefaultEventListener::MouseEvent(AnnMouseEvent e)
{
	player->applyRelativeBodyYaw(Ogre::Radian(float(-e.getAxis(MouseAxisId::X).getRelValue())*player->getTurnSpeed()));
}

void AnnDefaultEventListener::StickEvent(AnnStickEvent e)
{
}

