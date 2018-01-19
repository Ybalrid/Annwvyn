#pragma once

#include "systemMacro.h"
#include "AnnEvents.hpp"
#include "AnnEventManager.hpp"

namespace Annwvyn
{
	///Default event listener
	class AnnDllExport AnnDefaultEventListener : public AnnEventListener
	{
		//The default event listener that make WASD controls move the player
		//The mouse turns the player's body
		//Shift to "run"
		//F1 and F2 to switch between Debug Mode
		//F12 to recenter the rift
		//² or ~ or ` (depending on keyboard layout) to open the on-screen-console
		//Xbox controller with main stick for walking and 2nd stick for turning your body

	public:
		///Construct the default listener
		AnnDefaultEventListener();
		///Get events from keyboards
		void KeyEvent(AnnKeyEvent e) override;
		///Get events from the mouse
		void MouseEvent(AnnMouseEvent e) override;
		///Get events from the joystick
		void StickEvent(AnnStickEvent e) override;
		static void reclampDegreeToPositiveRange(float& degree);
		///Get events from an hand controller
		void HandControllerEvent(AnnHandControllerEvent e) override;

		///Set all the key-codes for the controls
		void setKeys(KeyCode::code fw,
			KeyCode::code bw,
			KeyCode::code sl,
			KeyCode::code sr,
			KeyCode::code jmp,
			KeyCode::code rn);
		enum turnStickMode { NORMAL, WHEEL };
		turnStickMode turnMode;

	protected:
		///W by default
		KeyCode::code forward;
		///S by default
		KeyCode::code backward;
		///A by default
		KeyCode::code straffleft;
		///D by default
		KeyCode::code straffright;
		///Space by default
		KeyCode::code jump;
		///shift by default
		KeyCode::code run;
		///F12 by default
		KeyCode::code recenter;

		///value used for trimming low joysticks value
		float deadzone;
		float wheelStickSensitivity;
		float maxWheelAngle, minWheelAngle;
		float stickCurrentAngleDegree;
		float computedWheelValue;
		///Axes
		enum { ax_walk, ax_straff, ax_rotate, ax_size };
		StickAxisId axes[ax_size];
		///Buttons
		enum { b_jump, b_run, b_console, b_debug, b_size };
		ButtonId buttons[b_size];

		float lastAngle;

		///Hash of controller type strings
		size_t OpenVRController, OculusTouchController;
	};
}
