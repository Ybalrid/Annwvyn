/**
* \file AnnEventManager.hpp
* \brief event management for Annwvyn
* \author A. Brainville (Ybalrid)
*/

#ifndef ANNEVENTMANAGER
#define ANNEVENTMANAGER

#include "systemMacro.h"

#include <array>
#include <memory>
#include <valarray>

//This also include OIS
#include "AnnKeyCode.h"
#include "AnnPlayer.hpp"
#include "AnnTriggerObject.hpp"

#include "AnnSubsystem.hpp"

//TODO if it breaks, just pre-declaer the class
#include "AnnUserSpaceSubSystem.hpp"

//the following two macros exist only for my "please, look nicer" side
///Macro for declaring a listener
#define LISTENER public Annwvyn::AnnEventListener
///Macro for declaring a listener constructor
#define constructListener() AnnEventListener()

namespace Annwvyn
{
	class AnnEngine;
	class AnnEventManager; //pre-declaration of the event manager for class friendliness directives

	class AnnUserSpaceEventLauncher;
	class AnnUserSpaceEvent;

	enum AnnEventType
	{
		NO_TYPE,
		USER_INPUT,
		TIMER_TIMEOUT,
		TRIGGER_CONTACT,
		HAND_CONTROLLER,
		COLLISION,
		PLAYER_COLLISION
	};
	///An input event
	class DLL AnnEvent
	{
	public:
		///Event constructor
		AnnEvent();
		AnnEventType getType() const;

	protected:
		bool accepted;
		bool rejected;
		bool unpopulated;
		bool valid;
		AnnEventType type;
		friend class AnnEventManager;
		///Class called by the event manager to tell that the event is valid (correctly constructed)
		void validate();
		///Method being called by the event manager to signify that the event object has been populated with data
		void populate();
	};

	///A keyboard event
	class DLL AnnKeyEvent : public AnnEvent
	{
		///Keyboard event constructor
		AnnKeyEvent();
	public:
		///Get the key involved in that event
		KeyCode::code getKey() const;

		///Return true if it's a key press. Key event are debounced.
		bool isPressed() const;

		///Return true if it's a key release. Key event are debounced.
		bool isReleased() const;

		///If this is true, it probably means that the keyboard is used for something else and that you should ignore this event.
		bool shouldIgnore() const;

	private:
		friend class AnnEventManager;
		///Code of the key this event relate to
		KeyCode::code key;
		///Pressed state
		bool pressed;
		///Released state
		bool released;

		bool ignored;
		///Set the event as a key release event
		void setPressed();
		///Set the event as a key press event
		void setReleased();
		///Set the keycode of the key
		/// \param c Keycode
		void setCode(KeyCode::code c);
	};

	///Name and number of axes
	enum MouseAxisId { X, Y, Z, nbAxes, invalidAxis };

	///Name and number of mouse button
	enum MouseButtonId { Left, Right, Middle, Button3, Button4, Button5, Button6, Button7, nbButtons, invalidButton };

	///A mouse axis information object
	class DLL AnnMouseAxis
	{
	public:
		///Construct a mouse axis information object
		AnnMouseAxis();
		///Return the id of the axis that object represent
		MouseAxisId getMouseAxisId() const;
		///Relative value in arbitrary unit
		int getRelValue() const;
		///Absolute value in arbitrary unit
		int getAbsValue() const;

	private:
		///Give access to private fields to the EventManager
		friend class AnnEventManager;
		///Give access to  private fields to the MouseEvent class
		friend class AnnMouseEvent;
		///ID of the axis
		MouseAxisId id;
		///Relative value
		int rel;
		///Absolute value (if applicable)
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

	///A mouse event information object
	class DLL AnnMouseEvent : public AnnEvent
	{
	public:
		AnnMouseEvent();
		///Returns true if given button is pressed
		/// \param id Id of the button
		bool getButtonState(MouseButtonId id);

		///Get given axis data
		/// \param id Id of the axis
		AnnMouseAxis getAxis(MouseAxisId id);

	private:
		AnnMouseAxis axes[nbAxes];
		bool buttonsStatus[nbButtons];

		friend class AnnEventManager;

		///Set the status of a button
		/// \param id Id of a specific button
		/// \param value Current pressed/released state of that button
		void setButtonStatus(MouseButtonId id, bool value);

		///Set the information about an axis
		/// \param id Id of a specific axis
		/// \param information The information object of the given axis
		void setAxisInformation(MouseAxisId id, AnnMouseAxis information);
	};

	///A joystick event
	typedef int ButtonId;
	typedef int StickAxisId;
	typedef int PovId;
#define InvalidStickAxisId -1
#define INVALID 42.0f

