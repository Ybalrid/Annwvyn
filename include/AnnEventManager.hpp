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
		friend class AnnEventManager;
		///Code of the key this event relate to
		Annwvyn::KeyCode::code key;
		///Pressed state
		bool pressed;
		///Released state
		bool released;
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
		///Give access to private feilds to the EventManager
		friend class AnnEventManager;
		///Give acces to  private feilds to the MouseEvent class
		friend class AnnMouseEvent;
		///ID of the axis
		MouseAxisId id;
		///Relative value
		int rel;
		///Absolute value (if aplicable)
		int abs;

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
			///Get the ID if this axis
            StickAxisId getAxisId();
            ///Compute a float number between -1 and 1. if relative value isn't supported by the input, will return INVALID (42)
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
			///Number of buttons this controller has
            size_t getNbButtons();

            std::vector<unsigned short> getPressed();
            std::vector<unsigned short> getReleased();

			///Return true if this button just have been pressed
			bool isPressed(ButtonId id);
			///Return true if this buttton just have been released
			bool isReleased(ButtonId id);
			///Return true if this button is currently pressed
			bool isDown(ButtonId id);
			///Get the axis object for this ID
			AnnStickAxis getAxis(StickAxisId ax);
			///Get the number of axes the controller has
			size_t getNbAxis();
			///Get the unique ID given by Annwvyn for this stick
			unsigned int getStickID();
			///Get the "vendor string" of this joystick (could be its name)
			std::string getVendor();

        private:
        friend class AnnEventManager;
            std::vector<bool> buttons;
            std::vector<AnnStickAxis> axes;
            std::vector<unsigned short> pressed;
            std::vector<unsigned short> released;
            std::string vendor;
			unsigned int stickID;
	};

	typedef size_t timerID;

	class DLL AnnTimeEvent : public AnnEvent
	{
	public:
		AnnTimeEvent();
		///Get the ID of this timer
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
		///Return true if if there's collision
		bool getContactStatus();
		///Pointer to the trigger that have sent this event
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
		///Event from the keyboard
		virtual void KeyEvent(AnnKeyEvent e)			{return;}
		///Event from the mouse
		virtual void MouseEvent(AnnMouseEvent e)		{return;}
		///Event for a Joystick
		virtual void StickEvent(AnnStickEvent e)		{return;}
		///Event from a timer
		virtual void TimeEvent(AnnTimeEvent e)			{return;}
		///Event from a trigger
		virtual void TriggerEvent(AnnTriggerEvent e)	{return;}
		///This method is called at each frame. Usefull for updating player's movement command for example
		virtual void tick()								{return;}
		///Utility function for applying a deadzone on a joystick axis
		static float trim(float value, float deadzone);
	protected:
		///Pointer to the player. Set by the constructor, provide easy access to the AnnPlayer
		AnnPlayer* player;
	};

	///The default event listener that make WASD controlls move the player
	///The mouse turns the player's body
	///Shift to "run"
	///F1 and F2 to switch between Debug Mode
	///F12 to recenter the rift
	///² or ~ or ` (depending on keyboard layout) to open the on-screen-console
	///Xbox controller with main stick for walking and 2nd stick for turning your body
	class DLL AnnDefaultEventListener : public AnnAbstractEventListener
	{
	public:
		AnnDefaultEventListener();
		///Get events from keyboards
		void KeyEvent(AnnKeyEvent e);
		///Get events from the mouse
		void MouseEvent(AnnMouseEvent e);
		///GEt events from the joystick
		void StickEvent(AnnStickEvent e);
		
		///Set all the keycodes for the the controlls
		void setKeys(KeyCode::code fw, 
			KeyCode::code bw, 
			KeyCode::code sl, 
			KeyCode::code sr, 
			KeyCode::code jmp, 
			KeyCode::code rn); 

	protected:
		///W
		KeyCode::code forward;
		///S
		KeyCode::code backward;
		///A
		KeyCode::code straffleft;
		///D
		KeyCode::code straffright;
		///Space
		KeyCode::code jump;
		///shift
		KeyCode::code run;
		///F12
		KeyCode::code recenter;

		///value used for trimming low joysticks value
		float deadzone;
		///Axes
		enum {ax_walk, ax_straff, ax_rotate};
		StickAxisId axes[3];
		///Buttons
		enum {b_jump, b_run};
		ButtonId buttons[2];

	};
	
	///Internal utility class that represent a timer
	class DLL AnnTimer
	{
	private:
		friend class AnnEventManager;
		AnnTimer(timerID id, double delay);
		bool isTimeout();
		timerID tID; 
		double timeoutTime;
	};
	
	///Internal utility class that store joystick information
	class DLL JoystickBuffer
	{
	private:
		friend class AnnEventManager;
		///Private constructor for AnnEventManager
		///Create a Joystick buffer object, incremetns a static counter of IDs
		JoystickBuffer(OIS::JoyStick* joystick) : stick(joystick)
		{id = idcounter++;}

		///Delete the OIS stick at destruction time
		~JoystickBuffer()
		{delete stick;}

		///Joystick object from OIS
		OIS::JoyStick* stick;
		///Array of "bool" for previous buttons
		std::vector<bool> previousStickButtonStates;
		///Get the ID if this stick
		unsigned int getID()
		{return id;}
	private://mebmers
		///The ID
		unsigned int id;
		///The counter
		static unsigned int idcounter;
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
		/// \param listener A listener object. If NULL (default), it will remove every listener form the manager (see clearListenerList())
		void removeListener(AnnAbstractEventListener* listener = NULL);

		///Create a timer that will timeout after "delay" seconds
		timerID fireTimer(double delay);

		///Create a timer that will timeout after "delay" millisconds
		timerID fireTimerMillisec(double millisecDelay);

		///Get the number of available sticks
		size_t getNbStick();

	private:
		std::vector<AnnAbstractEventListener*> listeners;

		friend class AnnEngine;
		friend class AnnPhysicsEngine;
		///Engine call for refreshing the event system
		void update();
		void processInput();
		void processTimers();
		void processTriggerEvents();

		///Send event to all listeners
		void notifyListeners(AnnEvent e);

		///Register trigger event for next triggerProcess by the engine
		void spatialTrigger(AnnTriggerObject* sender);

		///OIS Event Manager
		OIS::InputManager *InputManager;

		///Pointer that holds the keyboard
		OIS::Keyboard* Keyboard;

		///Pointer that holds the Mouse
		OIS::Mouse* Mouse;

		///parameter list for OIS
		std::vector<JoystickBuffer*> Joysticks;

		OIS::ParamList pl;
		///Array for remembering the key states at last update. 
		bool previousKeyStates[static_cast<unsigned int>(KeyCode::SIZE)];

		///Array for remembering the button states at last update
		bool previousMouseButtonStates[static_cast<unsigned int>(MouseButtonId::nbButtons)];

	    ///Dinamicly sized array for remembering the joystick button state at last update
		timerID lastTimerCreated;

		std::vector<AnnTimer> activeTimers;
		std::vector<AnnTimer> futureTimers;
		std::vector<AnnTriggerEvent> triggerEventBuffer;
    };
}

#endif //ANNEVENTMANAGER
