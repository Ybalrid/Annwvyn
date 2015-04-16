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
	axes[0] = 0;
	axes[1] = 1;
	//Use second analog stick for horizontal rotation
	axes[2] = 3;
	//Trim before 1st quarter of the stick
	deadzone = 1.0f/3.0f;
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
   /* //Display raw data:
    for(int i(0); i < e.getPressed().size(); i++)
        std::cout << "Button " << e.getPressed()[i] << " is pressed" << std::endl;
    for(int i(0); i < e.getRelased().size(); i++)
        std::cout << "Button " << e.getRelased()[i] << " is relased" << std::endl;
		*/
	if(e.getNbAxis() >= 4) //If we have 2 analog stick available
	{
		player->analogWalk = trim(e.getAxis(axes[0]).getAbsValue(), deadzone);
		player->analogStraff = trim(e.getAxis(axes[1]).getAbsValue(), deadzone);
		player->analogRotate = trim(e.getAxis(axes[2]).getAbsValue(), deadzone);
	}
	

}