	///A joystick axis
	class DLL AnnStickAxis
	{
	public:
		///This constructor will produce an invalid stick axis object
		AnnStickAxis();
		///Get the ID if this axis
		StickAxisId getAxisId() const;
		///Compute a float number between -1 and 1. if relative value isn't supported by the input, will return INVALID (42)
		float getRelValue() const;
		///Compute a float number between -1 and 1
		float getAbsValue() const;

	private:
		int a, r;
		StickAxisId id;
		friend class AnnEventManager;
		friend class AnnStickEvent;
		///Set the ID of the axis
		void setAxis(StickAxisId ax);
		///Set a relative value
		void setRelValue(int rel);
		///Set an absolute value
		void setAbsValue(int abs);
		///Real constructor
		AnnStickAxis(StickAxisId ax, int rel, int abs);
		bool noRel;
	};

	///Represent a pad's POV controller
	class DLL AnnStickPov
	{
	public:
		///Construct a Pov with no direction pressed
		AnnStickPov();

		///Get the up (north) state
		bool getNorth() const;
		///Get the down (south) state
		bool getSouth() const;
		///Get the right (east) state
		bool getEast() const;
		///Get the left (west) state
		bool getWest() const;

		///Get the north && east state
		bool getNorthEast() const;
		///Get the south && east state
		bool getSouthEast() const;
		///Get the north && west state
		bool getNorthWest() const;
		///Get the south && west state
		bool getSouthWest() const;

		///Return true if nothing is pressed on the POV controller
		bool isCentred() const;

	private:
		///up
		bool north;
		///down
		bool south;
		///right
		bool east;
		///left
		bool west;

		friend class AnnEventManager;
		friend class AnnStickEvent;

		///Private constructor used by the event manager. Need a direction integer from OIS
		AnnStickPov(unsigned int binaryDirection);
	};

	///A joystick event
	class DLL AnnStickEvent : public AnnEvent
	{
	public:
		///Construct a stick event object
		AnnStickEvent();
		///Destroy a stick event object
		~AnnStickEvent();
		///Number of buttons this controller has
		size_t getNbButtons() const;

		///Get the list of pressed buttons
		std::vector<unsigned short> getPressed() const;
		///Get the list of released buttons
		std::vector<unsigned short> getReleased() const;

		///Return true if this button just have been pressed
		bool isPressed(ButtonId id);
		///Return true if this button just have been released
		bool isReleased(ButtonId id);
		///Return true if this button is currently pressed
		bool isDown(ButtonId id);
		///Get the axis object for this ID
		AnnStickAxis getAxis(StickAxisId ax);
		///Get the number of axes the controller has
		size_t getNbAxis() const;
		///Get the unique ID given by Annwvyn for this stick
		unsigned int getStickID() const;
		///Get the "vendor string" of this joystick (could be its name)
		std::string getVendor() const;

		size_t getNbPov() const;
		AnnStickPov getPov(PovId pov);

		///Return true if this event is from an Xbox controller
		bool isXboxController() const;

	private:
		bool xbox;
		friend class AnnEventManager;
		std::vector<bool> buttons;
		std::vector<AnnStickAxis> axes;
		std::vector<AnnStickPov> povs;
		std::vector<unsigned short> pressed;
		std::vector<unsigned short> released;
		std::string vendor;
		int stickID;
	};
	class AnnHandController;

	class DLL AnnHandControllerEvent : public AnnEvent
	{
	public:
		AnnHandControllerEvent();

		///get access to the hand controller this event is related to
		AnnHandController* getController() const;
	private:
		friend class AnnEventManager;
		AnnHandController* sender;
	};

	typedef int timerID;

	///A timer timeout event
	class DLL AnnTimeEvent : public AnnEvent
	{
	public:
		///Create a timer timeout event
		AnnTimeEvent();
		///Get the ID of this timer
		timerID getID() const;
	private:
		friend class AnnEventManager;
		///Set the ID of the timer
		void setTimerID(timerID id);
		///Timer ID
		timerID tID;
	};

	class AnnGameObject;

	class DLL AnnCollisionEvent : public AnnEvent
	{
	public:
		AnnCollisionEvent(AnnGameObject* first, AnnGameObject* second) : AnnEvent(),
			a{ first },
			b{ second }
		{
			type = COLLISION;
		}

		bool hasObject(AnnGameObject* obj) const
		{
			if (obj == a || obj == b)
				return true;
			return false;
		}

		AnnGameObject* getA() const { return a; };
		AnnGameObject* getB() const { return b; };
	private:
		AnnGameObject* a;
		AnnGameObject* b;
	};

