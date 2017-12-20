/**
* \file AnnTriggerObject.hpp
* \brief Object for representing a volume that trigger an event
* \author A. Brainville (Ybalrid)
*/

#pragma once

#include "systemMacro.h"
#include <AnnTypes.h>
#include "AnnAbstractMovable.hpp"

namespace Annwvyn
{
	//Anticipated declaration of AnnEngine class
	class AnnEngine;
	class AnnPhysicsGameEngine;

	///Shape generator. Just a bunch of static methods. You will need to de-alliocate them by hand.
	class AnnDllExport AnnTriggerObjectShapeGenerator
	{
	public:
		///Deleted constructor. This class is just a collection of static methods
		AnnTriggerObjectShapeGenerator() = delete;

		///Generate a box shape. w, h and l are the dimentions on the x, y and z
		static btCollisionShape* box(const float& w, const float& h, const float& l);

		///Generate a sphere shape
		static btCollisionShape* sphere(const float& r);
	};

	///Object for representing a volume that trigger an event
	class AnnDllExport AnnTriggerObject : public AnnAbstractMovable
	{
	public:
		///Class constructor
		AnnTriggerObject(const std::string& name);

		std::string getName() const;

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

		///Set the shape of the object
		void setShape(btCollisionShape* shp);

	private:
		const std::string name;

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

		///Pointer to the body
		std::unique_ptr<btRigidBody> body;

		///Pointer to the shape
		std::unique_ptr<btCollisionShape> shape;
	};
}
