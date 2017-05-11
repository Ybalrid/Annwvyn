/**
* \file AnnTriggerObject.hpp
* \brief Object for representing a volume that trigger an event
* \author A. Brainville (Ybalrid)
*/

#ifndef ANNTRIGGEROBJECT
#define ANNTRIGGEROBJECT

#include "systemMacro.h"
#include "AnnVect3.hpp"
#include "AnnPlayer.hpp"
#include "AnnAbstractMovable.hpp"

namespace Annwvyn
{
	//Anticipated declaration of AnnEngine class
	class AnnEngine;
	class AnnPhysicsGameEngine;

	///Object for representing a volume that trigger an event
	class DLL AnnTriggerObject : public AnnAbstractMovable
	{
	public:
		///Class constructor
		AnnTriggerObject();

		///Class destructor
		virtual ~AnnTriggerObject();

		///Set position form Vector 3D
		/// \param pos 3D vector positioning the object
		void setPosition(AnnVect3 pos) override;

		///Does nothing
		void setOrientation(AnnQuaternion orient) override;

		///Get position
		AnnVect3 getPosition() override;

		///Does nothing
		AnnQuaternion getOrientation() override;

		///Get contact information
		bool getContactInformation() const;

		///When contact happened
		DEPRECATED virtual void atContact() { return; }

	private:

		///For engine : Set contact state
		/// \param contact Contact state
		void setContactInformation(bool contact);

		friend class AnnEngine;
		friend class AnnGameObjectManager;
		friend class AnnPhysicsEngine;

		///True if trigger triggers
		bool contactWithPlayer;

		///State of the last frame
		bool lastFrameContactWithPlayer;

		///After initialization
		DEPRECATED virtual void postInit() { return; }

		btRigidBody* body;
		btCollisionShape* shape;
	};
}

#endif