	class DLL AnnPlayerCollisionEvent : public AnnEvent
	{
	public:
		AnnPlayerCollisionEvent(AnnGameObject* collided) : AnnEvent(),
			col{ collided }
		{
			type = PLAYER_COLLISION;
		}

		AnnGameObject* getObject() const
		{
			return col;
		}

	private:
		AnnGameObject* col;
	};

	///Trigger in/out event
	class DLL AnnTriggerEvent : public AnnEvent
	{
	public:
		///Construct a trigger in/out event
		AnnTriggerEvent();
		///Return true if if there's collision
		bool getContactStatus() const;
		///Pointer to the trigger that have sent this event

		// TODO don't use a shared ptr here

		AnnTriggerObject* getSender() const;
	private:
		friend class AnnEventManager;
		bool contact;
		AnnTriggerObject* sender;
	};

	///Base class for all event listener
	class DLL AnnEventListener : public std::enable_shared_from_this<AnnEventListener>
	{
		//Base Event listener class. Technically not abstract since it provides a default implementation for all
		//virtual members. But theses definitions are pointless because they actually don't do anything.
		//You need to subclass it to create an EventListener

	public:
		virtual ~AnnEventListener();

		///Construct a listener
		AnnEventListener();
		///Event from the keyboard
		virtual void KeyEvent(AnnKeyEvent e) { return; }
		///Event from the mouse
		virtual void MouseEvent(AnnMouseEvent e) { return; }
		///Event for a Joystick
		virtual void StickEvent(AnnStickEvent e) { return; }
		///Event from a timer
		virtual void TimeEvent(AnnTimeEvent e) { return; }
		///Event from a trigger
		virtual void TriggerEvent(AnnTriggerEvent e) { return; }
		///Event from an HandController
		virtual void HandControllerEvent(AnnHandControllerEvent e) { return; }
		///Event from detected collisions
		virtual void CollisionEvent(AnnCollisionEvent e) { return; }
		///Event from detected player collisions
		virtual void PlayerCollisionEvent(AnnPlayerCollisionEvent e) { return; }
		///Events from code outside of Annwvyn itself
		virtual void EventFromUserSubsystem(AnnUserSpaceEvent& e, AnnUserSpaceEventLauncher* origin) { return; }

		///This method is called at each frame. Useful for updating player's movement command for example
		virtual void tick() { return; }
		///Utility function for applying a dead-zone on a joystick axis
		static float trim(float value, float deadzone);
		///return a shared_ptr to this listener
		std::shared_ptr<AnnEventListener> getSharedListener();
	protected:
		///Pointer to the player. Set by the constructor, provide easy access to the AnnPlayer
		AnnPlayer* player;
	};

	///Default event listener
	class DLL AnnDefaultEventListener : public AnnEventListener
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

	///Internal utility class that represent a timer
	class DLL AnnTimer
	{
	private:
		friend class AnnEventManager;
		///Timer object for the EventMAnager
		AnnTimer(timerID id, double delay);
		///If timeout
		bool isTimeout() const;
		///Timeout ID
		timerID tID;
		///Time of timeout
		double timeoutTime;
	};

	///Internal utility class that store joystick information
	class DLL JoystickBuffer
	{
	private:
		friend class AnnEventManager;
		///Private constructor for AnnEventManager
		///Create a Joystick buffer object, increments a static counter of IDs
		JoystickBuffer(OIS::JoyStick* joystick) : stick(joystick) { id = idcounter++; }

		///Delete the OIS stick at destruction time
		~JoystickBuffer() { delete stick; }

		///Joystick object from OIS
		OIS::JoyStick* stick;

		///Array of "bool" for previous buttons
		std::vector<bool> previousStickButtonStates;

		///Get the ID if this stick
		unsigned int getID() const { return id; }

	private://members
		///The ID
		unsigned int id;
		///The counter
		static unsigned int idcounter;
	};

	///This class permit to get text input from the keyboard
	class DLL AnnTextInputer : public OIS::KeyListener
	{
	public:
		///Object for text input
		AnnTextInputer();
		///Callback key press method
		bool keyPressed(const OIS::KeyEvent &arg) override;
		///Callback key released method
		bool keyReleased(const OIS::KeyEvent &arg) override;
		///Return the "input" string object
		std::string getInput() const;
		///Permit you to change the content of the input method
		void setInput(std::string content);
		///Clear the input string : remove all characters hanging there
		void clearInput();
		///Clear input THEN record typed text
		void startListening();
		///Stop recording typed text
		void stopListening();
	private:
		///String that holds typed text. Characters are push/popped at the back of this string
		std::string input;
		///If set false, this class does nothing.
		bool listen;

