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

//Include the Object-Oriented Input System library
#include <OIS.h>

///Macro for declaring a listener
#define LISTENER public Annwvyn::AnnAbstractEventListener
///Macro for declaring a listener constructor
#define constructListener(p) AnnAbstractEventListener(p) 

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
		///Class called by the event manager to tell that the event is valid (correctly constructed)
		void validate();
		///Method being called by the event manager to signifiy that the event object has been populated with data
		void populate();
	};

	///A keyboard event
	class DLL AnnKeyEvent : public AnnEvent
	{
		AnnKeyEvent();
	public:
		///Get the key involved in that event
		Annwvyn::KeyCode::code getKey();
		///Return true if it's a key press. Key event are debounced.
		bool isPressed();
		///Return ture if it's a key release. Key event are debounced.
		bool isReleased();

	private:
		Annwvyn::KeyCode::code key;
		bool pressed;
		bool released;
		
		friend class AnnEventManager;
		void setPressed();
		void setReleased();
		/// \param c Keycode
		void setCode(KeyCode::code c);

	};


	///Name and number of axes
	enum MouseAxisId {X, Y, Z, nbAxes, invalidAxis};
	///Name and number of mouse button
	enum MouseButtonId {Left, Right, Middle, Button3, Button4, Button5, Button6, Button7, nbButtons, invalidButton};

	///A mouse axis
	class DLL AnnMouseAxis
	{
	public:
		AnnMouseAxis();		
		MouseAxisId getMouseAxisId();
		int getRelValue();
		int getAbsValue();

	private:
		MouseAxisId id;
		int rel;
		int abs;

		friend class AnnEventManager;
		friend class AnnMouseEvent;
		void setAxis(MouseAxisId ax);
		void setRelValue(int rel); 
		void setAbsValue(int abs);
		AnnMouseAxis(MouseAxisId ax, int rel, int abs);
	};
	
	///A mouse event
	class DLL AnnMouseEvent : public AnnEvent
	{
	public:
		AnnMouseEvent();
		///Returns true if given butoon is pressed
		bool getButtonState(MouseButtonId id);
		///Get given axis data
		AnnMouseAxis getAxis(MouseAxisId id);

	private:
		AnnMouseAxis axes[MouseAxisId::nbAxes];
		bool buttonsStatus[MouseButtonId::nbButtons];

		friend class AnnEventManager;

		void setButtonStatus(MouseButtonId id, bool value);
		void setAxisInformation(MouseAxisId id, AnnMouseAxis information);
	};

	///A joystick event
    typedef size_t ButtonId;
    typedef size_t StickAxisId;
#define InvalidStickAxisId -1
#define INVALID 42.0f
    class DLL AnnStickAxis
    {
        public:
            AnnStickAxis();
            StickAxisId getAxisId();
            ///Compute a float number between -1 and 1. if relative value isn't supported by the input, will return INVALID
            float getRelValue();
            ///Compute a float number between -1 and 1
            float getAbsValue();
        private:
            int a, r; 
            StickAxisId id; 
            friend class AnnEventManager;
            friend class AnnStickEvent;
            void setAxis(StickAxisId ax);
            void setRelValue(int rel);
            void setAbsValue(int abs);
            AnnStickAxis(StickAxisId ax, int rel, int abs);
            bool noRel;

    };

	class DLL AnnStickEvent : public AnnEvent
	{
        public:
            AnnStickEvent();
            size_t getNbButtons();

            std::vector<unsigned short> getPressed();
            std::vector<unsigned short> getReleased();
			bool isPressed(ButtonId id);
			bool isReleased(ButtonId id);
			bool isDown(ButtonId id);


			AnnStickAxis getAxis(StickAxisId ax);
			size_t getNbAxis();

        private:
        friend class AnnEventManager;
            std::vector<bool> buttons;
            std::vector<AnnStickAxis> axes;
            std::vector<unsigned short> pressed;
            std::vector<unsigned short> released;
            std::string vendor;
	};

	///Base Event listener class
	class DLL AnnAbstractEventListener 
	{
	public:
		AnnAbstractEventListener(AnnPlayer* p);
		virtual void KeyEvent(AnnKeyEvent e) = 0;
		virtual void MouseEvent(AnnMouseEvent e) = 0;
		virtual void StickEvent(AnnStickEvent e) = 0;

		static float trim(float value, float deadzone);
	protected:
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

		void setKeys(KeyCode::code fw, 
			KeyCode::code bw, 
			KeyCode::code sl, 
			KeyCode::code sr, 
			KeyCode::code jmp, 
			KeyCode::code rn); 

	private:
		KeyCode::code forward;
		KeyCode::code backward;
		KeyCode::code straffleft;
		KeyCode::code straffright;
		KeyCode::code jump;
		KeyCode::code run;

		float deadzone;
		enum {ax_walk, ax_straff, ax_rotate};
		StickAxisId axes[3];
		enum {b_jump, b_run};
		ButtonId buttons[2];

	};
	
	///The event manager
	class DLL AnnEventManager
	{
	public:
		///Construct the event manager
		AnnEventManager(Ogre::RenderWindow* w);
		///Destroy the event manager
		~AnnEventManager();

		///Set the listener object to the event manager.
		/// \param callbackObject The instance of an event listener that will receive event informations
		void setListener(AnnAbstractEventListener* callbackObject);


		///Make the event manager forget about the listener
		void removeListener();

	private:
		AnnAbstractEventListener* listener;
		friend class AnnEngine;
		///Engine call for refreshing the event system
		void update();

		///OIS Event Manager
		OIS::InputManager *InputManager;
		///Pointer that holds the keyboard
		OIS::Keyboard* Keyboard;
		///Pointer that holds the Mouse
		OIS::Mouse* Mouse;
		///Pointer that holds the stick
		OIS::JoyStick* Joystick;
		///parameter list for OIS
		OIS::ParamList pl;


		///Array for remembering the key states at last update. 
		bool previousKeyStates[static_cast<unsigned int>(KeyCode::SIZE)];
		
		///Array for remembering the button states at last update
		bool previousMouseButtonStates[static_cast<unsigned int>(MouseButtonId::nbButtons)];
	    ///Dinamicly sized array for remembering the joystick button state at last update
        std::vector<bool> previousStickButtonStates;
    };
}

#endif //ANNEVENTMANAGER
