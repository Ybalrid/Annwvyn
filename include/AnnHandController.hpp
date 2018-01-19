#pragma once

#include "systemMacro.h"

#include <Ogre.h>
#include <OgreItem.h>
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"
#include "AnnTypes.h"

namespace Annwvyn
{
	//Forward declaration of the renderer classes that manager VR controllers
	class AnnOgreVRRenderer;
	class AnnOgreOpenVRRenderer;
	class AnnOgreOculusRenderer;

	///ID of an hand controller is the index of an array. using size_t s
	using AnnHandControllerID = size_t;

	///Represent the axis of an hand controller
	class AnnDllExport AnnHandControllerAxis
	{
	public:

		/// \brief Create an AnnHandControllerAxis object
		/// \param AxisName Name of the axis
		/// \param normalizedValue initial value
		AnnHandControllerAxis(const std::string& AxisName, float normalizedValue);

		///Default copy constructor. We are just a float and a string
		AnnHandControllerAxis(const AnnHandControllerAxis& axis) = default;

		///Name of the axis
		std::string getName() const;

		///Analog value between -1 and 1. Some trigger inputs are from 0 to 1 only.
		float getValue() const;

	private:
		friend class AnnOgreVRRenderer;
		friend class AnnOgreOpenVRRenderer;
		friend class AnnOgreOculusRenderer;

		///Change the value of the string.
		void updateValue(float normalizedValue);

		///Return true if the value v is acceptable
		static bool isInRange(float v);

		///Name of the axis
		std::string name;

		///Value of the axis
		float value;
	};

	///Represent an hand controller tracked by the VR system and that the user is actively using. Contains position, orientation, buttons and analog inputs
	class AnnDllExport AnnHandController
	{
	public:

		///Destroctor
		virtual ~AnnHandController() = default;

		///Hash of a string idientifier
		using AnnHandControllerTypeHash = size_t;
		///Hash of a string identifier
		using AnnHandControllerGestureHash = size_t;

		///Individual finger values
		using proportionalFingerValues = std::array<float, 5>;

		///Identify the controller as "left hand", "right hand" or "invalid hand"
		enum AnnHandControllerSide : uint8_t { leftHandController = 0, rightHandController = 1, invalidHandController = 2 };

		///Fix the bitflag at 16bits wide
		using HandControllerCapabilites_t = uint16_t;

		///List of the capabilities that can be tested against
		enum HandControllerCapabilites : HandControllerCapabilites_t
		{
			None = 0,
			RotationalTracking = 0b0000000001,		//Can get the orientation of the user hand
			PositionalTracking = 0b0000000010,		//Can get the position of the user hand
			AngularAccelerationTracking = 0b0000000100,		//Can get the current angular acceleration
			LinearAccelerationTracking = 0b0000001000,		//Can get the current linear acceleration
			ButtonInputs = 0b0000010000,		//Can get inputs form buttons
			AnalogInputs = 0b0000100000,		//Can get inputs from axes
			HapticFeedback = 0b0001000000,		//Can produce haptic feedback
			DiscreteHandGestures = 0b0010000000,		//Can detect some hand gestures (Like the touch controller does)
			SkeletalFingerTracking = 0b0100000000,		//Can fully get the pose of an hand (Like on the LEAP Motion controller)
			ProportionalFingerTrackng = 0b1000000000,		//Can get a 5 axis analog representation of the fingers
		};

		///Construct a Controller object
		AnnHandController(const std::string& Type, Ogre::SceneNode* handNode, AnnHandControllerID controllerID, AnnHandControllerSide controllerSide);

		///Get the side type as a std::string
		static std::string getSideAsString(AnnHandControllerSide s);

		///Advanced method: Attach a 3D model to the hand. Previously attached model will be detached
		void _attachModelItem(Ogre::Item* handModel);

		///Set model by name. IF model already attached, model will be detached, and the item will be destroyed.
		void setHandModel(const std::string& name);

		///Detach model without destroying it
		void detachModel();

		///Return the current model :
		Ogre::Item* getHandModel() const;

		///Get position in world space
		AnnVect3 getWorldPosition() const;

		///Get orientation in world space
		AnnQuaternion getWorldOrientation() const;

		///Get tracked angular speed
		AnnVect3 getAngularSpeed() const;

		///Get tracked linear speed
		AnnVect3 getLinearSpeed() const;

