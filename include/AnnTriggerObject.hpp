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
		void setOrientation(AnnQuaternion orient) override { return; }

		///Get position
		AnnVect3 getPosition() override;

		///Does nothing
		AnnQuaternion getOrientation() override { return AnnQuaternion::IDENTITY; }

		///Get contact information
		bool getContactInformation();

		///When contact happened
		virtual void atContact() { return; }

	private:

		///For engine : Set contact state
		/// \param contact Contact state
		void setContactInformation(bool contact);

		///Return true if player's head (this is player's trigger point) is inside the trigger volume.
		virtual bool computeVolumetricTest(std::shared_ptr<AnnPlayer> player) = 0;

		friend class AnnEngine;
		friend class AnnGameObjectManager;
		friend class AnnPhysicsEngine;

		///Position of the object
		AnnVect3 position;

		///True if trigger triggers
		bool contactWithPlayer;

		///State of the last frame
		bool lastFrameContactWithPlayer;

		///After initialization
		virtual void postInit() { return; }
	};

	///Trigger volume in the form of a sphere
	class DLL AnnSphericalTriggerObject : public AnnTriggerObject
	{
	public:

		///Construct a spherical trigger. The radius is set with setThreshold
		AnnSphericalTriggerObject();

		///GetThreshold distance
		float getThreshold();

		///Set contact information
		/// \param threshold Radius of the "activation sphere" of the trigger"
		void setThreshold(float threshold);

	private:

		///implement the test on player position
		bool computeVolumetricTest(std::shared_ptr<AnnPlayer> player) override;

		///Distance where the trigger is triggered
		float threshold;
		float squaredThreshold;
	};

	///Create a trigger volume that is aligned with the scene referential.
	class DLL AnnAlignedBoxTriggerObject : public AnnTriggerObject
	{
	///Volume is defined by min/max XYZ boundaries
	///This is the lowest load in CPU time

	public:

		///Create an aligned box trigger on the XYZ references
		AnnAlignedBoxTriggerObject();

		///Set the volume dimensions
		/// \param x1 X minimal plane boundary
		/// \param x2 X maximal plane boundary
		/// \param y1 Y minimal plane boundary
		/// \param y2 Y maximal plane boundary
		/// \param z1 Z minimal plane boundary
		/// \param z2 Z maximal plane boundary
		void setBoundaries(float x1, float x2, float y1, float y2, float z1, float z2);

	private:

		///implement the test on player position
		bool computeVolumetricTest(std::shared_ptr <AnnPlayer> player) override;

		///Boundaries values. All defaults to 0
		float xMin, xMax, yMin, yMax, zMin, zMax;
	};
}

#endif