/**
 * \file AnnEventManager.hpp
 * \brief event management for Annwvyn
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANNEVENTMANAGER
#define ANNEVENTMANAGER

#include "systemMacro.h"

#include "AnnKeyCode.h"
#include "AnnPlayer.hpp"

#include <OIS.h>

namespace Annwvyn
{
	class AnnEngine;
	class AnnEventManager; //predeclaration of the event manager for class friendness directives

	///An input event
	class DLL AnnEvent
	{
	public:
		AnnEvent();
	protected:
		bool accepted;
		bool rejected;
		bool unpopulated;
		bool valid;

		friend class AnnEventManager;
		void validate();
		void populate();
	};

	///A keyboard event
	class DLL AnnKeyEvent : public AnnEvent
	{
		AnnKeyEvent();
	public:
		Annwvyn::KeyCode::code getKey();
		bool isPressed();
		bool isRelased();

	private:
		Annwvyn::KeyCode::code key;
		bool pressed;
		bool relased;
		
		friend class AnnEventManager;
		void setPressed();
		void setRelased();

		void setCode(KeyCode::code c);

	};

	///A mouse event
	class DLL AnnMouseEvent : public AnnEvent
	{
	};

	///A joystick event
	class DLL AnnStickEvent : public AnnEvent
	{
	};

	///Base Event listener class
	class DLL AnnAbstractEventListener 
	{
	public:
		AnnAbstractEventListener(AnnPlayer* p);
		virtual void KeyEvent(AnnKeyEvent e) = 0;
		virtual void MouseEvent(AnnMouseEvent e) = 0;
		virtual void StickEvent(AnnStickEvent e) = 0;
	private:
		AnnPlayer* player;
	};

	///The default event listener that make WASD controlls move the player
	class DLL AnnDefaultEventListener : public AnnAbstractEventListener
	{
	public:
		AnnDefaultEventListener(AnnPlayer* p);
		void KeyEvent(AnnKeyEvent e);
		void MouseEvent(AnnMouseEvent e);
		void StickEvent(AnnStickEvent e);

		void setKeys(KeyCode::code fw, KeyCode::code bw, KeyCode::code sl, KeyCode::code sr, KeyCode::code jmp, KeyCode::code rn); 

	private:
		KeyCode::code forward;
		KeyCode::code backward;
		KeyCode::code straffleft;
		KeyCode::code straffright;
		KeyCode::code jump;
		KeyCode::code run;
	};
		
	///The event manager
	class DLL AnnEventManager
	{
	public:
		///Construct the event manager
		AnnEventManager();
		///Destroy the event manager
		~AnnEventManager();

		///Set the listener object to the event manager.
		void setListener(AnnAbstractEventListener* callbackObject);
		
		///Make the event manager forget about the listener
		void removeListener();

	private:
		AnnAbstractEventListener* listener;
		friend class AnnEngine;
		///Engine call for refreshing the event system
		void update();

		///Engine initialization for keyboard events
		void setKeyboard(OIS::Keyboard* k);
		///Engine initialization for Mouse events
		void setMouse(OIS::Mouse* m);
		///Engine initialization for Joystick events 
		void setJoystick(OIS::JoyStick* stick);

		///Pointer that holds the keyboard
		OIS::Keyboard* Keyboard;
		///Pointer that holds the Mouse
		OIS::Mouse* Mouse;
		///Pointer that holds the stick
		OIS::JoyStick* Joystick;

		///Array for remembering the key states at last update. 
		bool previousKeyStates[static_cast<unsigned int>(KeyCode::SIZE)];
	};
}

#endif //ANNEVENTMANAGER
