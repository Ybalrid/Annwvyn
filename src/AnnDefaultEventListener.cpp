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

		}
		if(e.getKey() == backward)
		{
		}
		if(e.getKey() == straffleft)
		{
		}
		if(e.getKey() == straffright)
		{
		}

	}
	else if(e.isRelased())
	{
		//modify linear speed vector
		if(e.getKey() == forward)
		{
		}
		if(e.getKey() == backward)
		{
		}
		if(e.getKey() == straffleft)
		{
		}
		if(e.getKey() == straffright)
		{
		}
	}
}

void AnnDefaultEventListener::MouseEvent(AnnMouseEvent e)
{
}

void AnnDefaultEventListener::StickEvent(AnnStickEvent e)
{
}