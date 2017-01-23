#pragma once

#include <Ogre.h>
#include "systemMacro.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

//Forward declaration of the renderer classes that manager VR controllers
class OgreVRRender;
class OgreOpenVRRender;
class OgreOculusRender;
namespace Annwvyn
{
	///ID of an hand controller is the index of an array. using size_t s
	using AnnHandControllerID = size_t;

	///Represent the axis of an hand controller
	class DLL AnnHandControllerAxis
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
		friend class OgreVRRender;
		friend class OgreOpenVRRender;
		friend class OgreOculusRender;

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
	class DLL AnnHandController
	{
	public:
		virtual ~AnnHandController() = default;
		using AnnHandControllerTypeHash = size_t;
		///Identify the controller as "left hand", "right hand" or "invalid hand"
		enum AnnHandControllerSide : size_t { leftHandController = 0, rightHandController = 1, invalidHandController = 2 };

		///Construct a Controller object
		AnnHandController(std::string Type, Ogre::SceneNode* handNode, AnnHandControllerID controllerID, AnnHandControllerSide controllerSide);

		///Get the side type as a std::string
		static std::string getSideAsString(AnnHandControllerSide s);

		///Attach a 3D model to the hand. Previously attached model will be detached
		void attachModel(Ogre::Entity* handModel);

		///Detach model without destroying it
		void detachModel();

		///Return the current model :
		Ogre::Entity* getModel() const;

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

		///For test/branching, prefer using getTypeHash(). Get the type of the controller. Can be anything. Expect stuff like "Vive Controller" or "Oculus Touch Controller".
		std::string getType() const;

		///Get the hash of the type of the controller
		AnnHandControllerTypeHash getTypeHash() const;

		///Start vibrating the controller with a set amount of "strength". Result will vary between VR systems...
		virtual void rumbleStart(float factor);

		///Stop vibrating, if the controller was vibrating in the first place...
		virtual void rumbleStop();

	protected:

		friend class OgreVRRender;
		friend class OgreOpenVRRender;
		friend class OgreOculusRender;

		///Type of the controller, Can be string like "Vive controller" or "Oculus Touch Controller"
		std::string controllerType;

		///Hash of the type, see controllerType
		AnnHandControllerTypeHash controllerTypeHash;

		///Get a reference to the axes vector
		std::vector<AnnHandControllerAxis>& getAxesVector();

		///Get a reference to the button state vector
		std::vector<bool>& getButtonStateVector();

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
		Ogre::Entity* model;

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
		std::vector<bool> buttonsState;

		///buttons that has been pressed or released
		std::vector<uint8_t> pressedButtons, releasedButtons;
	};
}