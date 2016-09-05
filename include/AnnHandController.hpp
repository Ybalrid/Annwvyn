#pragma once

#include <Ogre.h>
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"


namespace Annwvyn 
{

	typedef size_t AnnHandControllerID;
	class AnnHandController
	{
	public:
		enum AnnHandControllerSide{leftHandController, rightHandController, invalidHandController};
		AnnHandController(Ogre::SceneNode* handNode, AnnHandControllerID controllerID, AnnHandControllerSide controllerSide) :
			node(handNode),
			id(controllerID),
			side(controllerSide),
			model(nullptr),
			grabbed(nullptr),
			tracked(false),
			trackedAngularSpeed(AnnVect3::ZERO),
			trackedLinearSpeed(AnnVect3::ZERO)
		{
			std::cerr << "HandController ID : " << id << " created"; 
			std::cerr << "For side : " << getSideAsString(side);
		}

		inline std::string getSideAsString(AnnHandControllerSide s)
		{
			if (s == leftHandController) return "Left Hand";
			return "Right Hand";
		}

		void attachModel(Ogre::Entity* handModel)
		{
			if(model) node->detachObject(model);
			model = handModel;
			node->attachObject(model);
		}

		void detachModel()
		{
			if (model) node->detachObject(model);
			model = nullptr;
		}

		AnnVect3 getWorldPosition()
		{
			return node->getPosition();
		}

		AnnQuaternion getWorldOrientation()
		{
			return node->getOrientation();
		}

		AnnVect3 getAngularSpeed()
		{
			return trackedAngularSpeed;
		}

		AnnVect3 getLinearSpeed()
		{
			return trackedLinearSpeed;
		}

		AnnVect3 getPointingDirection()
		{
			return node->getOrientation() * AnnVect3::NEGATIVE_UNIT_Z;
		}

		void attachNode(Ogre::SceneNode* grabbedObject)
		{
			if (grabbedObject->getParentSceneNode())
				grabbedObject->getParentSceneNode()->removeChild(grabbedObject);
			node->addChild(grabbedObject);
			grabbed = grabbedObject;
		}

		void setTrackedPosition(AnnVect3 position)
		{
			tracked = true;
			node->setPosition(position);
		}

		void setTrackedOrientation(AnnQuaternion orientation)
		{
			tracked = true;
			node->setOrientation(orientation);
		}

		void setTrackedLinearSpeed(AnnVect3 v)
		{
			tracked = true;
			trackedLinearSpeed = v;
		}

		void setTrackedAngularSpeed(AnnVect3 v)
		{
			tracked = true;
			trackedAngularSpeed = v;
		}

		bool isTracked()
		{
			return tracked;
		}

	private:
		AnnHandControllerID id;
		AnnHandControllerSide side;

		Ogre::SceneNode* node, * grabbed;
		Ogre::Entity* model;
		bool tracked;

		AnnVect3 trackedAngularSpeed; 
		AnnVect3 trackedLinearSpeed;
	};
}
