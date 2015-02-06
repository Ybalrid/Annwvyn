#ifndef ANNEVENTMANAGER
#define ANNEVENTMANAGER

#include "systemMacro.h"

#include "AnnKeyCode.h"

#include <OIS.h>

namespace Annwvyn
{
	class AnnEngine;

	class DLL AnnEvent
	{
	protected:
		bool accepted;
		bool rejected;
		bool unpopulated;

		bool valid;
	};

	class DLL AnnMouseEvent : public AnnEvent
	{
	};

	class DLL AnnKeyEvent : public AnnEvent
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

	private:
		AnnAbstractEventListener* listener;
		friend class AnnEngine;
		void update();

		void setKeyboard(OIS::Keyboard* k);
		void setMouse(OIS::Mouse* m);
		void setJoystick(OIS::JoyStick* stick);

		OIS::Keyboard* Keyboard;
		OIS::Mouse* Mouse;
	};
}

#endif //ANNEVENTMANAGER
