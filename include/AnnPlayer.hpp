/**
* \file AnnPlayer.hpp
* \brief class that represent the player
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_PLAYER
#define ANN_PLAYER

#include "systemMacro.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"
#include "AnnPlayerActuator.hpp"

#include "euler.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Annwvyn
{
	enum AnnPlayerMode { STANDING, ROOMSCALE };
	class AnnEngine; //pre-declaration of AnnEngine

	///Parameters of the user's VirtualBody
	class DLL bodyParams
	{
		/**
		* That class was a structure. It is kept as a class with public attributes to keep compatibility with legacy code.
		*/
	public:
		///Constructor that handle the default body parameters.
		bodyParams();

		float eyeHeight;
		float walkSpeed;
		float runFactor;
		float turnSpeed;
		float mass;
		AnnVect3 FeetPosition;
		AnnVect3& RoomBase;
		Ogre::Euler Orientation;
		AnnQuaternion HeadOrientation;

		//bullet
		btCollisionShape* Shape;
		btRigidBody* Body;

		AnnVect3 getHeadPosition() const { return FeetPosition + eyeHeight*AnnVect3::UNIT_Y; }
	};

	///Correspondence between array position and walk direction for the "walking" array
	enum walkDirection { forward, backward, left, right };

	///class that represent the player. This is the user's "Virtual body" in the world. It's the object that you have to move and turn to explore the space.
	class DLL AnnPlayer
	{
	public:

		static const AnnVect3 DEFAULT_STARTING_POS;
		static const Ogre::Euler DEFAULT_STARTING_ORIENT;

		///Construct the player object
		AnnPlayer();

		///Destroy the player object
		~AnnPlayer();

		///Prevent modification of physical parameter
		void lockParameters();

		///Permit to know if parameters are locked
		bool isLocked() const;

		///Set the position of the player. If physics is enabled you need to call  AnnEngine::resetPlayerPhysics() to recreate player's body.
		/// \param Position 3D vector representing the position of the player (referenced by the point between his eyes)
		void setPosition(AnnVect3 Position) const;

		///Set body orientation
		/// \param Orientation Euler angle that represent the orientation of the player's BODY. That orientation is used as a "zero point" for the head orientation.
		void setOrientation(Ogre::Euler Orientation) const;

		///Set the head orientation
		/// \param HeadOrientation A quaternion representing the orientation of the head
		void setHeadOrientation(AnnQuaternion HeadOrientation) const;

		///distance between foot-plane and eyes
		/// \param eyeHeight floating point number in meter
		void setEyesHeight(float eyeHeight) const;

		///WalkSpeed, meters by second
		/// \param walkSpeed The speed a the user is walking
		void setWalkSpeed(float walkSpeed) const;

		///Turn-speed
		/// \param turnSpeed Angular speed that the user can turn his body
		void setTurnSpeed(float turnSpeed) const;

		///Mass in Kg
		/// \param mass Mass of the player in Kg
		void setMass(float mass) const;

		///Bullet shape
		/// \param Shape the Bullet collision shape used to simulate player physics
		void setShape(btCollisionShape* Shape);

		///Bullet RIGID body
		/// \param Body The bullet rigid body used for simulating player's physics
		void setBody(btRigidBody* Body);

		///Get the distance between foot-plane and eyes in meters
		float getEyesHeight() const;

		///Get a vector that correspond to the translation between the feet point and the eyes
		AnnVect3 getEyeTranslation() const;

		///Get walk-speed in meter/seconds
		float getWalkSpeed() const;

		///Get turn-speed in rad/seconds
		float getTurnSpeed() const;

		///Get mass in Kg
		float getMass() const;

		///Get position vector
		AnnVect3 getPosition() const;

		///Get body orientation (Euler vector)
		Ogre::Euler getOrientation() const;

		///Get rigid body
		btRigidBody* getBody() const;

		///Get Shape
		btCollisionShape* getShape() const;

		///Apply a relative yaw transform to the player. Useful to bind it to the mouse X axis for FPS-like gameplay.
		/// \param angle Radian angle of the transformation.
		void applyRelativeBodyYaw(Ogre::Radian angle);

		///Apply the rotation from the mouse relative value
		/// \param relValue Relative value in pixels of the mouse linear movement
		void applyMouseRelativeRotation(int relValue);

		///Return true if physics has been initialized once
		bool hasPhysics() const;

		///Get the translation vector (normalized) from the walking state
		AnnVect3 getTranslation();

		///Get the translation from analog joystick value
		AnnVect3 getAnalogTranslation() const;

		///Set the player actuator object
		void setActuator(AnnPlayerActuator* act);

		///Boolean false if the player can get orientation transformation from
		bool standing;
		float mouseSensitivity;
		///Get the ratio between walking and running speed
		float getRunFactor() const;

		///If the player is handled through the physics engine, this method will detach the rigid-body from the camera,
		///remove it from the dynamics world, de-allocate it from the memory and recreate it from scratch. This is useful for
		///"teleporting" the player, for example if you need to reset his position.
		void resetPlayerPhysics();

		///Teleport the player to the given location, and get it facing the given direction
		void teleport(AnnVect3 position, AnnRadian orientation);

		///Teleport the player without touching it's direction
		void teleport(AnnVect3 position);

		///Set the player mode between standing and roomscale;
		void setMode(AnnPlayerMode playerMode);

		///Register the room reference node
		void setRoomRefNode(Ogre::SceneNode* node);

		///Put the reference point for the roomscale VR at Y altitude
		void reground(float YvalueForGround);

		///call reground(pointOnGround.y);
		void reground(AnnVect3 pointOnGround);

		///Shoot a ray form the player to relative -Y. If it hits a rigidbody, call reground() on the impact position
		void regroundOnPhysicsBody(float lenght = 1000, AnnVect3 preoffset = AnnVect3::ZERO);

	protected:

		///Object that keep body parameters (= legacy structure)
		bodyParams* playerBody;

	private:

		AnnPlayerMode mode;

		///Give back the right to modify some parameters
		void unlockParameters();

		///Give Annwvyn::AnnEngine the right to access private members
		friend class AnnEngine;
		friend class AnnPhysicsEngine;

		///Engine update call for each frame
		void engineUpdate(float time);

		///Get the pointer to bodyParams (compatibility with legacy code here. Highly dangerous, do not mess with whatever is pointed by that vector. Seriously.
		bodyParams* getLowLevelBodyParams() const;

		///The famous boolean that permit to prevent YOU for breaking my work! :D
		bool locked;

		///If there is contact with ground
		bool contactWithGround;

		///Apply yaw from analog value
		void applyAnalogYaw();

		///time length of the frame
		double updateTime;

		///Player body physics enabled
		bool physics;

		///PlayerActuator to use
		AnnPlayerActuator* actuator;

		///Room reference node
		Ogre::SceneNode* RoomReferenceNode;

		///If true, will extract the yaw angle from the last head pose, and store it in roomTranslateQuatReference
		bool needNewRoomTranslateReference;

		///Reference for the translation of the room
		AnnQuaternion roomTranslateQuatReference;

	public:

		///Turning that off bypass the physics code. Cool for menu scene or weird manipulation of the player object
		bool ignorePhysics;

		///Waling state. Forward Backward Left Right
		bool walking[4];

		///Running state
		bool run;

		//Analog values between -1 and 1
		float analogWalk;
		float analogStraff;
		float analogRotate;
	};
}
#endif //ANN_PLAYER