		///Get a vector aligned with the pointing direction
		AnnVect3 getPointingDirection() const;

		///Attach the node as a child to the controller node
		void attachNode(Ogre::SceneNode* grabbedObject);

		///Set the position of the hand
		void setTrackedPosition(AnnVect3 position);

		///Set the orientation of the hand
		void setTrackedOrientation(AnnQuaternion orientation);

		///Set the linear velocity of the hand
		void setTrackedLinearSpeed(AnnVect3 v);

		///Set the angular velocity of the hand
		void setTrackedAngularSpeed(AnnVect3 v);

		///Check if controller is visible or not
		void updateVisibility() const;

		///Return true if the hand controller object has revived updates from the tracking system
		bool isTracked() const;

		///Get the state of the given button
		///\param buttonIndex index of the button to test
		bool getButtonState(uint8_t buttonIndex);

		///Return the number of buttons
		size_t getNbButton() const;

		///Return true if the given button has been pressed during the frame
		///\param buttonIndex index of the button to test
		bool hasBeenPressed(uint8_t buttonIndex);

		///Return false if the given button has been pressed during the frame
		///\param buttonIndex index of the button to test
		bool hasBeenReleased(uint8_t buttonIndex);

		///Return the number of analog axis this controller has.
		size_t getNbAxes() const;

		///Get the axis object for each axis
		///\param index Index of the axis you need
		AnnHandControllerAxis& getAxis(size_t index);

		///Get the "hand side" of this particular controller
		AnnHandControllerSide getSide() const;

		///For test/branching, prefer using getType(). Get the type of the controller. Can be anything. Expect stuff like "Vive Controller" or "Oculus Touch Controller".
		std::string getTypeString() const;

		///Get the hash of the type of the controller
		AnnHandControllerTypeHash getType() const;

		///Start vibrating the controller with a set amount of "strength". Result will vary between VR systems...
		virtual void rumbleStart(float factor);

		///Stop vibrating, if the controller was vibrating in the first place...
		virtual void rumbleStop();

		///Get the capabilities (bit test again the capability flags)
		HandControllerCapabilites_t getCapabilities() const;

		///Get the name of the current gesture. Please prefer use the hashed version.
		virtual std::string getCurrentGesture();

		///Get the hash of the current gesture
		virtual AnnHandControllerGestureHash gestcurrentGesturesHash();

	protected:

		///Capabilities of this controller
		HandControllerCapabilites_t capabilites;

		friend class AnnOgreVRRenderer;
		friend class AnnOgreOpenVRRenderer;
		friend class AnnOgreOculusRenderer;

		///Type of the controller, Can be string like "Vive controller" or "Oculus Touch Controller"
		std::string controllerTypeString;

		///Hash of the type, see controllerTypeString
		AnnHandControllerTypeHash controllerTypeHash;

		///Get a reference to the axes vector
		std::vector<AnnHandControllerAxis>& getAxesVector();

		///Get a reference to the button state vector
		std::vector<byte>& getButtonStateVector();

		///Get a reference to the pressed event vector
		std::vector<uint8_t>& getPressedButtonsVector();

		///Get a reference to the released event vector
		std::vector<uint8_t>& getReleasedButtonsVector();

		///ID of the controller, expect 1 or 2
		AnnHandControllerID id;

		///Side of the controller, right hand? left hand? Who knows!
		AnnHandControllerSide side;

		///Some Ogre Scene Nodes
		Ogre::SceneNode* node, *grabbed;

		///Currently attached entity
		Ogre::Item* model;

		///tracked boolean, true if controller has been updated by the engine
		bool tracked;

		///Angular velocity (Euler?) vector
		AnnVect3 trackedAngularSpeed;

		///Linear velocity vector
		AnnVect3 trackedLinearSpeed;

		///Axes vector
		std::vector<AnnHandControllerAxis> axes;

		///An invalid one to return a reference to if we can't return a valid axis
		AnnHandControllerAxis invalidAxis;

		///An array of the buttons states
		std::vector<byte> buttonsState;

		///buttons that has been pressed or released
		std::vector<uint8_t> pressedButtons, releasedButtons;

		///The hash of the "N/A" string
		const AnnHandControllerGestureHash gestureNotAvailableHash;

		///Permanently set to "N/A"
		static constexpr const char* const gestureNotAvailableString{ "N/A" };
	};

	using AnnHandControllerPtr = std::shared_ptr<AnnHandController>;
}