		bool asciiOnly;
	};

	//The event manager handles all events that can occur during the gameplay loop. The private 'update()' method is called by
	//AnnEngine and provide the heartbeat for the event system.
	//Events can be user inputs or mostly anything else.
	//AnnEventManager creates AnnEvent (or subclass of AnnEvent) for each kind of event, populate that object with relevant event data
	//And propagate that event to any declared event listener.
	//Listeners should subclass AnnEventListener. A listener is registered when a pointer to it is passed as argument to the addListener() method.
	//You'll crash the engine if you destroy a listener without removing it from the EventManager (the EM will dereference an non-existing pointer)

	///Event Manager : Object that handle the event system
	class DLL AnnEventManager : public AnnSubSystem
	{
	public:
		///Construct the event manager
		AnnEventManager(Ogre::RenderWindow* w);

		///Destroy the event manager
		~AnnEventManager();

		///Set the engine to use the "default" event listener.
		///This will create an instance of AnnDefaultEventListener (if it doesn't already exist inside of AnnEngine)
		///This will also unregister all listeners known by AnnEventListener
		///The default event listener implement a simple "FPS-like" control scheme
		/// WASD for walking
		/// Horizontal view with mouse X relative movement
		/// That event listener is designed as an example of an event listener, and for exploring the environment without having to write a custom event listener
		void useDefaultEventListener();

		std::shared_ptr<AnnEventListener> getDefaultEventListener() const;

		///Ad a listener to the event manager
		/// \param listener Pointer to a listener object
		void addListener(std::shared_ptr<AnnEventListener> listener);

		///Remove every listener known from the EventManager.
		///This doesn't clear any memory
		void clearListenerList();

		///Make the event manager forget about the listener
		/// \param listener A listener object. If NULL (default), it will remove every listener form the manager (see clearListenerList())
		void removeListener(std::shared_ptr<AnnEventListener> listener = nullptr);

		///Create a timer that will timeout after "delay" seconds
		timerID fireTimer(double delay);

		///Create a timer that will timeout after "delay" milliseconds
		timerID fireTimerMillisec(double millisecDelay);

		///Get the number of available sticks
		size_t getNbStick() const;

		///Get the text inputer object
		AnnTextInputer* getTextInputer() const;

		void keyboardUsedForText(bool state = true);

	private:

		///List of pointer to the listeners.
		///The use of weak pointers permit to keep access to the listeners without having to own them.
		///This permit to use any classes of the engine (like levels) to be themselves event listener.
		std::vector<std::weak_ptr<AnnEventListener>> listeners;

		friend class AnnEngine;
		friend class AnnPhysicsEngine;
		friend class AnnUserSpaceEventLauncher;

		///Send the given event to the listeners
		void userSpaceDispatchEvent(std::shared_ptr<AnnUserSpaceEvent> e, AnnUserSpaceEventLauncher* sender);

		///Engine call for refreshing the event system
		void update() override;
		///Process user inputs
		void processInput();
		///Process timers
		void processTimers();
		///Process triggers
		void processTriggerEvents();
		///Process collisions
		void processCollisionEvents();
		///Process user event dispatch()
		void processUserSpaceEvents();

		// TODO get rid of the shared pointer here
		///Register trigger event for next triggerProcess by the engine
		void spatialTrigger(std::shared_ptr<AnnTriggerObject> sender);

		void detectedCollision(void* a, void* b);
		void playerCollision(void* object);

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
		std::array<bool, KeyCode::SIZE> previousKeyStates;

		///Array for remembering the button states at last update
		std::array<bool, nbButtons> previousMouseButtonStates;

		///Dynamically sized array for remembering the joystick button state at last update
		timerID lastTimerCreated;

		///List of timers
		std::vector<AnnTimer> activeTimers;
		///List of timer that will timeout in a future frame
		std::vector<AnnTimer> futureTimers;
		///List of trigger event to process
		std::vector<AnnTriggerEvent> triggerEventBuffer;
		///The text inputer object itself
		AnnTextInputer* textInputer;

		///Default event listener
		std::shared_ptr<AnnDefaultEventListener> defaultEventListener;

		std::vector<std::pair<void*, void*>> collisionBuffer;
		std::vector<AnnGameObject*> playerCollisionBuffer;

		//Using a shared ptr to keep ownership of the event object until the event is dealt with. Also, polymorphism.
		std::vector<std::pair<std::shared_ptr<AnnUserSpaceEvent>, AnnUserSpaceEventLauncher*>> userSpaceEventBuffer;

		StickAxisId xboxID;
		bool knowXbox;

		bool keyboardIgnore;
	};
}

#endif //ANNEVENTMANAGER