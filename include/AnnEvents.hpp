#pragma once

#include "systemMacro.h"
#include "AnnUserSpaceEvent.hpp"
#include "AnnKeyCode.h"
#include "AnnPlayerBody.hpp"
#include "AnnHandController.hpp"

namespace Annwvyn
{
	class AnnTimer;
	class AnnTriggerObject;

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
	class AnnDllExport AnnEvent
	{
	public:
		///Event constructor
		AnnEvent();
		AnnEventType getType() const;

	protected:
		AnnEventType type;
		friend class AnnEventManager;
	};

	///A keyboard event
	class AnnDllExport AnnKeyEvent : public AnnEvent
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
		///Keyboard event that should be ignored has this flag as "true"
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
	class AnnDllExport AnnMouseAxis
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
	class AnnDllExport AnnMouseEvent : public AnnEvent
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
	using ButtonId = int;
	using StickAxisId = int;
	using PovId = int;

	static constexpr StickAxisId InvalidStickAxisId = -1;
	static constexpr float INVALID = 42.0f;

	///A joystick axis
	class AnnDllExport AnnStickAxis
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

		///Raw values
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
		///True if the there's no "relative" value
		bool noRel;
	};

	///Represent a pad's POV controller
	class AnnDllExport AnnStickPov
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
	class AnnDllExport AnnStickEvent : public AnnEvent
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
		///Get the number of PoV controller on this one
		size_t getNbPov() const;
		///Get the PoV corresponding to this ID
		AnnStickPov getPov(PovId pov);

		///Return true if this event is from an Xbox controller
		bool isXboxController() const;

	private:
		///set to true if this is an Xbox controller (Xinput)
		bool xbox;
		friend class AnnEventManager;
		///Button array
		std::vector<bool> buttons;
		///Axis array
		std::vector<AnnStickAxis> axes;
		///Pov Array
		std::vector<AnnStickPov> povs;
		///Pressed event "queue"
		std::vector<unsigned short> pressed;
		///Released event "queue"
		std::vector<unsigned short> released;
		///Joystick "vendor" name (generally the brand and model)
		std::string vendor;
		///Joystick ID for the engine
		int stickID;
	};
	class AnnHandController;

	///A hand controller event
	class AnnDllExport AnnHandControllerEvent : public AnnEvent
	{
	public:
		AnnHandControllerEvent();
		AnnHandControllerEvent(AnnHandController* controller);

		AnnVect3 getPosition() const;
		AnnQuaternion getOrientation() const;
		AnnVect3 getPointingDirection() const;
		AnnVect3 getLinearSpeed() const;
		AnnVect3 getAngularSpeed() const;
		AnnHandControllerAxis& getAxis(const uint8_t id) const;
		size_t getNbAxes() const;
		size_t getNbButton() const;
		bool buttonPressed(const uint8_t id) const;
		bool buttonReleased(const uint8_t id) const;
		bool buttonState(const uint8_t id) const;

		AnnHandController::AnnHandControllerTypeHash getType() const;

		///advanced : get access to the hand controller this event is related to
		AnnHandController* _getController() const;
	private:
		friend class AnnEventManager;
		AnnHandController* controller;
	};

	using  timerID = int;

	///A timer timeout event
	class AnnDllExport AnnTimeEvent : public AnnEvent
	{
	public:
		///Create a timer timeout event
		AnnTimeEvent();

		AnnTimeEvent(const AnnTimer& timer);
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

	///Collision event between 2 game objects
	class AnnDllExport AnnCollisionEvent : public AnnEvent
	{
	public:
		///Event constructor
		AnnCollisionEvent(AnnGameObject* first, AnnGameObject* second);

		///Check if this event is about that object
		bool hasObject(AnnGameObject* obj) const;

		///Get first object
		AnnGameObject* getA() const;
		///Get second object
		AnnGameObject* getB() const;
	private:
		///Some naked pointers
		AnnGameObject *a, *b;
	};

	///Collision between the player and another object
	class AnnDllExport AnnPlayerCollisionEvent : public AnnEvent
	{
	public:
		///Constructor
		AnnPlayerCollisionEvent(AnnGameObject* collided);

		///Get the object this event is about
		AnnGameObject* getObject() const;

	private:
		///Naked pointer to the collider
		AnnGameObject* col;
	};

	///Trigger in/out event
	class AnnDllExport AnnTriggerEvent : public AnnEvent
	{
	public:
		///Construct a trigger in/out event
		AnnTriggerEvent();

		///Return true if if there's collision
		bool getContactStatus() const;

		///Pointer to the trigger that have sent this event
		AnnTriggerObject* getSender() const;
	private:
		friend class AnnEventManager;
		bool contact;
		AnnTriggerObject* sender;
	};

	///Base class for all event listener
	class AnnDllExport AnnEventListener : public std::enable_shared_from_this<AnnEventListener>
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
		///Pointer to the player. Set by the constructor, provide easy access to the AnnPlayerBody
		AnnPlayerBody* player;
	};

	using AnnEventListenerPtr = std::shared_ptr<AnnEventListener>;
	using AnnEventListenerWeakPtr = std::weak_ptr<AnnEventListener>;

	///Internal utility class that represent a timer
	class AnnDllExport AnnTimer
	{
	public:
		timerID getID() const;
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
	class AnnDllExport JoystickBuffer
	{
	private:
		friend class AnnEventManager;
		///Private constructor for AnnEventManager
		///Create a Joystick buffer object, increments a static counter of IDs
		JoystickBuffer(OIS::JoyStick* joystick);

		///Delete the OIS stick at destruction time
		~JoystickBuffer() { delete stick; }

		///Joystick object from OIS
		OIS::JoyStick* stick;

		///Array of "bool" for previous buttons
		std::vector<bool> previousStickButtonStates;

		///Get the ID if this stick
		unsigned int getID() const { return id; }

	private:
		///The ID
		unsigned int id;
		///The counter
		static unsigned int idcounter;
	};

	///This class permit to get text input from the keyboard
	class AnnDllExport AnnTextInputer : public OIS::KeyListener
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
		///true if this text should be ascii only
		bool asciiOnly;
	};
}
