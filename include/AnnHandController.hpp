#pragma once

#include <Ogre.h>
#include "systemMacro.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"

namespace Annwvyn
{
	typedef size_t AnnHandControllerID;
	class DLL AnnHandController
	{
	public:
		///Identify the controller as "left hand", "right hand" or "invalid hand"
		enum AnnHandControllerSide { leftHandController, rightHandController, invalidHandController };

		///Construct a Controller object
		AnnHandController(Ogre::SceneNode* handNode, AnnHandControllerID controllerID, AnnHandControllerSide controllerSide);

		///Get the side type as a std::strinng
		inline std::string getSideAsString(AnnHandControllerSide s);

		///Attach a 3D model to the hand. Previously attached model will be detached
		void attachModel(Ogre::Entity* handModel);

		///Detach model without destroying it
		void detachModel();

		///Return the current model :
		Ogre::Entity* getModel();

		///Get position in world space
		AnnVect3 getWorldPosition();

		///Get orientaiton in world space
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

		///Set the orientaiton of the hand
		void setTrackedOrientation(AnnQuaternion orientation);

		///Set the linear velocity of the hand
		void setTrackedLinearSpeed(AnnVect3 v);

		///Set the angular velocity of the hand
		void setTrackedAngularSpeed(AnnVect3 v);

		///Return true if the hand controller object has recived updates from the tracking system
		bool isTracked();

	private:
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

		///Angular velocity (euler?) vector
		AnnVect3 trackedAngularSpeed;

		///Linear velocity vector
		AnnVect3 trackedLinearSpeed;
	};
}