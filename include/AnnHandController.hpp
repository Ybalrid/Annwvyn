#pragma once

#include <Ogre.h>
#include "systemMacro.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

//Forward declaration of the renderer classes that manager VR controllers
class OgreVRRender;
class OgreOpenVRRender;
namespace Annwvyn
{
	typedef size_t AnnHandControllerID;

	class AnnHandControllerAxis
	{
	public:
		AnnHandControllerAxis(const std::string& AxisName, float normalizedValue) :
			name(AxisName),
			value(0)
		{
			updateValue(normalizedValue);
		}

		AnnHandControllerAxis(const AnnHandControllerAxis& axis) = default;

		const std::string getName() const
		{
			return name;
		}

		const float getValue() const
		{
			return value;
		}
	private:
		friend class OgreVRRender;
		friend class OgreOpenVRRender;

		void updateValue(float normalizedValue)
		{
			if (isInRange(normalizedValue))
				value = normalizedValue;
		}

		const bool isInRange(float v) const
		{
			return (v >= -1 && v <= 1);
		}

		std::string name;
		float value;
	};

	class DLL AnnHandController
	{
	public:
		///Identify the controller as "left hand", "right hand" or "invalid hand"
		enum AnnHandControllerSide : size_t { leftHandController = 0, rightHandController = 1, invalidHandController = 2 };

		///Construct a Controller object
		AnnHandController(Ogre::SceneNode* handNode, AnnHandControllerID controllerID, AnnHandControllerSide controllerSide);

		///Get the side type as a std::string
		inline std::string getSideAsString(AnnHandControllerSide s);

		///Attach a 3D model to the hand. Previously attached model will be detached
		void attachModel(Ogre::Entity* handModel);

		///Detach model without destroying it
		void detachModel();

		///Return the current model :
		Ogre::Entity* getModel();

		///Get position in world space
		AnnVect3 getWorldPosition();

		///Get orientation in world space
		AnnQuaternion getWorldOrientation();

		///Get tracked angular speed
		AnnVect3 getAngularSpeed();

		///Get tracked linear speed
		AnnVect3 getLinearSpeed();

		///Get a vector aligned with the pointing direction
		AnnVect3 getPointingDirection();

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
		bool isTracked();

		bool getButtonState(uint8_t buttonIndex);

		size_t getNbButton();

		bool hasBeenPressed(uint8_t buttonIndex);

		bool hasBeenReleased(uint8_t buttonIndex);

		size_t getNbAxes();
		AnnHandControllerAxis getAxis(size_t index);

	private:

		///Get a reference to the axes vector
		std::vector<AnnHandControllerAxis>& getAxesVector();

		///Get a reference to the button state vector
		std::vector<bool>& getButtonStateVector();

		///Get a reference to the pressed event vector
		std::vector<uint8_t>& getPressedButtonsVector();

		///Get a reference to the released event vector
		std::vector<uint8_t>& getReleasedButtonsVector();

		friend class OgreVRRender;
		friend class OgreOpenVRRender;

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

		std::vector<AnnHandControllerAxis> axes;

		std::vector<bool> buttonsState;

		std::vector<uint8_t> pressedButtons, releasedButtons;
	};
}