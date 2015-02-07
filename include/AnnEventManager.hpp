#ifndef ANNEVENTMANAGER
#define ANNEVENTMANAGER

#include "systemMacro.h"

#include "AnnKeyCode.h"

#include <OIS.h>

namespace Annwvyn
{
	class AnnEngine;
	class AnnEventManager; //predeclaration of the event manager for class friendness directives

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

	class DLL AnnMouseEvent : public AnnEvent
	{
	};

	class DLL AnnStickEvent : public AnnEvent
	{
	};

	class DLL AnnAbstractEventListener 
	{
	public:
		AnnAbstractEventListener();
		virtual void KeyEvent(AnnKeyEvent e) = 0;
		virtual void MouseEvent(AnnMouseEvent e) = 0;
		virtual void StickEvent(AnnStickEvent e) = 0;
	};

	class DLL AnnDefaultEventListener : public AnnAbstractEventListener
	{
	public:
		AnnDefaultEventListener();
		void KeyEvent(AnnKeyEvent e);
		void MouseEvent(AnnMouseEvent e);
		void StickEvent(AnnStickEvent e);
	};

	class DLL AnnEventManager
	{
	public:
		AnnEventManager();
		~AnnEventManager();

		void setListener(AnnAbstractEventListener* callbackObject);

	private:
		AnnAbstractEventListener* listener;
		friend class AnnEngine;
		void update();

		void setKeyboard(OIS::Keyboard* k);
		void setMouse(OIS::Mouse* m);
		void setJoystick(OIS::JoyStick* stick);

		OIS::Keyboard* Keyboard;
		OIS::Mouse* Mouse;
		OIS::JoyStick* Joystick;

		bool previousKeyStates[static_cast<unsigned int>(KeyCode::SIZE)];
	};
}

#endif //ANNEVENTMANAGER
