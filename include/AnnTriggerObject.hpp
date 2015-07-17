/**
* \file AnnTriggerObject.hpp
* \brief Object for representing a volume that trigger an event
* \author A. Brainville
*/

#ifndef ANNTRIGGEROBJECT
#define ANNTRIGGEROBJECT

#include "systemMacro.h"
#include "AnnVect3.hpp"
#include "AnnPlayer.hpp"
#include "AnnTools.h"

namespace Annwvyn
{
	//Anticipated declaration of AnnEngine class 
	class AnnEngine;
	class AnnPhysicsGameEngine;

	///Object for representing a volume that trigger an event
	class DLL AnnTriggerObject
	{
	public:
		///Class constructor
		AnnTriggerObject();

		///Class destructor
		virtual ~AnnTriggerObject(){}

		///Set position form Vector 3D
		/// \param pos 3D vector positioning the object
		void setPosition(Ogre::Vector3 pos);

		///Set position form Variables
		/// \param x X component of the poisition vector
		/// \param y Y component of the poisition vector
		/// \param z Z component of the poisition vector
		void setPosition(float x, float y, float z);

		///Get position
		Ogre::Vector3 getPosition();

		///Get contact information
		bool getContactInformation();

	private:	
		///For engine : Set contact state 
		/// \param contact Contact state
		void setContactInformation(bool contact);
		
		///Return true if player's head (this is player's trigger point) is inside the trigger volume.
		virtual bool computeVolumetricTest(AnnPlayer* player) = 0;

		friend class AnnEngine;
		friend class AnnPhysicsEngine;

	private:
		///Position of the object
		AnnVect3 m_position;

		///True if trigger triggerd
		bool m_contactWithPlayer;
		bool lastFrameContactWithPlayer;

	public:
		///When contact happened
		virtual void atContact() {return;}
		///After initialization
		virtual void postInit() {return;}
	};

	class DLL AnnSphericalTriggerObject : public AnnTriggerObject
	{
	public:
		AnnSphericalTriggerObject();		
		///GetThreshold distance
		float getThreshold();

		///Set contact information
		/// \param threshold Radius of the "activation sphere" of the trigger"
		void setThreshold(float threshold);

	private:
		bool computeVolumetricTest(AnnPlayer* player);
		///Distance where the trigger is triggered
		float m_threshold;
	};

	///Create a trigger volume that is aligned with the scene referential.
	///Volume is defined by min/max XYZ boundaries
	///This is the lowest load in CPU time
	class DLL AnnAlignedBoxTriggerObject : public AnnTriggerObject
	{
	public:
		AnnAlignedBoxTriggerObject();

		///Set the volume dimentions
		/// \param x1 X minimal plane boundary
		/// \param x2 X maximal plane boundary
		/// \param y1 Y minimal plane boundary
		/// \param y2 Y maximal plane boundary
		/// \param z1 Z minimal plane boundary
		/// \param z2 Z maximal plane boundary
		void setBoundaries(float x1, float x2, float y1, float y2, float z1, float z2);
	private:
		bool computeVolumetricTest(AnnPlayer* player);
		///Boundaries values. All defaults to 0
		float xMin, xMax, yMin, yMax, zMin, zMax;
	};
}


#endif


