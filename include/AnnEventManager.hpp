/**
 * \file AnnEventManager.hpp
 * \brief event management for Annwvyn
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANNEVENTMANAGER
#define ANNEVENTMANAGER

#include "systemMacro.h"

//This also include OIS
#include "AnnKeyCode.h"
#include "AnnPlayer.hpp"
#include "AnnTriggerObject.hpp"

//the following two macros exist only for my "please, look nicer" side
///Macro for declaring a listener
#define LISTENER public Annwvyn::AnnAbstractEventListener
///Macro for declaring a listener constructor
#define constructListener() AnnAbstractEventListener() 

namespace Annwvyn
{
	class AnnEngine;
	class AnnEventManager; //predeclaration of the event manager for class friendness directives
	enum AnnEventType
	{
		USER_INPUT,
		TIMER_TIMEOUT,
		TRIGGER_CONTACT
	};
	///An input event
	class DLL AnnEvent
	{
	public:
		///Event constructor 
		AnnEvent();
		AnnEventType getType();

	protected:
		bool accepted;
		bool rejected;
		bool unpopulated;
		bool valid;
		AnnEventType type;
		friend class AnnEventManager;
		///Class called by the event manager to tell that the event is valid (correctly constructed)
		void validate();
		///Method being called by the event manager to signifiy that the event object has been populated with data
		void populate();
	};

	///A keyboard event
	class DLL AnnKeyEvent : public AnnEvent
	{
		///Keyboard event constructor
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
		
		///Set the event as a key relase event
		void setPressed();

		///Set the event as a key press event
		void setReleased();

		///Set the keycode of the key
		/// \param c Keycode
		void setCode(KeyCode::code c);

	};


	///Name and number of axes
	enum MouseAxisId {X, Y, Z, nbAxes, invalidAxis};

	///Name and number of mouse button
	enum MouseButtonId {Left, Right, Middle, Button3, Button4, Button5, Button6, Button7, nbButtons, invalidButton};

	///A mouse axis infomation object
	class DLL AnnMouseAxis
	{
	public:
		///Construct a mouse axis infomation object
		AnnMouseAxis();		

		///Return the id of the axis that object represent
		MouseAxisId getMouseAxisId();
		
		///Relative value in arbitrary unit
		int getRelValue();
		///Absolute value in arbitrary unit
		int getAbsValue();

	private:

		MouseAxisId id;
		int rel;
		int abs;

		friend class AnnEventManager;
		
		friend class AnnMouseEvent;
		///Set the id of the axis
		void setAxis(MouseAxisId ax);
		///Set the relative value of the axis
		void setRelValue(int rel); 
		///Set the absolute value of the axis
		void setAbsValue(int abs);
		///Private magic one line constructor !!!! ;-)
		AnnMouseAxis(MouseAxisId ax, int rel, int abs);
	};
	
	///A mouse event informaiton objecct
	class DLL AnnMouseEvent : public AnnEvent
	{
	public:
		AnnMouseEvent();
		///Returns true if given butoon is pressed
		/// \parm id Id of the button
		bool getButtonState(MouseButtonId id);
		///Get given axis data
		/// \param id Id of the axis
		AnnMouseAxis getAxis(MouseAxisId id);

	private:
		AnnMouseAxis axes[MouseAxisId::nbAxes];
		bool buttonsStatus[MouseButtonId::nbButtons];

		friend class AnnEventManager;

		///Set the status of a button
		/// \param id Id of a specific button
		/// \param value Current pressed/relased state of that button
		void setButtonStatus(MouseButtonId id, bool value);
		///Set the infomatio about an axis
		/// \param id Id of a specific axis
		/// \param information The information object of the given axis
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
			~AnnStickEvent();
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

	typedef size_t timerID;

	class DLL AnnTimeEvent : public AnnEvent
	{
	public:
		AnnTimeEvent();
		timerID getID();
	private:
		friend class AnnEventManager;
		void setTimerID(timerID id);
		timerID tID;
	};

	class DLL AnnTriggerEvent : public AnnEvent
	{
	public:
		AnnTriggerEvent();
		bool getContactStatus();
		AnnTriggerObject* getSender();
	private:
		friend class AnnEventManager;
		bool contact;
		AnnTriggerObject* sender;
	};

	///Base Event listener class. Technicaly not abstract since it provides a default implementation for all
	///virtual members. But theses definitions are pointless because they acutally don't do anything.
	///You need to subclass it to create an EventListener 
	class DLL AnnAbstractEventListener 
	{

	public:
		AnnAbstractEventListener();
		virtual void KeyEvent(AnnKeyEvent e)			{return;}
		virtual void MouseEvent(AnnMouseEvent e)		{return;}
		virtual void StickEvent(AnnStickEvent e)		{return;}
		virtual void TimeEvent(AnnTimeEvent e)			{return;}
		virtual void TriggerEvent(AnnTriggerEvent e)	{return;}

		///Called at each frame
		virtual void tick()								{return;}

		static float trim(float value, float deadzone);
	protected:
		AnnPlayer* player;
	};

	///The default event listener that make WASD controlls move the player
	class DLL AnnDefaultEventListener : public AnnAbstractEventListener
	{
	public:
		AnnDefaultEventListener();
		void KeyEvent(AnnKeyEvent e);
		void MouseEvent(AnnMouseEvent e);
		void StickEvent(AnnStickEvent e);

		void setKeys(KeyCode::code fw, 
			KeyCode::code bw, 
			KeyCode::code sl, 
			KeyCode::code sr, 
			KeyCode::code jmp, 
			KeyCode::code rn); 

	protected:
		KeyCode::code forward;
		KeyCode::code backward;
		KeyCode::code straffleft;
		KeyCode::code straffright;
		KeyCode::code jump;
		KeyCode::code run;
		KeyCode::code recenter;

		float deadzone;
		enum {ax_walk, ax_straff, ax_rotate};
		StickAxisId axes[3];

		enum {b_jump, b_run};
		ButtonId buttons[2];

	};
	
	
	class DLL AnnTimer
	{
	private:
		friend class AnnEventManager;
		AnnTimer(timerID id, double delay);
		bool isTimeout();
		timerID tID; 
		double timeoutTime;
	};
	
	
	///The event manager handles all events that can occur during the gameplay loop. The private 'update()' method is called by 
	///AnnEngine and provide the hearbeat for the event system.
	///Events can be user inputs or mostly anything else.
	///AnnEventManager creates AnnEvent (or subclass of AnnEvent) for each kind of event, populate that object with relevent envent data
	///And propagate that event to any declared event listener.
	///Listeners should subclass AnnEventListener. A listener is registred when a pointer to it is passed as argument to the addListener() method.
	///You'll crash the engine if you destroy a listener without removing it from the EventManager (the EM will dereference an non-existing pointer)

	class DLL AnnEventManager
	{
	public:
		///Construct the event manager
		AnnEventManager(Ogre::RenderWindow* w);
		///Destroy the event manager
		~AnnEventManager();

		///Ad a listener to the event manager
		/// \param listener Pointer to a listener object
		void addListener(AnnAbstractEventListener* listener);
		///Remove every listener known from the EventManager. 
		///This doesn't clear any memory
		void clearListenerList();
		///Make the event manager forget about the listener
		/// \param listener A listener object. If NULL, it will remove every listener form the manager
		void removeListener(AnnAbstractEventListener* listener = NULL);

		timerID fireTimer(double delay);

	private:
		std::vector<AnnAbstractEventListener*> listeners;

		friend class AnnEngine;
		friend class AnnPhysicsEngine;
		///Engine call for refreshing the event system
		void update();
		void processInput();
		void processTimers();
		void processTriggerEvents();

		void notifyListeners(AnnEvent e);

		void spatialTrigger(AnnTriggerObject* sender);

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
		timerID lastTimerCreated;
		std::vector<AnnTimer> activeTimers;
		std::vector<AnnTimer> futureTimers;
		std::vector<AnnTriggerEvent> triggerEventBuffer;
    };
}

#endif //ANNEVENTMANAGER
