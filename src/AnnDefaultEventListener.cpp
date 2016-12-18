#include "stdafx.h"
#include "AnnEventManager.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnDefaultEventListener::AnnDefaultEventListener() : AnnEventListener(),
forward(KeyCode::w),
backward(KeyCode::s),
straffleft(KeyCode::a),
straffright(KeyCode::d),
jump(KeyCode::space),
run(KeyCode::lshift),
recenter(KeyCode::f12),
deadzone{ 1 / 10 },
wheelStickSensitivity{ 6.0f / 5.0f },
maxWheelAngle{ 10 },
minWheelAngle{ 0.25f },
stickCurrentAngleDegree{ 0 },
computedWheelValue{ 0 },
turnMode{ WHEEL },
lastAngle{ 0 }
{
	//Use 1st analog stick for displacement
	axes[ax_walk] = 0;
	axes[ax_straff] = 1;
	//Use second analog stick for horizontal rotation
	axes[ax_rotate] = 3;

	buttons[b_run] = 2;
	buttons[b_jump] = 0;
	buttons[b_console] = 7;
	buttons[b_debug] = 6;
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

//Each key press an release are a separated event. in contain a state "pressed" or "released" and the Annwvyn keycode corresponding to the key.
//They are derivate from the OIS keycode list, see AnnKeyCode.h for the complete list.
//All keys are regarded as their equivalent on the American QWERTY layout, independently of the operating system behavior.
//For compatibility purposes, it's possible that the engine has done a system call to set the current keyboard layout to the American QWERTY layout.
//It will normally switch the keyboard back to it's original configuration once the AnnEngine object is destroyed
void AnnDefaultEventListener::KeyEvent(AnnKeyEvent e)
{
	//If the corresponding key is pressed, set the direction to true.
	if (e.getKey() == forward)
		player->walking[walkDirection::forward] = e.isPressed();
	if (e.getKey() == backward)
		player->walking[walkDirection::backward] = e.isPressed();
	if (e.getKey() == straffleft)
		player->walking[left] = e.isPressed();
	if (e.getKey() == straffright)
		player->walking[right] = e.isPressed();
	if (e.getKey() == run)
		player->run = e.isPressed();

	//Jumping is a function call because it's an action and not a "state" the player has.
	if (e.isPressed() && e.getKey() == recenter)
		AnnGetVRRenderer()->recenter();

	if (e.isPressed()) switch (e.getKey())
	{
		case KeyCode::grave:
			AnnGetEngine()->toogleOnScreenConsole();
			break;
		case KeyCode::tab:
			AnnGetVRRenderer()->cycleDebugHud();
			break;
		case KeyCode::f1:
			AnnGetVRRenderer()->showDebug(OgreVRRender::RAW_BUFFER);
			break;
		case KeyCode::f2:
			AnnGetVRRenderer()->showDebug(OgreVRRender::HMD_MIRROR);
			break;
		case KeyCode::f3:
			AnnGetVRRenderer()->showDebug(OgreVRRender::MONOSCOPIC);
			break;
		case KeyCode::f5:
			AnnGetPhysicsEngine()->toggleDebugPhysics();
		default: break;
	}
}

//The mouse event contain all information about the mouse. Mouse movement are integer and are represented by 3 axis
// X : horizontal movement to the right in pixels
// Y : Vertical movement, to the front in pixels
// Z : Scroll wheel movement, scroll up is positive, in "line" increments
void AnnDefaultEventListener::MouseEvent(AnnMouseEvent e)
{
	player->applyMouseRelativeRotation(e.getAxis(MouseAxisId(X)).getRelValue());
}

//The stick event contain all the data for a specific joystick. In includes buttons current states, press and release events, stick relative and absolute values
void AnnDefaultEventListener::StickEvent(AnnStickEvent e)
{
	if (!e.isXboxController()) return;
	if (e.getNbAxis() >= 4) //If we have 2 analog stick (or equivalent) available
	{
		player->analogWalk = trim(e.getAxis(axes[ax_walk]).getAbsValue(), deadzone);
		player->analogStraff = trim(e.getAxis(axes[ax_straff]).getAbsValue(), deadzone);

		player->analogRotate = trim(e.getAxis(axes[ax_rotate]).getAbsValue(), deadzone);
	}
	if (e.isPressed(buttons[b_run]))
		player->run = true;
	if (e.isReleased(buttons[b_run]))
		player->run = false;

	if (e.isPressed(buttons[b_console]))
		AnnGetEngine()->toogleOnScreenConsole();
	if (e.isPressed(buttons[b_debug]))
		AnnGetVRRenderer()->cycleDebugHud();
}

void AnnDefaultEventListener::reclampDegreeToPositiveRange(float& degree)
{
	if (degree < 0)
		degree += 360.0f;
}

void AnnDefaultEventListener::HandControllerEvent(AnnHandControllerEvent e)
{
	auto controller = e.getController();
	switch (controller->getSide())
	{
		default:break;
		case AnnHandController::leftHandController:
		{
			player->analogStraff = controller->getAxis(0).getValue();
			player->analogWalk = -controller->getAxis(1).getValue();

			//TODO use an hashing system to prevent string compare here
			if (controller->hasBeenPressed(2))
			{
				if (controller->getType() == "Oculus Touch")
					AnnGetVRRenderer()->recenter();
			}
			break;
		}
		case AnnHandController::rightHandController:
		{
			switch (turnMode)
			{
				default: case WHEEL:
					//If we take the stick values as coordinate in the trigonometric plan, this will give the angle
					stickCurrentAngleDegree = AnnRadian(std::atan2(controller->getAxis(1).getValue(), controller->getAxis(0).getValue())).valueDegrees();
					//Change range from [-180; +180] to [0; 360]
					reclampDegreeToPositiveRange(stickCurrentAngleDegree);

					//Detect the relative angle between 2 frames
					computedWheelValue = lastAngle - stickCurrentAngleDegree;
					//If value is too high it's either that you completed a full turn or there's a glitch in the input data, ignore.
					if (stickCurrentAngleDegree == 0 ||
						std::abs(computedWheelValue) > maxWheelAngle ||
						std::abs(computedWheelValue) < minWheelAngle)
						computedWheelValue = 0;

					player->analogRotate = wheelStickSensitivity * computedWheelValue;
					lastAngle = stickCurrentAngleDegree;
					break;

				case NORMAL:
					player->analogRotate = controller->getAxis(0).getValue();
			}
			break;
		}
	}
}